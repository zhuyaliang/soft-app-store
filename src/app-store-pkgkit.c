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

static void
GetSoftAppRepoList (PkClient *client, GAsyncResult *res, PackageApp *pkg)
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
void PackageKitNew(PackageApp *pkg)
{
	g_autoptr(GPtrArray) array = NULL;
    pkg->package_sack = pk_package_sack_new ();
    pkg->cancellable  = g_cancellable_new ();
    pkg->control      = pk_control_new ();
    pkg->repos        = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	pkg->task         = pk_task_new ();
	pkg->action       = GPK_ACTION_NONE;	
    pk_client_get_repo_list_async (PK_CLIENT (pkg->task),
                                   pk_bitfield_value (PK_FILTER_ENUM_NONE),
                                   pkg->cancellable,
                                   NULL, NULL,
                                  (GAsyncReadyCallback) GetSoftAppRepoList, pkg);

}    
