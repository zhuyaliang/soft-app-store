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

void emit_details_complete(SoftAppPkgkit *pkg)
{
	g_signal_emit (pkg, signals[DETAILS_READY], 0);
}
static void
soft_app_pkgkit_dispose (GObject *object)
{
	SoftAppPkgkit *pkg = SOFT_APP_PKGKIT(object);

    g_print ("soft_app_pkgkit_dispose         !!!!!\r\n");
	if (pkg->control != NULL)
        g_object_unref (pkg->control);
    if (pkg->task != NULL)
        g_object_unref (pkg->task);
    if (pkg->cancellable != NULL)
        g_object_unref (pkg->cancellable);
    if (pkg->package_sack != NULL)
        g_object_unref (pkg->package_sack);
    //g_free (pkg);
}

static void
soft_app_pkgkit_class_init (SoftAppPkgkitClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = soft_app_pkgkit_dispose;
	
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
	pkg->task         = pk_task_new ();
	pkg->action       = GPK_ACTION_NONE;	
	pkg->cache_cnt    = 0;	
	pkg->metadata_cnt = 0;

}

SoftAppPkgkit *soft_app_pkgkit_new(void)
{
	SoftAppPkgkit *pkg;
	pkg = g_object_new (SOFT_APP_TYPE_PKGKIT, NULL);
	
	return SOFT_APP_PKGKIT(pkg);
}
