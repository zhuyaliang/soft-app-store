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


#define SOFT_APP_TYPE_CATEGORY      (soft_app_category_get_type ())
#define SOFT_APP_CATEGORY(object)   (G_TYPE_CHECK_INSTANCE_CAST ((object),\
                                     SOFT_APP_TYPE_CATEGORY,\
                                     SoftAppCategory))


typedef struct SoftAppCategory
{
    GObject      parent_instance;
    
    char        *soft_name;
    char        *icon_name;
    char        *suburl;
    int          subnum;
    GPtrArray   *key_colors;

}SoftAppCategory;
typedef struct SoftAppCategoryClass
{
    GObjectClass parent_instance_class;
}SoftAppCategoryClass;

typedef struct SoftAppCategoryTile
{
	GtkButton	 parent_instance;

    SoftAppCategory *cate;
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

GType             soft_app_category_tile_get_type           (void) G_GNUC_CONST;
GType             soft_app_category_get_type                (void) G_GNUC_CONST;

GtkWidget        *soft_app_category_tile_new                (SoftAppCategory *soft_cate);

SoftAppCategory  *soft_app_category_new                     (const char *     app_name);

const gchar      *soft_app_category_get_name                (SoftAppCategory *category);

void              soft_app_category_set_name                (SoftAppCategory *category,
                                                             const gchar     *name);

const gchar      *soft_app_category_get_icon                (SoftAppCategory *category);

void              soft_app_category_set_icon                (SoftAppCategory *category,
                                                             const gchar     *icon);

const gchar      *soft_app_category_get_suburl              (SoftAppCategory *category);

void              soft_app_category_set_suburl              (SoftAppCategory *category,
                                                             const gchar     *suburl);

int               soft_app_category_get_subnum              (SoftAppCategory *category);

void              soft_app_category_set_subnum              (SoftAppCategory *category,
                                                             int              subnum);

GPtrArray        *soft_app_category_get_key_colors          (SoftAppCategory *category);


void              soft_app_category_add_key_color           (SoftAppCategory *category,
                                                             const GdkRGBA   *key_color);

void              soft_app_category_tile_set_category       (SoftAppCategoryTile  *tile,
                                                             SoftAppCategory *soft_cate);


G_END_DECLS

#endif
