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

typedef struct {
    const gchar *name;
    const gchar *icon;  
    const gchar *key_colors;
}SoftAppDesktopData;    

static const SoftAppDesktopData msdata[] = 
{
    {("Audio"),        "folder-music-symbolic",             "#215d9c"},
    {("Developer"),    "applications-engineering-symbolic", "#43d7c7"},
    {("Education"),    "system-help-symbolic",              "#5631c7"},
    {("Games"),        "applications-games-symbolic",       "#c4a000"},
    {("Graphics"),     "applications-graphics-symbolic",    "#75507b"},
    {("Productivity"), "text-editor-symbolic",              "#cc0000"},
    {("Communication"),"user-available-symbolic",           "#729fcf"},
    {("Reference"),    "view-dual-symbolic",                "#6a1354"},
    {("Utilities"),    "applications-utilities-symbolic",   "#8fc1ac"},
    { NULL }
};
static void CreateStoreCategoryList(SoftAppStore *app);
static GPtrArray *GetCategoryInfo(SoftAppStore *app)
{
    SoftAppCategory *cate;
    GdkRGBA          key_color;
    gint             i;
    GPtrArray       *list = NULL;
    
    list = g_ptr_array_new ();
    for (i = 0; msdata[i].name != NULL; i++)
    {
        cate = soft_app_category_new (msdata[i].name);
        soft_app_category_set_icon (cate, msdata[i].icon);
        gdk_rgba_parse (&key_color, msdata[i].key_colors);
        soft_app_category_add_key_color (cate, &key_color);
        g_ptr_array_add (list, cate);
    }   

    return list;
}

static GPtrArray *GetRecommendSoftInfo(SoftAppStore *app)
{
    SoftAppThumbnail *thb;
    gint             i;
    GPtrArray       *list = NULL;
    
    list = g_ptr_array_new ();
    for (i = 0; i< 8; i++)
    {
        thb = soft_app_thumbnail_new ();
        soft_app_thumbnail_set_icon  (thb,"/tmp/time-admin.png");
        soft_app_thumbnail_set_name  (thb,_("time-admin"));
        soft_app_thumbnail_set_score (thb,i%5+0.5);
        g_ptr_array_add (list,thb);
    }   

    return list;
}   

static GPtrArray *GetCategoryListInfo(SoftAppStore *app)
{
    SoftAppThumbnail *thb;
    gint             i;
    GPtrArray       *list = NULL;
    
    list = g_ptr_array_new ();
    for (i = 0; i< 100; i++)
    {
        thb = soft_app_thumbnail_new ();
        soft_app_thumbnail_set_icon  (thb,"/tmp/user-admin.png");
        soft_app_thumbnail_set_name  (thb,_("user-admin"));
        soft_app_thumbnail_set_score (thb,i%5+0.5);
        g_ptr_array_add (list,thb);
    }   

    return list;
}   

static GtkWidget *LoadHeader_bar(GtkWidget  *header,
		                         const char *icon,
								 gboolean    mode)
{
    GtkWidget *button;

    button = gtk_button_new (); 
    gtk_container_add (GTK_CONTAINER (button),
                       gtk_image_new_from_icon_name (icon,
                       GTK_ICON_SIZE_BUTTON));

	if(mode)
	{
		gtk_header_bar_pack_start (GTK_HEADER_BAR (header), button);
	}
	else
	{
		gtk_header_bar_pack_end (GTK_HEADER_BAR (header), button);
	}
	return button;
}


static void SwitchPageToReturn (GtkWidget *button, SoftAppStore *app)
{
    app->page = app->parent_page--;
	SwitchPage(app);
}
static void SwitchPageToCategoryListPage (GtkWidget *button, SoftAppStore *app)
{
    app->page = CATEGORY_LIST_PAGE;
	SwitchPage(app);
	CreateStoreCategoryList(app);
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

	SwitchPage(app);
    CreateRecommendDetails(app,SOFT_APP_THUMBNAIL_TILE(button));
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

static void CreateStoreCategoryList(SoftAppStore *app)
{
    GtkWidget *hbox;
	GtkWidget *button;
	GtkWidget *label;
	GtkWidget *ComboBox;
	GtkWidget *sw;
	GtkWidget *flowbox;
    GPtrArray *list;
    GtkWidget *fixed;
    SoftAppThumbnail *thb;
    GtkWidget *Recom;
	guint       i;

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);  
	gtk_box_pack_start(GTK_BOX(app->StackCategoryBox),
                       hbox ,
                       FALSE, 
                       FALSE, 
                       12);
		
	sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), 
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
    
    flowbox = gtk_flow_box_new ();
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX (flowbox), 8);
    gtk_flow_box_set_selection_mode (GTK_FLOW_BOX (flowbox), GTK_SELECTION_NONE);
    gtk_flow_box_set_column_spacing(GTK_FLOW_BOX (flowbox),1);
    gtk_flow_box_set_row_spacing(GTK_FLOW_BOX (flowbox),12);
	gtk_container_add (GTK_CONTAINER (sw), flowbox);
	gtk_box_pack_start(GTK_BOX(app->StackCategoryBox),sw ,TRUE, TRUE, 0);

	list = GetCategoryListInfo(app);
    for (i = 0; i < list->len; i++)
    {    
        thb = SOFT_APP_THUMBNAIL (g_ptr_array_index (list, i));
        Recom = soft_app_thumbnail_tile_new (thb);
        g_signal_connect (Recom, 
                         "clicked",
                          G_CALLBACK (SwitchPageToDetailsPage), 
                          app);
		fixed = gtk_fixed_new();
		gtk_fixed_put(GTK_FIXED(fixed),Recom, 0, 0);
        gtk_container_add (GTK_CONTAINER (flowbox),fixed);
    }
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
    GtkWidget *flowbox;
    GPtrArray *CategoryList;
    GPtrArray *RecommendList;
    GtkWidget *tile;
    SoftAppCategory *cate;
    SoftAppThumbnail *thb;
    GtkWidget *fixed;
    GtkWidget *Recom;
    GtkWidget *hbox;
    GtkWidget *button;
    guint      i;

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);  
    flowbox = CategorySoftWindow(vbox);
    CategoryList = GetCategoryInfo(app);
	
	//button = LoadHeader_bar(app->Header,"edit-find-symbolic",FALSE);
	button = LoadHeader_bar(app->Header,"go-previous-symbolic",TRUE);    	
	g_signal_connect (button, 
                     "clicked",
                      G_CALLBACK (SwitchPageToReturn), 
                      app);
    for (i = 0; i < CategoryList->len; i++)
    {
        cate = SOFT_APP_CATEGORY (g_ptr_array_index (CategoryList, i));
        tile = soft_app_category_tile_new (cate);
		app->page = CATEGORY_LIST_PAGE;
        g_signal_connect (tile, 
                         "clicked",
                          G_CALLBACK (SwitchPageToCategoryListPage), 
                          app);
        gtk_container_add (GTK_CONTAINER (flowbox),tile);
    }

    hbox = RecommendSoftWindow(vbox);
    RecommendList = GetRecommendSoftInfo(app);
    for (i = 0; i < RecommendList->len; i++)
    {    
        thb = SOFT_APP_THUMBNAIL (g_ptr_array_index (RecommendList, i));
        Recom = soft_app_thumbnail_tile_new (thb);
        g_signal_connect (Recom, 
                         "clicked",
                          G_CALLBACK (SwitchPageToDetailsPage), 
                          app);
		fixed = gtk_fixed_new();
		gtk_fixed_put(GTK_FIXED(fixed),Recom, 0, 0);
	    gtk_box_pack_start(GTK_BOX(hbox),fixed ,FALSE, FALSE, 16);
    }

    return vbox;
}
