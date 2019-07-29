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
#include "app-store-local.h"
#include "app-store-row.h"
#include "app-store-details.h"
#include "app-store-pkgkit.h"
 
static void UpdateLocalInstallPage(SoftAppStore *app)
{
    GtkWidget *vbox;
    GtkWidget *sw;
    GPtrArray *list;
	GtkWidget *listbox;
	SoftAppMessage *Message;
	GtkWidget *row;
	guint       i;
    GtkWidget   *NoteName; 
    PackageApp *pkg = app->pkg;

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
	sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
	listbox = gtk_list_box_new ();
	gtk_container_add (GTK_CONTAINER (sw), listbox);
	
    for (i = 0; i < pkg->list->len; i++)
    {
        Message = SOFT_APP_MESSAGE (g_ptr_array_index (pkg->list, i));
		row = soft_app_row_new(Message);
		gtk_widget_set_halign(row, GTK_ALIGN_CENTER);
		gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(row),TRUE);

		app->page = MAIN_PAGE;
		gtk_list_box_insert (GTK_LIST_BOX(listbox), row, i);
	}
    NoteName = gtk_label_new(_("local update"));
    gtk_notebook_append_page(GTK_NOTEBOOK (app->NoteBook),
			                 vbox,
							 NoteName);
	gtk_widget_show_all(app->MainWindow);
}
static void
GetLocalSoftAppDetails (PkClient       *client, 
                        GAsyncResult   *res, 
                        SoftAppMessage *Message)
{
    g_autoptr(PkResults) results = NULL;
    g_autoptr(GError)    error = NULL;
    g_autoptr(PkError)   error_code = NULL;
    g_autoptr(GPtrArray) array = NULL;
    PkDetails        *item;
    guint64           size;
    g_autofree gchar *description = NULL;
    g_autofree gchar *license = NULL;
    g_autofree gchar *package_id = NULL;
    g_autofree gchar *package = NULL;
    g_autofree gchar *summary = NULL;
    g_autofree gchar *url = NULL;
    g_autofree gchar *s_size = NULL;
    PkGroupEnum       group;


    results = pk_client_generic_finish (client, res, &error);
    if (results == NULL) 
    {
        SoftAppStoreLog("Error","failed to get list of categories: %s", error->message);
        return;
    }
    error_code = pk_results_get_error_code (results);
    if (error_code != NULL) 
    {
        SoftAppStoreLog("Error","failed to get details: %s, %s", 
                         pk_error_enum_to_string (pk_error_get_code (error_code)), 
                         pk_error_get_details (error_code));
 
        if (pk_error_get_code (error_code) != PK_ERROR_ENUM_TRANSACTION_CANCELLED) 
        {
            //MessageReport(pk_error_get_code (error_code));
        }
        return;
     }
   
    array = pk_results_get_details_array (results);
    if (array->len != 1) 
    {
        SoftAppStoreLog("Debug","not one entry %u", array->len);
        return;
    }
    item = g_ptr_array_index (array, 0);
    g_object_get (item,
                 "package-id", &package_id,
                 "url", &url,
                 "group", &group,
                 "license", &license,
                 "description", &description,
                 "size", &size,
                  NULL);
    
    s_size = g_strdup_printf ("%.2f",(float)size/(1024*1024));
    package = pk_package_id_to_printable (package_id);
    soft_app_message_set_describe(Message,description);
    g_print("description = %s\r\n",description);
	soft_app_message_set_size(Message,s_size);
	soft_app_message_set_url(Message,url);
	soft_app_message_set_version(Message,license);
	soft_app_message_set_package(Message,package);
}    

static void
pk_console_package_cb (PkPackage *package, PackageApp *pkg)
{
    PkInfoEnum      info;
    const gchar    *package_id;
    g_auto(GStrv)   package_ids = NULL;
    g_auto(GStrv)   split = NULL;
	SoftAppMessage *Message;
    
    /* ignore finished */
    info = pk_package_get_info (package);
    if (info == PK_INFO_ENUM_FINISHED)
        return;

    /* split */
    package_id = pk_package_get_id (package);
    split = pk_package_id_split (package_id);
    if (split == NULL)
    {    
        return;
    }
	Message = soft_app_message_new ();
	soft_app_message_set_name(Message,split[PK_PACKAGE_ID_NAME]);
	soft_app_message_set_version(Message,split[PK_PACKAGE_ID_VERSION]);
	soft_app_message_set_arch(Message,split[PK_PACKAGE_ID_ARCH]);
	soft_app_message_set_icon(Message,"/tmp/time-admin.png");
	soft_app_message_set_score(Message,0.5);
    package_ids = pk_package_ids_from_id (package_id);
    pk_client_get_details_async (PK_CLIENT(pkg->task), 
                                 package_ids, 
                                 pkg->cancellable,
                                (PkProgressCallback) 
                                 NULL,NULL,
                                (GAsyncReadyCallback) GetLocalSoftAppDetails,
                                 Message);
    g_ptr_array_add (pkg->list, Message);

}

static void
pk_console_finished_cb (GObject *object, GAsyncResult *res, gpointer data)
{
    const gchar   *filename;
    GPtrArray     *array;
    PkPackageSack *sack;
    PkRoleEnum     role;
    g_autoptr(GError)    error = NULL;
    g_autoptr(PkError)   error_code = NULL;
    g_autoptr(PkResults) results = NULL;
    SoftAppStore *app = (SoftAppStore *)data;

    PackageApp *pkg = app->pkg;
    results = pk_task_generic_finish (PK_TASK (pkg->task), res, &error);
    if (results == NULL) 
    {
        SoftAppStoreLog ("Error","%s: %s", _("Fatal error"), error->message);
        goto out;
    }
    g_object_get (G_OBJECT(results), "role", &role, NULL);
    error_code = pk_results_get_error_code (results);
    if (error_code != NULL) 
    {
        if (role == PK_ROLE_ENUM_UPDATE_PACKAGES &&
            pk_error_get_code (error_code) == PK_ERROR_ENUM_NO_PACKAGES_TO_UPDATE) 
        {
            SoftAppStoreLog ("Warning","%s", _("There are no packages to update."));
        } 
        else 
        {
			SoftAppStoreLog ("Warning","%s: %s, %s", _("The transaction failed"), 
					          pk_error_enum_to_string (pk_error_get_code (error_code)), 
					          pk_error_get_details (error_code));
        }
        if (pk_error_get_code (error_code) == PK_ERROR_ENUM_NO_PACKAGES_TO_UPDATE)
            goto out;
    }

    /* get the sack */
    sack = pk_results_get_package_sack (results);
    pk_package_sack_sort (sack, PK_PACKAGE_SACK_SORT_TYPE_NAME);
    array = pk_package_sack_get_array (sack);

    /* package */
    filename = g_object_get_data (G_OBJECT (pkg->task), "PkConsole:list-create-filename");
    if ((role != PK_ROLE_ENUM_INSTALL_PACKAGES &&
         role != PK_ROLE_ENUM_UPDATE_PACKAGES &&
         role != PK_ROLE_ENUM_REMOVE_PACKAGES &&
         filename == NULL)) {
        g_ptr_array_foreach (array, (GFunc) pk_console_package_cb, pkg);
    }

    if (array->len == 0 &&
        (role == PK_ROLE_ENUM_GET_UPDATES ||
         role == PK_ROLE_ENUM_UPDATE_PACKAGES)) {
         SoftAppStoreLog ("Error","%s\n", _("There are no updates available at this time."));
         pkg->retval = FALSE;
    }
    
    g_print("list len = %d\r\n",pkg->list->len);
    UpdateLocalInstallPage(app);
    g_ptr_array_unref (array);
    g_object_unref (sack);
/*
    array = pk_results_get_require_restart_array (results);
    g_ptr_array_foreach (array, (GFunc) pk_console_require_restart_cb, ctx);
    g_ptr_array_unref (array);
*/

out:
	g_print("error !!!!!!\r\n");
}


static GPtrArray *GetLocalSoftMessage(SoftAppStore *app)
{
	PackageApp *pkg = app->pkg;
    
	pkg->list = g_ptr_array_new ();
    pk_task_get_packages_async (pkg->task,
                                16777284,
                                pkg->cancellable,
                                NULL,NULL,
                                pk_console_finished_cb, app);

	return pkg->list;
}
static void CreateLocalSoftDetails(SoftAppStore *app,SoftAppRow *row)
{
    GtkWidget   *sw;
	SoftAppInfo *info;
	GtkFixed    *details;
	const char  *name;
	const char  *icon;
	float        score;
	//GtkWidget   *install_button;
	GtkWidget   *install_bar;
	
    soft_app_container_remove_all (GTK_CONTAINER (app->StackDetailsBox));
    sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start (GTK_BOX (app->StackDetailsBox), sw, TRUE, TRUE, 0);
	
	name = soft_app_message_get_name(row->Message);
	icon = soft_app_message_get_icon(row->Message);
	score = soft_app_message_get_score(row->Message);

	info = soft_app_info_new(name);
	soft_app_info_set_icon(info,icon);
	soft_app_info_set_comment(info,"manage local time and time zone");
	soft_app_info_set_button(info,"removed");
	soft_app_info_set_score(info,score);
	soft_app_info_set_screenshot(info,"/tmp/time.png");
	soft_app_info_set_explain(info,"This function simply calls local time zone date ntp sync net time hellow world world hello  get depends on some condition.");
	soft_app_info_set_version(info,"v1.1.1");
	soft_app_info_set_protocol(info,"GPL-3.0");
	soft_app_info_set_source(info,"github.com/zhuyaliang");
	soft_app_info_set_size(info,"12M");
	
	details = soft_app_details_new(info);
    gtk_widget_set_halign (GTK_WIDGET (details), GTK_ALIGN_CENTER);
    gtk_widget_set_valign (GTK_WIDGET (details), GTK_ALIGN_CENTER);
	app->details = SOFT_APP_DETAILS(details);	
    gtk_container_add (GTK_CONTAINER (sw), GTK_WIDGET(details));

    gtk_widget_show_all(app->StackDetailsBox);
	install_bar = soft_app_details_get_bar(SOFT_APP_DETAILS(details));
	gtk_widget_hide(install_bar);
}
static void SwitchPageToIndividualDetailsPage (GtkListBox    *list_box,
                                               GtkListBoxRow *Row,
                                               SoftAppStore  *app)

{
	SoftAppRow *row = SOFT_APP_ROW(Row); 
    app->page = INDIVIDUAL_SOFT_PAGE;
	g_print("abcdefg = %f\r\n",soft_app_message_get_score(row->Message));
	SwitchPage(app);
    CreateLocalSoftDetails(app,row);
}
GtkWidget *LoadLocalInstall(SoftAppStore *app)
{
    GtkWidget *vbox;
    GtkWidget *sw;
    GPtrArray *list;
	GtkWidget *listbox;
	SoftAppMessage *Message;
	GtkWidget *row;
	guint       i;

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
	sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
	listbox = gtk_list_box_new ();
	gtk_container_add (GTK_CONTAINER (sw), listbox);
	
    g_signal_connect (listbox, 
                     "row-activated",
                      G_CALLBACK (SwitchPageToIndividualDetailsPage), 
                      app);
	list = GetLocalSoftMessage(app);
    for (i = 0; i < list->len; i++)
    {
        Message = SOFT_APP_MESSAGE (g_ptr_array_index (list, i));
		row = soft_app_row_new(Message);
		gtk_widget_set_halign(row, GTK_ALIGN_CENTER);
		gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(row),TRUE);

		app->page = MAIN_PAGE;
		gtk_list_box_insert (GTK_LIST_BOX(listbox), row, i);
	}
	return vbox;
}
