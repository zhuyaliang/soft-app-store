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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "app-store-util.h"
#include "app-store-search.h"
#include "app-store-row.h"
static void
set_content_request (SoupMessage *message, const char *SoftName)
{
    gchar *data;
    guint length;
    
    data = g_strdup_printf ("{\"keyword\":\"%s\"}",SoftName);
    length = strlen (data);
    soup_message_set_request (message, "application/json", SOUP_MEMORY_TAKE, data, length);
}
static void GetLocalSoftSearchResult (const char *SoftName,SoftAppStore *app)
{
	SoftAppMessage *Message;
	const char *package_id;
	GtkWidget  *row;
	guint       i;
    g_auto(GStrv)     split = NULL;

    for (i = 0; i < app->pkg->list->len; i++)
    {
        Message = SOFT_APP_MESSAGE (g_ptr_array_index (app->pkg->list, i));
		package_id = soft_app_message_get_pkgname (Message);
		split = pk_package_id_split (package_id);

		if (strstr(split[PK_PACKAGE_ID_NAME],SoftName) == NULL)
		{
			continue;
		}
		row = soft_app_row_new(Message);
		gtk_widget_set_halign(row, GTK_ALIGN_CENTER);
		gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(row),TRUE);
		gtk_list_box_insert (GTK_LIST_BOX(app->SearchSoftListBox), row, i);
	}
	gtk_widget_show_all(app->SearchSoftListBox);
}
static void
SoupGetSoftInfo (SoupSession *session,
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
	GtkWidget        *row;
    
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
    soft_app_message_set_mode (Message,FALSE);
    
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
	row = soft_app_row_new(Message);
	gtk_widget_set_halign(row, GTK_ALIGN_CENTER);
	gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(row),TRUE);
	gtk_list_box_insert (GTK_LIST_BOX(app->SearchSoftListBox), row, 1);
	gtk_widget_show_all(app->SearchSoftListBox);
}
static void
SoupGetStoreSearch (SoupSession *session,
                    SoupMessage *msg,
                    gpointer     data)
{
    GPtrArray       *list = NULL;
    json_object     *js;
    const char      *url;
    SoupMessage     *SubMessage;
    
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
    list = GetJsonSubArrayType(msg->response_body->data);
    for (guint i = 0; i < list->len; i++)
    {
        js = g_ptr_array_index (list,i);
        url = GetJsonSpecifiedData (js,"url");
        
	    SubMessage = soup_message_new (SOUP_METHOD_GET,url);
        soup_session_queue_message (app->SoupSession,
								    SubMessage,
								    SoupGetSoftInfo,
								    app);
    }   
	g_ptr_array_free (list, FALSE);
   
}    
static void GetStoreSoftSearchResult (const char *SoftName,SoftAppStore *app)
{
    g_autofree  char *request = NULL;
	SoupMessage *msg;
    SoupSession *sso;

    request = g_strdup_printf ("%s:%d/api/apps/search/",   STORESERVERADDR,STORESERVERPOER);
	sso = soup_session_new ();
	msg = soup_message_new (SOUP_METHOD_POST,request);
    set_content_request (msg, SoftName);
    
    soup_session_queue_message (sso,
								msg,
								SoupGetStoreSearch,
								app);

}    
static void StartSearchSoftName (GtkSearchEntry *entry,SoftAppStore *app)
{
	const char *SoftName;
	int         current_page;

	SoftName = gtk_entry_get_text (GTK_ENTRY(entry));
	if (strlen(SoftName) <= 2)
	{
		return;
	}
	current_page = gtk_notebook_get_current_page (GTK_NOTEBOOK(app->NoteBook));
	if (current_page == 0)
	{
		SoftAppStoreLog ("Debug","start search store soft %s",SoftName);
		soft_app_container_remove_all (GTK_CONTAINER (app->SearchSoftListBox));
		GetStoreSoftSearchResult (SoftName,app);
		SoftAppStoreLog ("Debug","finish Successfu search store soft");
	}
	else if (current_page == 1)
	{
		SoftAppStoreLog ("Debug","start search local soft %s",SoftName);
		soft_app_container_remove_all (GTK_CONTAINER (app->SearchSoftListBox));
		GetLocalSoftSearchResult (SoftName,app);
		SoftAppStoreLog ("Debug","finish Successfu search local soft");
	}
}
static void LoadSearchSoftPage(SoftAppStore *app,int current_page)
{
	GtkWidget *label;
	GtkWidget *entry;
	GtkWidget *container;
	GtkWidget *searchbar;
    GtkWidget *sw;

	label = gtk_label_new (NULL);
	if (current_page == 1)
	{
		SetLableFontType (label,"black",15,_("Search for local packages enter the correct software name"),TRUE);
	}
	else if (current_page == 0)
	{
		SetLableFontType (label,"black",15,_("Search for store packages enter the correct software name"),TRUE);
	}
	else
	{
		return;
	}
    SoftAppStoreLog ("Debug","start search package %d",current_page);
    gtk_box_pack_start (GTK_BOX (app->StackSearchBox), label, FALSE, FALSE, 0);
	entry = gtk_search_entry_new ();
    container = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign (container, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (container), entry, FALSE, FALSE, 0);
    searchbar = gtk_search_bar_new ();
    gtk_search_bar_connect_entry (GTK_SEARCH_BAR (searchbar), GTK_ENTRY (entry));
    gtk_search_bar_set_show_close_button (GTK_SEARCH_BAR (searchbar), FALSE);
    gtk_container_add (GTK_CONTAINER (searchbar), container);
    gtk_box_pack_start (GTK_BOX (app->StackSearchBox), searchbar, FALSE, FALSE, 0);
	gtk_search_bar_set_search_mode (GTK_SEARCH_BAR (searchbar), TRUE);
	g_signal_connect (entry, 
                     "search-changed",
                      G_CALLBACK (StartSearchSoftName), 
                      app);
	
	sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start (GTK_BOX (app->StackSearchBox), sw, TRUE, TRUE, 0);
	app->SearchSoftListBox = gtk_list_box_new ();
	gtk_container_add (GTK_CONTAINER (sw), app->SearchSoftListBox);
    g_signal_connect (app->SearchSoftListBox, 
                     "row-activated",
                      G_CALLBACK (SwitchPageToIndividualDetailsPage), 
                      app);
	
	gtk_widget_show (app->StackSearchBox);
	gtk_widget_show (label);
	gtk_widget_show (entry);
	gtk_widget_show (container);
	gtk_widget_show (searchbar);
	gtk_widget_show (sw);
	gtk_widget_show (app->SearchSoftListBox);
}
void SwitchPageToSearch (GtkWidget *button, SoftAppStore *app)
{
	int        current_page;
	
	current_page = gtk_notebook_get_current_page (GTK_NOTEBOOK(app->NoteBook));
	if (current_page == 0)
	{
		;
	}
	else if (current_page == 1 && app->pkg->action != GPK_ACTION_DONE)
	{
		MessageReport (_("Search soft"),_("The local package was not loaded successfully. Please try again later"),INFOR);
		return;
	}
	else if (current_page == 2)
	{
		return;
	}
	soft_app_container_remove_all (GTK_CONTAINER (app->StackSearchBox));
	app->page = SEARCH_SOFT_PAGE;
	SwitchPage(app);
    LoadSearchSoftPage(app,current_page);
}
