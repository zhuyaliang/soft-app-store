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

#ifndef __APP_STORE_THUMBNAIL_H__
#define __APP_STORE_THUMBNAIL_H__

#include <gtk/gtk.h>
//#include "app-store.h"

     
G_BEGIN_DECLS

#define SOFT_APP_TYPE_THUMBNAIL_TILE      (soft_app_thumbnail_tile_get_type ())
#define SOFT_APP_THUMBNAIL_TILE(object)   (G_TYPE_CHECK_INSTANCE_CAST ((object),\
                                           SOFT_APP_TYPE_THUMBNAIL_TILE,\
                                           SoftAppThumbnailTile))

typedef struct SoftAppThumbnail
{
    char *app_name;
    char *image_name;
    float level;
}SoftAppThumbnail;

typedef struct SoftAppThumbnailTile
{
	GtkFixed	 parent_instance;
    
    GtkWidget   *event_box;
	GtkWidget	*label;
    char        *app_name;
	GtkWidget	*image;
    char        *image_name;   
    GtkWidget   *stars1;
    GtkWidget   *stars2;
    GtkWidget   *stars3;
    GtkWidget   *stars4;
    GtkWidget   *stars5;
    float level;

}SoftAppThumbnailTile;
typedef struct SoftAppThumbnailTileClass
{
	GtkFixedClass	 parent_instance_class;

}SoftAppThumbnailTileClass;

GType             soft_app_thumbnail_tile_get_type           (void) G_GNUC_CONST;

GtkWidget        *soft_app_thumbnail_tile_new                (SoftAppThumbnail *soft_thumbnail);

void              soft_app_star_widget_set_rating            (GtkWidget *stars,
		                                                      float      level);    
G_END_DECLS

#endif
