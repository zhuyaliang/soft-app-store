/*  soft-app-store 
*   Copyright (C) 2018  zhuyaliang https://github.com/zhuyaliang/
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

#ifndef __APP_STORE_PKGKIT_H__
#define __APP_STORE_PKGKIT_H__

#define I_KNOW_THE_PACKAGEKIT_GLIB2_API_IS_SUBJECT_TO_CHANGE
#include <packagekit-glib2/packagekit.h>

typedef enum {
    GPK_ACTION_NONE,
    GPK_ACTION_INSTALL,
    GPK_ACTION_REMOVE,
    GPK_ACTION_UNKNOWN
} GpkActionMode;
typedef struct PackageApp
{
    GHashTable      *repos;
    PkBitfield       filters_current;
    PkBitfield       groups;
    PkBitfield       roles;
    PkControl       *control;
    PkPackageSack   *package_sack;
    PkStatusEnum     status_last;
    GCancellable    *cancellable;
	GpkActionMode    action;
	PkTask          *task;
    GPtrArray       *list;
    gint             retval;

}PackageApp;
void            PackageKitNew(PackageApp *pkg);
#endif
