/*  soft-app-store 
*   Copyright (C) 2019  zhuyaliang https://github.com/zhuyaliang/
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.

*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.

*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "app-store-util.h"
#include "app-store-store.h"
#include "app-store-category.h"
#include "app-store-thumbnail.h"
#include "app-store-details.h"
#include "app-store-search.h"

typedef struct {
    const gchar *name;
    const gchar *icon;  
    const gchar *key_colors;
}SoftAppDesktopData;    

static const SoftAppDesktopData msdata[] = 
{
    {("Games"),        "applications-games-symbolic",       "#c4a000"},
    {("Developer"),    "applications-engineering-symbolic", "#43d7c7"},
    {("Audio"),        "folder-music-symbolic",             "#215d9c"},
    {("Education"),    "system-help-symbolic",              "#5631c7"},
    {("Graphics"),     "applications-graphics-symbolic",    "#75507b"},
    {("Productivity"), "text-editor-symbolic",              "#cc0000"},
    {("Communication"),"user-available-symbolic",           "#729fcf"},
    {("Reference"),    "view-dual-symbolic",                "#6a1354"},
    {("Utilities"),    "applications-utilities-symbolic",   "#8fc1ac"},
    { NULL }
};
static void CreateStoreCategoryList(SoftAppStore *app,
                                    SoftAppCategoryTile *tile);
static void SwitchPageToCategoryListPage (GtkWidget *button, SoftAppStore *app)
{
    GtkAdjustment *adj = NULL;
    
	SoftAppCategoryTile *tile = SOFT_APP_CATEGORY_TILE (button);
    app->page = CATEGORY_LIST_PAGE;
	SwitchPage(app);
    soft_app_container_remove_all (GTK_CONTAINER (app->StackCategoryBox));
	CreateStoreCategoryList(app,tile);
    adj = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (app->sw));
    gtk_adjustment_set_value (adj, gtk_adjustment_get_lower (adj));
}
static void SwitchPageToDetailsPage (GtkWidget *button, SoftAppStore *app)
{
	const char *page_name;

	page_name = gtk_stack_get_visible_child_name(GTK_STACK(app->StoreStack));
    if(g_strcmp0(page_name,"main-page") == 0)
	{
		app->parent_page = MAIN_PAGE;
	}
	else if(g_strcmp0(page_name,"list-page") == 0)
	{
		app->parent_page = CATEGORY_LIST_PAGE;
	}
	app->page = INDIVIDUAL_SOFT_PAGE;
    soft_app_container_remove_all (GTK_CONTAINER (app->StackDetailsBox));
	SwitchPage(app);
    CreateRecommendDetails(app,SOFT_APP_THUMBNAIL_TILE(button));
}
static void
SoupGetStoreCategory (SoupSession *session,
                      SoupMessage *msg,
                      gpointer     data)
{
    GPtrArray       *list = NULL;
    guint            i;
    json_object     *js;
    const char      *name;
    const char      *url;
    const char      *subnum;
    SoftAppCategory *cate;
    GdkRGBA          key_color;
    GtkWidget       *tile;
    
    SoftAppStore *app = (SoftAppStore *)data;
    list = g_ptr_array_new ();
    if (msg->status_code == SOUP_STATUS_NOT_MODIFIED)
    {
        SoftAppStoreLog ("Warning","get category SOUP_STATUS_NOT_MODIFIED");
        return;
    }
    if (msg->status_code != SOUP_STATUS_OK)
    {
        SoftAppStoreLog ("Warning","get category"
                         "status code '%u': %s",
                          msg->status_code,
                          msg->reason_phrase);
        return;
    }
    list = GetJsonCategory (msg->response_body->data); 
    for (i = 0; i < list->len; i++)
    {
        js = g_ptr_array_index (list,i);
        name = GetJsonSpecifiedData (js,"name");
        url  = GetJsonSpecifiedData (js,"url");
        subnum  = GetJsonSpecifiedData (js,"count");
        if (name == NULL || url == NULL || atoi(subnum) <= 0)
        {
            continue;
        }    
        cate = soft_app_category_new (name);
        soft_app_category_set_icon (cate, msdata[i % 9].icon);
        gdk_rgba_parse (&key_color, msdata[i % 9].key_colors);
        soft_app_category_add_key_color (cate, &key_color);
        soft_app_category_set_suburl (cate,url);
        soft_app_category_set_subnum (cate,subnum);
        
        tile = soft_app_category_tile_new (cate);
        g_signal_connect (tile, 
                         "clicked",
                          G_CALLBACK (SwitchPageToCategoryListPage), 
                          app);
        gtk_container_add (GTK_CONTAINER (app->StoreFlowbox),tile);
        g_free ((gpointer)subnum);
    }
	g_ptr_array_free (list, TRUE);
    gtk_widget_show_all (app->StoreFlowbox);
}    
static void
SoupGetRichRecomInfo (SoupSession *session,
                      SoupMessage *msg,
                      gpointer     data)
{

    SoftAppMessage   *Message;
    json_object      *js;
    const char       *name;
    const char       *icon;
    const char       *sumary;
    const char       *destination;
    const char       *pkgname;
    const char       *licenses;
    const char       *arch;
    const char       *homepage;
    const char       *screenshot_url;
    const char       *version;
    const char       *size;
    const char		 *package_id;
    const char		 *package;
    GError *error_local = NULL;
	gboolean          state;
	int               score;
    char             *screenshot;
    char             *s_size;
    char             *icon_url;
    GtkWidget        *fixed;
    GtkWidget        *Recom;
    
    SoftAppStore *app = (SoftAppStore *)data;
    if (msg->status_code == SOUP_STATUS_NOT_MODIFIED)
    {
        SoftAppStoreLog ("Warning","get Recommend SOUP_STATUS_NOT_MODIFIED");
        return;
    }
    if (msg->status_code != SOUP_STATUS_OK)
    {
        SoftAppStoreLog ("Warning","get Recommend"
                         "status code '%u': %s",
                          msg->status_code,
                          msg->reason_phrase);
        return;
    }

    js = json_tokener_parse(msg->response_body->data);
    name = GetJsonSpecifiedData (js,"name");
    icon = GetJsonSpecifiedData (js,"icon");
    score = atoi (GetJsonSpecifiedData (js,"score"));
    sumary = GetJsonSpecifiedData (js,"summary");
    destination = GetJsonSpecifiedData (js,"description");
    pkgname = GetJsonSpecifiedData (js,"pkgname");
    licenses = GetJsonSpecifiedData (js,"license");
    arch = GetJsonSpecifiedData (js,"platform");
    size = GetJsonSpecifiedData (js,"size");
    homepage = GetJsonSpecifiedData (js,"homepage");
    version = GetJsonSpecifiedData (js,"version");
    screenshot_url = GetJsonSpecifiedData (js,"screenshots");
    screenshot = g_strdup_printf ("%s:%d/media/%s",STORESERVERADDR,STORESERVERPOER,screenshot_url);
    s_size = g_strdup_printf ("%.2f MB",atof(size) / 1024);
    icon_url = g_strdup_printf ("%s:%d%s",STORESERVERADDR,STORESERVERPOER,icon);
    
    Message = soft_app_message_new ();
    soft_app_message_set_icon  (Message,icon_url);
    soft_app_message_set_name  (Message,name);
    soft_app_message_set_score (Message,score);
    soft_app_message_set_summary (Message,sumary);
    soft_app_message_set_screenshot (Message,screenshot);
    soft_app_message_set_description (Message,destination);
    soft_app_message_set_license (Message,licenses);
    soft_app_message_set_arch (Message,arch);
    soft_app_message_set_size (Message,s_size);
    soft_app_message_set_url (Message,homepage);
    soft_app_message_set_version (Message,version);
    
	pk_bitfield_add (app->Ctx->filters, PK_FILTER_ENUM_INSTALLED);
    package_id = pk_get_resolve_package (app->Ctx,
                                         pkgname,
                                        &error_local);
    pk_bitfield_remove (app->Ctx->filters, PK_FILTER_ENUM_INSTALLED);
	if (package_id == NULL)
	{
		soft_app_message_set_pkgname (Message,"unknow");
		soft_app_message_set_package (Message,"unknow");
	}
	else
	{
		soft_app_message_set_pkgname (Message,package_id);
		package = pk_package_id_to_printable (package_id);
		soft_app_message_set_package (Message,package);
	}
    state = DetermineStoreSoftInstalled (package_id,app);

    Recom = soft_app_thumbnail_tile_new (Message);
    g_signal_connect (Recom, 
                     "clicked",
                      G_CALLBACK (SwitchPageToDetailsPage), 
                      app);
	fixed = gtk_fixed_new();
	gtk_fixed_put(GTK_FIXED(fixed),Recom, 0, 0);
    gtk_box_pack_start(GTK_BOX(app->StoreRecmHbox),fixed ,FALSE, FALSE, 16);

    gtk_widget_show_all (fixed);
	if (!state)
		gtk_widget_hide (SOFT_APP_THUMBNAIL_TILE (Recom)->image_install);
    g_free (screenshot);
    g_free (s_size);
    g_free (icon_url);

}    
static void
SoupGetStoreRecommend (SoupSession *session,
                       SoupMessage *msg,
                       gpointer     data)
{
    GPtrArray       *list = NULL;
    guint            i;
    json_object     *js;
    const char      *url;
    SoupMessage     *SubMessage;
    
    SoftAppStore *app = (SoftAppStore *)data;
    list = g_ptr_array_new ();
    if (msg->status_code == SOUP_STATUS_NOT_MODIFIED)
    {
        SoftAppStoreLog ("Warning","get Recommend SOUP_STATUS_NOT_MODIFIED");
        return;
    }
    if (msg->status_code != SOUP_STATUS_OK)
    {
        SoftAppStoreLog ("Warning","get Recommend"
                         "status code '%u': %s",
                          msg->status_code,
                          msg->reason_phrase);
        return;
    }
	
    list = GetJsonSubArrayType (msg->response_body->data);
    for (i = 0; i < 8; i++)
    {
        js = g_ptr_array_index (list,i);
        url = GetJsonSpecifiedData (js,"url");
        
	    SubMessage = soup_message_new (SOUP_METHOD_GET,url);
        soup_session_queue_message (app->SoupSession,
								    SubMessage,
								    SoupGetRichRecomInfo,
								    app);
    }   
	g_ptr_array_free (list, FALSE);
   
}    
static void
SoupGetRichSubInfo (SoupSession *session,
                    SoupMessage *msg,
                    gpointer     data)
{

    SoftAppMessage   *Message;
    json_object      *js;
    const char       *name;
    const char       *icon;
    const char       *sumary;
    const char       *destination;
    const char       *pkgname;
    const char       *licenses;
    const char       *arch;
    const char       *homepage;
    const char       *screenshot_url;
    const char       *version;
    const char       *size;
	const char       *package_id;
    const char		 *package;
    GError *error_local = NULL;
    int               score;
    char             *screenshot;
    char             *s_size;
    char             *icon_url;
    GtkWidget        *fixed;
    GtkWidget        *Recom;
    gboolean          state;

    SoftAppStore *app = (SoftAppStore *)data;
    if (msg->status_code == SOUP_STATUS_NOT_MODIFIED)
    {
        SoftAppStoreLog ("Warning","get subcategory SOUP_STATUS_NOT_MODIFIED");
        return;
    }
    if (msg->status_code != SOUP_STATUS_OK)
    {
        SoftAppStoreLog ("Warning","get subcategory"
                         "status code '%u': %s",
                          msg->status_code,
                          msg->reason_phrase);
        return;
    }
    js = json_tokener_parse(msg->response_body->data);
    name = GetJsonSpecifiedData (js,"name");
    icon = GetJsonSpecifiedData (js,"icon");
    score = atoi (GetJsonSpecifiedData (js,"score"));
    sumary = GetJsonSpecifiedData (js,"summary");
    destination = GetJsonSpecifiedData (js,"description");
    pkgname = GetJsonSpecifiedData (js,"pkgname");
    licenses = GetJsonSpecifiedData (js,"license");
    arch = GetJsonSpecifiedData (js,"platform");
    size = GetJsonSpecifiedData (js,"size");
    homepage = GetJsonSpecifiedData (js,"homepage");
    version = GetJsonSpecifiedData (js,"version");
    screenshot_url = GetJsonSpecifiedData (js,"screenshots");
    screenshot = g_strdup_printf ("%s:%d/media/%s",STORESERVERADDR,STORESERVERPOER,screenshot_url);
    s_size = g_strdup_printf ("%.2f MB",atof(size) / 1024);
    icon_url = g_strdup_printf ("%s:%d%s",STORESERVERADDR,STORESERVERPOER,icon);
    
    Message = soft_app_message_new ();
    soft_app_message_set_icon  (Message,icon_url);
    soft_app_message_set_name  (Message,name);
    soft_app_message_set_score (Message,score);
    soft_app_message_set_summary (Message,sumary);
    soft_app_message_set_screenshot (Message,screenshot);
    soft_app_message_set_description (Message,destination);
    soft_app_message_set_pkgname (Message,pkgname);
    soft_app_message_set_license (Message,licenses);
    soft_app_message_set_arch (Message,arch);
    soft_app_message_set_size (Message,s_size);
    soft_app_message_set_url (Message,homepage);
    soft_app_message_set_version (Message,version);
    
	pk_bitfield_add (app->Ctx->filters, PK_FILTER_ENUM_INSTALLED);
    package_id = pk_get_resolve_package (app->Ctx,
                                         pkgname,
                                        &error_local);
    pk_bitfield_remove (app->Ctx->filters, PK_FILTER_ENUM_INSTALLED);
	if (package_id == NULL)
	{
		soft_app_message_set_pkgname (Message,"unknow");
		soft_app_message_set_package (Message,"unknow");
	}
	else
	{
		soft_app_message_set_pkgname (Message,package_id);
		package = pk_package_id_to_printable (package_id);
		soft_app_message_set_package (Message,package);
	}
	state = DetermineStoreSoftInstalled (package_id,app);
    soft_app_message_set_state (Message,state); 
    Recom = soft_app_thumbnail_tile_new (Message);
    g_signal_connect (Recom, 
                     "clicked",
                      G_CALLBACK (SwitchPageToDetailsPage), 
                      app);
	fixed = gtk_fixed_new();
    gtk_fixed_put(GTK_FIXED(fixed),Recom, 0, 0);
    gtk_container_add (GTK_CONTAINER (app->SubFlowbox),fixed);
    g_free (screenshot);
    g_free (s_size);
    g_free (icon_url);
    gtk_widget_show_all(fixed);
	if (!state)
		gtk_widget_hide (SOFT_APP_THUMBNAIL_TILE (Recom)->image_install);

}    
static void
SoupGetStoreSubCategory (SoupSession *session,
                         SoupMessage *msg,
                         gpointer     data)
{
    GPtrArray   *list = NULL;
    SoupMessage *SubMessage;
    uint         i;
    json_object *js;
    const char  *url;

    SoftAppStore *app = (SoftAppStore *)data;
    list = g_ptr_array_new ();
    if (msg->status_code == SOUP_STATUS_NOT_MODIFIED)
    {
        SoftAppStoreLog ("Warning","get subcategory SOUP_STATUS_NOT_MODIFIED");
        return;
    }
    if (msg->status_code != SOUP_STATUS_OK)
    {
        SoftAppStoreLog ("Warning","get subcategory"
                         "status code '%u': %s",
                          msg->status_code,
                          msg->reason_phrase);
        return;
    }
    list = GetJsonSubArrayType (msg->response_body->data);
    
    for (i = 0; i < list->len; i++)
    {   
        js = g_ptr_array_index (list,i);
        url = GetJsonSpecifiedData (js,"url");
	    SubMessage = soup_message_new (SOUP_METHOD_GET,url);
        soup_session_queue_message (app->SoupSession,
								    SubMessage,
								    SoupGetRichSubInfo,
								    app);
        
    }    
}    
static GtkWidget *LoadHeader_bar(GtkWidget  *header,
		                         const char *icon,
								 gboolean    mode)
{
    GtkWidget *button;

	if(mode)
	{
		button = gtk_button_new (); 
		gtk_header_bar_pack_start (GTK_HEADER_BAR (header), button);
	}
	else
	{
		button = gtk_toggle_button_new ();
		gtk_header_bar_pack_end (GTK_HEADER_BAR (header), button);
	}
    gtk_container_add (GTK_CONTAINER (button),
                       gtk_image_new_from_icon_name (icon,
                       GTK_ICON_SIZE_BUTTON));
	return button;
}

static void SwitchPageToReturn (GtkWidget *button, SoftAppStore *app)
{
    app->page = app->parent_page--;
	SwitchPage(app);
}

static GtkWidget *CreateSubclassCombo(SoftAppStore *app)
{
	 GtkListStore    *Store;
     GtkTreeIter      Iter;
     GtkCellRenderer *Renderer;
     GtkWidget       *ComboBox;

     Store = gtk_list_store_new(1,G_TYPE_STRING);
     gtk_list_store_append(Store,&Iter);
     gtk_list_store_set(Store,&Iter,0,"test1-class",-1);
     gtk_list_store_append(Store,&Iter);
     gtk_list_store_set(Store,&Iter,0,"test2-class",-1);

     ComboBox = gtk_combo_box_new_with_model(GTK_TREE_MODEL(Store));
     g_object_unref(G_OBJECT(Store));
     Renderer = gtk_cell_renderer_text_new();
     gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(ComboBox),Renderer,TRUE);
     gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(ComboBox),Renderer,"text",0,NULL);

     return ComboBox;

}

static void CreateStoreCategoryList(SoftAppStore *app,
                                    SoftAppCategoryTile *tile)
{
    GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *ComboBox;
    const char *suburl;
    SoupMessage *SubMessage;

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);  
	gtk_box_pack_start(GTK_BOX(app->StackCategoryBox),
                       hbox ,
                       FALSE, 
                       FALSE, 
                       12);
		
	app->sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (app->sw), 
			                        GTK_POLICY_NEVER, 
									GTK_POLICY_AUTOMATIC);
	label = gtk_label_new(NULL);
	SetLableFontType(label,"black",12,_("Subclass"),FALSE);
	gtk_box_pack_start(GTK_BOX(hbox),label ,FALSE, FALSE, 6);
	ComboBox = CreateSubclassCombo(app);
	gtk_widget_set_valign (ComboBox, GTK_ALIGN_START);
	gtk_widget_set_halign (ComboBox, GTK_ALIGN_START);
	gtk_combo_box_set_active(GTK_COMBO_BOX(ComboBox),0);
	gtk_box_pack_start(GTK_BOX(hbox),ComboBox ,FALSE, FALSE, 0);

    label = gtk_label_new(NULL);
	SetLableFontType(label,"black",12,_("sort"),FALSE);
	gtk_box_pack_start(GTK_BOX(hbox),label ,FALSE, FALSE, 6);
	ComboBox = CreateSubclassCombo(app);
	gtk_widget_set_valign (ComboBox, GTK_ALIGN_START);
	gtk_widget_set_halign (ComboBox, GTK_ALIGN_START);
	gtk_combo_box_set_active(GTK_COMBO_BOX(ComboBox),0);
	gtk_box_pack_start(GTK_BOX(hbox),ComboBox ,TRUE, TRUE, 0);
    
    app->SubFlowbox = gtk_flow_box_new ();
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX (app->SubFlowbox), 8);
    gtk_flow_box_set_selection_mode (GTK_FLOW_BOX (app->SubFlowbox), GTK_SELECTION_NONE);
    gtk_flow_box_set_column_spacing(GTK_FLOW_BOX (app->SubFlowbox),1);
    gtk_flow_box_set_row_spacing(GTK_FLOW_BOX (app->SubFlowbox),12);
	gtk_container_add (GTK_CONTAINER (app->sw), app->SubFlowbox);
	gtk_box_pack_start(GTK_BOX(app->StackCategoryBox),app->sw ,TRUE, TRUE, 0);
    
    suburl = soft_app_category_get_suburl (tile->cate);
	SubMessage = soup_message_new (SOUP_METHOD_GET,suburl);
    soup_session_queue_message (app->SoupSession,
								SubMessage,
								SoupGetStoreSubCategory,
								app);
    gtk_widget_show_all(app->StackCategoryBox);
}   

static GtkWidget *CategorySoftWindow(GtkWidget *vbox)
{
	GtkWidget *label;
    GtkWidget *flowbox;
    	
	label = gtk_label_new(NULL);
	gtk_widget_set_halign (label, GTK_ALIGN_START);
	gtk_widget_set_valign (label, GTK_ALIGN_END);
	SetLableFontType(label,"black",14,_("soft category"),TRUE);
	gtk_box_pack_start(GTK_BOX(vbox),label ,TRUE, TRUE, 0);
    
    flowbox = gtk_flow_box_new ();
    gtk_widget_set_valign (flowbox, GTK_ALIGN_START);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX (flowbox), 3);
    gtk_flow_box_set_selection_mode (GTK_FLOW_BOX (flowbox), GTK_SELECTION_NONE);
    gtk_flow_box_set_column_spacing(GTK_FLOW_BOX (flowbox),12);
    gtk_flow_box_set_row_spacing(GTK_FLOW_BOX (flowbox),12);
	gtk_box_pack_start(GTK_BOX(vbox),flowbox ,TRUE, TRUE, 0);
   
    return flowbox;
}   

static GtkWidget *RecommendSoftWindow (GtkWidget *vbox)
{
	GtkWidget *label;
    GtkWidget *hbox;
    
    label = gtk_label_new(NULL);
	gtk_widget_set_halign (label, GTK_ALIGN_START);
	gtk_widget_set_valign (label, GTK_ALIGN_END);
	SetLableFontType(label,"black",14,_("soft recommend"),TRUE);
	gtk_box_pack_start(GTK_BOX(vbox),label ,TRUE, TRUE, 0);
    
    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_box_pack_start(GTK_BOX(vbox),hbox ,TRUE, TRUE, 0);

    return hbox;

}    
GtkWidget *LoadStoreSoft(SoftAppStore *app)
{
    GtkWidget *vbox;
    GtkWidget *button_return;
    GtkWidget *button_search;
	char      *request;
	SoupMessage *msg;
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);  

    InitStorePkCtx (app);
    request = g_strdup_printf ("%s:%d/api/apps/category/",STORESERVERADDR,STORESERVERPOER);
	app->SoupSession = soup_session_new ();
	app->SoupMessage = soup_message_new (SOUP_METHOD_GET,request);
	
	button_search = LoadHeader_bar(app->Header,"edit-find-symbolic",FALSE);
	g_signal_connect (button_search, 
                     "clicked",
                      G_CALLBACK (SwitchPageToSearch),
					  app);
	app->button_search = button_search;
	button_return = LoadHeader_bar(app->Header,"go-previous-symbolic",TRUE);    	
	g_signal_connect (button_return, 
                     "clicked",
                      G_CALLBACK (SwitchPageToReturn), 
                      app);
    app->button_return = button_return;	
	app->StoreFlowbox = CategorySoftWindow(vbox);

    soup_session_queue_message (app->SoupSession,
								app->SoupMessage,
								SoupGetStoreCategory,
								app);
   								
	g_free (request);

	request = g_strdup_printf ("%s:%d/api/apps/recommend/",STORESERVERADDR,STORESERVERPOER);
	msg = soup_message_new (SOUP_METHOD_GET,request);
    
	app->StoreRecmHbox = RecommendSoftWindow(vbox);
	soup_session_queue_message (app->SoupSession,
								msg,
								SoupGetStoreRecommend,
								app);
	g_free (request);

    gtk_widget_show_all (app->StoreRecmHbox);

    return vbox;
}


static void soft_app_install_progress_cb (PkProgress     *progress, 
		                                 PkProgressType  type, 
								   	     SoftAppStore   *app)
{
    PkStatusEnum status;
    gint percentage;
    gboolean allow_cancel;
    GtkWidget *button;

    g_object_get (progress,
                 "status", &status,
                 "percentage", &percentage,
                 "allow-cancel", &allow_cancel,
                  NULL);

    if (type == PK_PROGRESS_TYPE_STATUS)
    {
        if (status == PK_STATUS_ENUM_FINISHED)
		{
			gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(app->details->progressbar),
										  1.00);
            gtk_widget_hide (app->details->progressbar);
            button = soft_app_details_get_button (app->details);
            soft_app_info_set_state (app->details->info,TRUE);
            gtk_button_set_label (GTK_BUTTON(button),_("installed"));
            gtk_widget_show (button);
		}
	} 
    else if (type == PK_PROGRESS_TYPE_PERCENTAGE) 
    {
        if (percentage > 0) 
        {
			gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(app->details->progressbar),
										 (float) percentage / 100.0f);
		}
	}	
}

static void
soft_app_install_finished_cb (PkTask *task, GAsyncResult *res, SoftAppStore *app)
{
    g_autoptr(PkResults) results = NULL;
    g_autoptr(GError)    error = NULL;
    g_autoptr(PkError)   error_code = NULL;

    /* get the results */
    results = pk_task_generic_finish (task, res, &error);
    if (results == NULL) 
    {
        SoftAppStoreLog ("Warning","failed to remove packages: %s", error->message);
        return;
    }

    /* check error code */
    error_code = pk_results_get_error_code (results);
    if (error_code != NULL) 
	{
		SoftAppStoreLog ("failed to remove packages: %s, %s", 
				          pk_error_enum_to_string (pk_error_get_code (error_code)), 
						  pk_error_get_details (error_code));

        return;
    }
}
void InstallStoreSoftApp (GtkWidget *button,SoftAppStore *app)
{    
	g_auto(GStrv) package_ids = NULL;
    const gchar  *package_id = NULL;
    
    if (soft_app_info_get_state(app->details->info))
    {
        return;
    }    
	package_id = soft_app_info_get_package(app->details->info);
    if (package_id == NULL)
    {
        MessageReport (_("install"),_("No package name, maybe the back end is not set."),ERROR);
        return; 
    }    
    if (!pk_bitfield_contain (app->Ctx->filters, PK_FILTER_ENUM_ARCH) &&
        !pk_bitfield_contain (app->Ctx->filters, PK_FILTER_ENUM_NOT_ARCH))
        pk_bitfield_add (app->Ctx->filters, PK_FILTER_ENUM_ARCH);

    if (!pk_bitfield_contain (app->Ctx->filters, PK_FILTER_ENUM_SOURCE) &&
        !pk_bitfield_contain (app->Ctx->filters, PK_FILTER_ENUM_NOT_SOURCE))
        pk_bitfield_add (app->Ctx->filters, PK_FILTER_ENUM_NOT_SOURCE);

    pk_bitfield_add (app->Ctx->filters, PK_FILTER_ENUM_NOT_INSTALLED);
    pk_bitfield_add (app->Ctx->filters, PK_FILTER_ENUM_NEWEST);
    
    package_ids = PackageNameToPackageids (package_id,app);
    if (package_ids == NULL)
    {
        MessageReport (_("install"),_("No suitable package can be found in the system warehouse.\
        Please update the warehouse."),ERROR);
        return;
    }
    pk_bitfield_remove (app->Ctx->filters,PK_FILTER_ENUM_NEWEST);
    pk_bitfield_remove (app->Ctx->filters,PK_FILTER_ENUM_NOT_INSTALLED);
    pk_bitfield_remove (app->Ctx->filters,PK_FILTER_ENUM_NOT_SOURCE);
    pk_bitfield_remove (app->Ctx->filters,PK_FILTER_ENUM_ARCH);
    
	gtk_widget_hide(button);
	gtk_widget_show(app->details->progressbar);
   /* install */
    pk_task_install_packages_async (PK_TASK (app->Ctx->task),
                                    package_ids, app->Ctx->cancellable,
                                    (PkProgressCallback) soft_app_install_progress_cb, app,
                                    (GAsyncReadyCallback)soft_app_install_finished_cb, app);

}    
