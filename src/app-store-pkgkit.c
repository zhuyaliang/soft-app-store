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
#include "app-store-pkgkit.h"
#include "app-store-row.h"

enum {
        DETAILS_READY,
        LAST_SIGNAL
};
static guint signals[LAST_SIGNAL] = { 0 };
G_DEFINE_TYPE (SoftAppPkgkit, soft_app_pkgkit, SOFT_APP_TYPE_MESSAGE)
static void
GetSoftAppRepoList (PkClient *client, GAsyncResult *res, SoftAppPkgkit *pkg)
{
    g_autoptr(PkResults) results = NULL;
    g_autoptr(GError)    error = NULL;
    g_autoptr(PkError)   error_code = NULL;
    g_autoptr(GPtrArray) array = NULL;
    g_autofree gchar    *repo_id = NULL;
    g_autofree gchar    *description = NULL;
    PkRepoDetail *item;
    guint i;

    /* get the results */
    results = pk_client_generic_finish (client, res, &error);
    if (results == NULL) {
        g_warning ("failed to get list of repos: %s", error->message);
        return;
    }
    /* check error code */
    error_code = pk_results_get_error_code (results);
    if (error_code != NULL) {
        g_warning ("failed to repo list: %s, %s", 
				    pk_error_enum_to_string (pk_error_get_code (error_code)), 
					pk_error_get_details (error_code));

        return;
    }

    array = pk_results_get_repo_detail_array (results);
    for (i = 0; i < array->len; i++) 
	{
        item = g_ptr_array_index (array, i);
        g_object_get (item,
                     "repo-id", &repo_id,
                     "description", &description,
                      NULL);
        if (description != NULL)
            g_hash_table_insert (pkg->repos, g_strdup (repo_id), g_strdup (description));
    }
}

static void soft_app_pkgkit_get_details	(SoftAppMessage *Message,
		                             PkClient       *client,
			                         const char    **package_ids,
								   	 GCancellable   *cancellable,
									 PkProgressCallback progress_callback,
                                     gpointer        progress_user_data,
                                     GAsyncReadyCallback callback_ready,
                                     gpointer        user_data)
{
	g_print("package_ids = %s\r\n",package_ids[0]);
}
void emit_details_complete(SoftAppPkgkit *pkg)
{
	g_signal_emit (pkg, signals[DETAILS_READY], 0);
}
static void
soft_app_pkgkit_dispose (GObject *object)
{
	SoftAppPkgkit *pkg = SOFT_APP_PKGKIT(object);

	if (pkg->control != NULL)
        g_object_unref (pkg->control);
    if (pkg->task != NULL)
        g_object_unref (pkg->task);
    if (pkg->cancellable != NULL)
        g_object_unref (pkg->cancellable);
    if (pkg->package_sack != NULL)
        g_object_unref (pkg->package_sack);
    if (pkg->repos != NULL)
        g_hash_table_destroy (pkg->repos);
    g_free (pkg);
}

static void
soft_app_pkgkit_class_init (SoftAppPkgkitClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	SoftAppMessageClass *message_class = SOFT_APP_MESSAGE_CLASS (klass);

	object_class->dispose = soft_app_pkgkit_dispose;
	message_class->get_local_soft_detalis = soft_app_pkgkit_get_details;
	
	signals [DETAILS_READY] =
             g_signal_new ("details-ready",
                            G_TYPE_FROM_CLASS (klass),
                            G_SIGNAL_RUN_LAST,
                            0,
                            NULL, NULL,
                            g_cclosure_marshal_VOID__VOID,
                            G_TYPE_NONE, 0);
}

static void
soft_app_pkgkit_init (SoftAppPkgkit *pkg)
{
	pkg->package_sack = pk_package_sack_new ();
    pkg->cancellable  = g_cancellable_new ();
    pkg->control      = pk_control_new ();
    pkg->repos        = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	pkg->task         = pk_task_new ();
	pkg->action       = GPK_ACTION_NONE;	
	pkg->cache_cnt    = 0;	
	pkg->metadata_cnt = 0;	
    pk_client_get_repo_list_async (PK_CLIENT (pkg->task),
                                   pk_bitfield_value (PK_FILTER_ENUM_NONE),
                                   pkg->cancellable,
                                   NULL, NULL,
                                  (GAsyncReadyCallback) GetSoftAppRepoList, pkg);


}

SoftAppPkgkit *soft_app_pkgkit_new(void)
{
	SoftAppPkgkit *pkg;
	pkg = g_object_new (SOFT_APP_TYPE_PKGKIT, NULL);
	
	return SOFT_APP_PKGKIT(pkg);
}
