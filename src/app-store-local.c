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
	SoftAppMessage *Message;
	GtkWidget *row;
	guint       i;
	app->page = MAIN_PAGE;
    
    for (i = 0; i < app->pkg->list->len; i++)
    {
        Message = SOFT_APP_MESSAGE (g_ptr_array_index (app->pkg->list, i));
		row = soft_app_row_new(Message);
		gtk_widget_set_halign(row, GTK_ALIGN_CENTER);
		gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(row),TRUE);
		gtk_list_box_insert (GTK_LIST_BOX(app->LocalSoftListBox), row, i);
	}
	gtk_widget_show_all(app->MainWindow);
}
static void
GetLocalSoftAppDetails (PkClient       *client, 
                        GAsyncResult   *res, 
                        SoftAppPkgkit  *pkg)
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
    g_auto(GStrv)     split = NULL;
	SoftAppMessage   *Message;
    static int i = 0;

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
    split = pk_package_id_split (package_id);
    if (split == NULL)
    {    
        return;
    }
	Message = soft_app_message_new ();
	soft_app_message_set_pkgid(Message,package_id);
	soft_app_message_set_name(Message,split[PK_PACKAGE_ID_NAME]);
	soft_app_message_set_version(Message,split[PK_PACKAGE_ID_VERSION]);
	soft_app_message_set_arch(Message,split[PK_PACKAGE_ID_ARCH]);
	soft_app_message_set_icon(Message,"/tmp/time-admin.png");
	soft_app_message_set_score(Message,0.5);
    soft_app_message_set_describe(Message,description);
	soft_app_message_set_size(Message,s_size);
	soft_app_message_set_url(Message,url);
	soft_app_message_set_license(Message,license);
	soft_app_message_set_package(Message,package);
    g_print("count get details %d num\r\n",i++);
    g_ptr_array_add (pkg->list, Message);
}    
static void
GetLocalDetailsProgress(PkProgress    *progress, 
                        PkProgressType type, 
                        SoftAppPkgkit *pkg)
{
    PkStatusEnum status;
    gint percentage;
    gboolean allow_cancel;
    static uint soft_sum = 0;

    g_object_get (progress,
                 "status", &status,
                 "percentage", &percentage,
                 "allow-cancel", &allow_cancel,
                  NULL);

    if (type == PK_PROGRESS_TYPE_STATUS)
    {
        if (status == PK_STATUS_ENUM_FINISHED)
        {    
            if(++soft_sum >=pkg->listlen)
            {
	            emit_details_complete(pkg);
            }    
        }
    } 
    else if (type == PK_PROGRESS_TYPE_PERCENTAGE) 
    {
        if (percentage > 0) 
        {
            g_print("percentage = %.2f\r\n",(float) percentage / 100.0f);
        } 
    } 
}

static void
pk_console_package_cb (PkPackage *package, SoftAppPkgkit *pkg)
{
    PkInfoEnum      info;
    const gchar    *package_id;
    g_auto(GStrv)   package_ids = NULL;
    
    /* ignore finished */
    info = pk_package_get_info (package);
    if (info == PK_INFO_ENUM_FINISHED)
        return;

    /* split */
    package_id = pk_package_get_id (package);
/*
    soft_app_local_soft_detalis (Message,
		                         PK_CLIENT(pkg->task),
			                     package_ids,
								 pkg->cancellable,
                                 (GAsyncReadyCallback) GetLocalSoftAppDetails,
                                 pkg);
*/
    package_ids = pk_package_ids_from_id (package_id);
    pk_client_get_details_async (PK_CLIENT(pkg->task), 
                                 package_ids, 
                                 pkg->cancellable,
                                (PkProgressCallback) GetLocalDetailsProgress,
                                 pkg,
                                (GAsyncReadyCallback) GetLocalSoftAppDetails,
                                 pkg);
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

    SoftAppPkgkit *pkg = app->pkg;
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
    pkg->listlen = array->len;
    g_print("array->len = %u\r\n",array->len);
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

static void get_local_soft_details_ready (SoftAppPkgkit *pkg,
                                          SoftAppStore  *app)
{
    g_print("list len = %u\r\n",app->pkg->list->len);
    UpdateLocalInstallPage(app);
	//g_print("get_local_soft_details_ready \r\n");
}
static void GetLocalSoftMessage(SoftAppStore *app)
{
	app->pkg = g_new0 (SoftAppPkgkit, 1);
	app->pkg = soft_app_pkgkit_new();
	app->pkg->list = g_ptr_array_new ();
	g_signal_connect (app->pkg,
                     "details-ready",
                      G_CALLBACK (get_local_soft_details_ready),
                      app);
    pk_task_get_packages_async (app->pkg->task,
                                16777284,
                                app->pkg->cancellable,
                                NULL,NULL,
                                pk_console_finished_cb, app);

}
static void CreateLocalSoftDetails(SoftAppStore *app,SoftAppRow *row)
{
    GtkWidget   *sw;
	SoftAppInfo *info;
	GtkFixed    *details;
	const char  *name;
	const char  *icon;
	const char  *explain;
	const char  *version;
	const char  *license;
	const char  *url;
	const char  *size;
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
    explain = soft_app_message_get_describe(row->Message);
    version = soft_app_message_get_version(row->Message);
    license = soft_app_message_get_license(row->Message);
    url     = soft_app_message_get_url(row->Message);
    size     = soft_app_message_get_size(row->Message);

	info = soft_app_info_new(name);
	soft_app_info_set_icon(info,icon);
	soft_app_info_set_comment(info,"manage local time and time zone");
	soft_app_info_set_button(info,"removed");
	soft_app_info_set_score(info,score);
	soft_app_info_set_screenshot(info,"/tmp/time.png");
	soft_app_info_set_explain(info,explain);
	soft_app_info_set_version(info,version);
	soft_app_info_set_protocol(info,license);
	soft_app_info_set_source(info,url);
	soft_app_info_set_size(info,size);
	
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
	SwitchPage(app);
    CreateLocalSoftDetails(app,row);
}
GtkWidget *LoadLocalInstall(SoftAppStore *app)
{
    GtkWidget *vbox;
    GtkWidget *sw;

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
	sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
	app->LocalSoftListBox = gtk_list_box_new ();
	gtk_container_add (GTK_CONTAINER (sw), app->LocalSoftListBox);
    g_signal_connect (app->LocalSoftListBox, 
                     "row-activated",
                      G_CALLBACK (SwitchPageToIndividualDetailsPage), 
                      app);

	GetLocalSoftMessage(app);
	return vbox;
}
