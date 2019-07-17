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

#ifndef __APP_STORE_CATEGORY_H__
#define __APP_STORE_CATEGORY_H__

#include <gtk/gtk.h>
//#include "app-store.h"

G_BEGIN_DECLS
#define SOFT_APP_TYPE_CATEGORY_TILE      (soft_app_category_tile_get_type ())
#define SOFT_APP_CATEGORY_TILE(object)   (G_TYPE_CHECK_INSTANCE_CAST ((object),\
                                          SOFT_APP_TYPE_CATEGORY_TILE,\
                                          SoftAppCategoryTile))

typedef struct SoftAppCategory
{
    char *soft_name;
    char *image_name;
    char *color;
}SoftAppCategory;

typedef struct SoftAppCategoryTile
{
	GtkButton	 parent_instance;
	GtkWidget	*label;
    char        *soft_name;
	GtkWidget	*image;
    char        *image_name;   
	char	    *color;

}SoftAppCategoryTile;
typedef struct SoftAppCategoryTileClass
{
	GtkButtonClass	 parent_instance_class;

}SoftAppCategoryTileClass;
//G_DECLARE_FINAL_TYPE (SoftAppCategoryTile, soft_app_category_tile, SOFT_APP, CATEGORY_TILE, GtkButton)

GType             soft_app_category_tile_get_type           (void) G_GNUC_CONST;

GtkWidget        *soft_app_category_tile_new                (SoftAppCategory *soft_cate);


void              soft_app_category_tile_set_category       (SoftAppCategoryTile  *tile,
                                                             SoftAppCategory *soft_cate);
    
G_END_DECLS

#endif
