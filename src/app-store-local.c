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
 
static const char *
GetLcocalSoftAppName(const gchar *package_id)
{
	g_autoptr(PkPackage) package = NULL;
	
	package = pk_package_new ();
	pk_package_set_id (package, package_id, NULL);

	return pk_package_get_name(package);
}
static const char *
GetLcocalSoftAppId (PkPackage *item)
{
    gchar *package_id = NULL;
    
	g_object_get (item,
                 "package-id", &package_id,
                  NULL);

	return package_id;

}
static gchar *
pk_strpad (const gchar *data, guint length)
{
    gint size;
    guint data_len;
    g_autofree gchar *padding = NULL;

    if (data == NULL)
        return g_strnfill (length, ' ');

    /* ITS4: ignore, only used for formatting */
    data_len = strlen (data);

    /* calculate */
    size = (length - data_len);
    if (size <= 0)
        return g_strdup (data);

    padding = g_strnfill (size, ' ');
    return g_strdup_printf ("%s%s", data, padding);
}

static void
pk_console_package_cb (PkPackage *package, PackageApp *pkg)
{
    PkInfoEnum info;
    const gchar *package_id;
    g_autofree gchar *info_pad = NULL;
    g_autofree gchar *printable = NULL;
    g_autofree gchar *printable_pad = NULL;
    g_auto(GStrv) split = NULL;

    /* ignore finished */
    info = pk_package_get_info (package);
    if (info == PK_INFO_ENUM_FINISHED)
        return;

    /* split */
    package_id = pk_package_get_id (package);
    split = pk_package_id_split (package_id);
    if (split == NULL)
        return;

    /* make these all the same length */
    info_pad = pk_strpad (pk_info_enum_to_localised_past (info), 12);

    /* create printable */
    printable = g_strdup_printf ("%s-%s.%s (%s)",
                     split[PK_PACKAGE_ID_NAME],
                    split[PK_PACKAGE_ID_VERSION],
                     split[PK_PACKAGE_ID_ARCH],
                     split[PK_PACKAGE_ID_DATA]);


    /* pad the name-version */
    printable_pad = pk_strpad (printable, 60);
    g_print ("lllll%s\t%s\t%s\n", info_pad, printable_pad,
         pk_package_get_summary (package));
}

static void
pk_console_finished_cb (GObject *object, GAsyncResult *res, gpointer data)
{
    const gchar *filename;
    gboolean ret;
    GPtrArray *array;
    PkPackageSack *sack;
    PkRestartEnum restart;
    PkRoleEnum role;
    PackageApp *pkg = (PackageApp *) data;
    g_autoptr(GError) error = NULL;
    g_autoptr(GFile) file = NULL;
    g_autoptr(PkError) error_code = NULL;
    g_autoptr(PkResults) results = NULL;

	
    results = pk_task_generic_finish (PK_TASK (pkg->task), res, &error);
    if (results == NULL) {
        g_print ("%s: %s\n", _("Fatal error"), error->message);
        switch (error->code - 0xff) {
        case PK_ERROR_ENUM_REPO_NOT_AVAILABLE:
            break;
        default:
            break;
        }
        goto out;
    }

    g_object_get (G_OBJECT(results), "role", &role, NULL);
    error_code = pk_results_get_error_code (results);
    if (error_code != NULL) {

        if (role == PK_ROLE_ENUM_UPDATE_PACKAGES &&
            pk_error_get_code (error_code) == PK_ERROR_ENUM_NO_PACKAGES_TO_UPDATE) {
            g_print ("%s\n", _("There are no packages to update."));
        } else {
			g_print ("%s: %s, %s\n", _("The transaction failed"), 
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
/*
    if (array->len == 0 &&
        (role == PK_ROLE_ENUM_GET_UPDATES ||
         role == PK_ROLE_ENUM_UPDATE_PACKAGES)) {
        g_print ("%s\n", _("There are no updates available at this time."));
        ctx->retval = PK_EXIT_CODE_NOTHING_USEFUL;
    }

    g_ptr_array_unref (array);
    g_object_unref (sack);

    array = pk_results_get_transaction_array (results);
    g_ptr_array_foreach (array, (GFunc) pk_console_transaction_cb, ctx);

    if (array->len == 0 && role == PK_ROLE_ENUM_GET_OLD_TRANSACTIONS)
        ctx->retval = PK_EXIT_CODE_NOTHING_USEFUL;

    g_ptr_array_unref (array);

    array = pk_results_get_distro_upgrade_array (results);
    g_ptr_array_foreach (array, (GFunc) pk_console_distro_upgrade_cb, ctx);

    if (array->len == 0 && role == PK_ROLE_ENUM_GET_DISTRO_UPGRADES) {
        g_print ("%s\n", _("There are no upgrades available at this time."));
        ctx->retval = PK_EXIT_CODE_NOTHING_USEFUL;
    }

    g_ptr_array_unref (array);

    array = pk_results_get_category_array (results);
    g_ptr_array_foreach (array, (GFunc) pk_console_category_cb, ctx);

   if (array->len == 0 && role == PK_ROLE_ENUM_GET_CATEGORIES)
        ctx->retval = PK_EXIT_CODE_NOTHING_USEFUL;

    g_ptr_array_unref (array);

    array = pk_results_get_update_detail_array (results);
    g_ptr_array_foreach (array, (GFunc) pk_console_update_detail_cb, ctx);

    if (array->len == 0 && role == PK_ROLE_ENUM_GET_UPDATE_DETAIL)
        ctx->retval = PK_EXIT_CODE_NOTHING_USEFUL;

    g_ptr_array_unref (array);

    array = pk_results_get_repo_detail_array (results);
    g_ptr_array_foreach (array, (GFunc) pk_console_repo_detail_cb, ctx);

    if (array->len == 0 && role == PK_ROLE_ENUM_GET_REPO_LIST)
        ctx->retval = PK_EXIT_CODE_NOTHING_USEFUL;

    g_ptr_array_unref (array);
    array = pk_results_get_require_restart_array (results);
    g_ptr_array_foreach (array, (GFunc) pk_console_require_restart_cb, ctx);
 g_ptr_array_unref (array);

    array = pk_results_get_details_array (results);
    g_ptr_array_foreach (array, (GFunc) pk_console_details_cb, ctx);

    if (array->len == 0 && role == PK_ROLE_ENUM_GET_DETAILS)
        ctx->retval = PK_EXIT_CODE_NOTHING_USEFUL;

    g_ptr_array_unref (array);

    if (role != PK_ROLE_ENUM_DOWNLOAD_PACKAGES) {
        array = pk_results_get_files_array (results);
        g_ptr_array_foreach (array, (GFunc) pk_console_files_cb, ctx);
        g_ptr_array_unref (array);
    }

    restart = pk_results_get_require_restart_worst (results);
    if (restart == PK_RESTART_ENUM_SYSTEM) {
        g_print ("%s\n", _("Please restart the computer to complete the update."));
    } else if (restart == PK_RESTART_ENUM_SESSION) {
        g_print ("%s\n", _("Please logout and login to complete the update."));
    } else if (restart == PK_RESTART_ENUM_SECURITY_SYSTEM) {
        g_print ("%s\n", _("Please restart the computer to complete the update as important security updates have been installed."));
    } else if (restart == PK_RESTART_ENUM_SECURITY_SESSION) {
        g_print ("%s\n", _("Please logout and login to complete the update as important security updates have been installed."));
    }

    if (role == PK_ROLE_ENUM_GET_PACKAGES && filename != NULL) {
        file = g_file_new_for_path (filename);
        ret = pk_package_sack_to_file (sack, file, &error);
        if (!ret) {
            g_print ("%s: %s\n", _("Fatal error"), error->message);
            ctx->retval = PK_EXIT_CODE_TRANSACTION_FAILED;
            goto out;
        }
    }
	*/
out:
	g_print("error !!!!!!\r\n");
}


static GPtrArray *GetLocalSoftMessage(SoftAppStore *app)
{
	guint i;
	SoftAppMessage *Message;
    GPtrArray       *list = NULL;
	g_autoptr(GPtrArray) array = NULL;
	PackageApp *pkg = app->pkg;
	PkPackage *package;
    const gchar *package_id = NULL;
    const gchar *package_name = NULL;
	PkTask *task;	
	task         = pk_task_new ();
	g_print("ssssssssssssssssssssssssssss\r\n");
    pk_task_get_packages_async (task,
                                4,
								// PK_FILTER_ENUM_NOT_INSTALLED,
                                NULL,
                                NULL,NULL,
                                pk_console_finished_cb, pkg);

	list = g_ptr_array_new ();

    for (i = 0; i < 100; i++) 
	{
        //package_id = GetLcocalSoftAppId (package);
		//package_name = GetLcocalSoftAppName(package_id);
		//g_print("name = %s\r\n",package_name);
		Message = soft_app_message_new ();
		soft_app_message_set_name(Message,_("time-admin"));
		soft_app_message_set_icon(Message,"/tmp/time-admin.png");
		soft_app_message_set_score(Message,(i%5)+0.5);
		soft_app_message_set_describe(Message,_("Marks row as changed, causing any state that depends on this to be updated.This affects sorting, filtering and headers."));
		soft_app_message_set_size(Message,"35.6M");
		soft_app_message_set_buttontype(Message,TRUE);
        g_ptr_array_add (list, Message);
	}

	return list;
}
static void CreateLocalSoftDetails(SoftAppStore *app,SoftAppRow *row)
{
    GtkWidget   *sw;
	SoftAppInfo *info;
	GtkFixed    *details;
	const char  *name;
	const char  *icon;
	float        score;
	GtkWidget   *install_button;
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
