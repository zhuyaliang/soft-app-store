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

#ifndef __APP_STORE_THUMBNAIL_H__
#define __APP_STORE_THUMBNAIL_H__

#include <gtk/gtk.h>
     
G_BEGIN_DECLS

#define SOFT_APP_TYPE_THUMBNAIL           (soft_app_thumbnail_get_type ())
#define SOFT_APP_THUMBNAIL(object)        (G_TYPE_CHECK_INSTANCE_CAST ((object),\
                                           SOFT_APP_TYPE_THUMBNAIL,\
                                           SoftAppThumbnail))

#define SOFT_APP_TYPE_THUMBNAIL_TILE      (soft_app_thumbnail_tile_get_type ())
#define SOFT_APP_THUMBNAIL_TILE(object)   (G_TYPE_CHECK_INSTANCE_CAST ((object),\
                                           SOFT_APP_TYPE_THUMBNAIL_TILE,\
                                           SoftAppThumbnailTile))
typedef struct SoftAppThumbnail
{
    GObject parent_instance;
    char   *soft_name;
    char   *icon_name;
    char   *description;
    char   *sumary;
    char   *pkgname;
    char   *version;
    char   *licenses;
    char   *arch;
    char   *size;
    char   *homepage;
    char   *screenshot_url;
    float   soft_score;
}SoftAppThumbnail;

typedef struct SoftAppThumbnailClass
{
    GObjectClass parent_instance_calss;
}SoftAppThumbnailClass;

typedef struct SoftAppThumbnailTile
{
	GtkButton	 parent_instance;
    
    SoftAppThumbnail *thb;
    GtkWidget   *event_box;
	GtkWidget	*label;
	GtkWidget	*image;
    GtkWidget   *stars1;
    GtkWidget   *stars2;
    GtkWidget   *stars3;
    GtkWidget   *stars4;
    GtkWidget   *stars5;

}SoftAppThumbnailTile;
typedef struct SoftAppThumbnailTileClass
{
	GtkButtonClass	 parent_instance_class;

}SoftAppThumbnailTileClass;

GType             soft_app_thumbnail_tile_get_type           (void) G_GNUC_CONST;
GType             soft_app_thumbnail_get_type                (void) G_GNUC_CONST;

GtkWidget        *soft_app_thumbnail_tile_new                (SoftAppThumbnail *soft_thumbnail);
SoftAppThumbnail *soft_app_thumbnail_new                     (void);

void              soft_app_star_widget_set_rating            (GtkWidget        *stars,
		                                                      float             level);   

const gchar      *soft_app_thumbnail_get_name                (SoftAppThumbnail *thb);

void              soft_app_thumbnail_set_name                (SoftAppThumbnail *thb,
                                                              const gchar      *name);

const gchar      *soft_app_thumbnail_get_icon                (SoftAppThumbnail *thb);

void              soft_app_thumbnail_set_icon                (SoftAppThumbnail *thb,
                                                              const gchar      *soft_icon);

const gchar      *soft_app_thumbnail_get_sumary              (SoftAppThumbnail *thb);

void              soft_app_thumbnail_set_sumary              (SoftAppThumbnail *thb,
                                                              const gchar      *sumary);

const gchar      *soft_app_thumbnail_get_description         (SoftAppThumbnail *thb);

void              soft_app_thumbnail_set_description         (SoftAppThumbnail *thb,
                                                              const gchar      *description);

const gchar      *soft_app_thumbnail_get_pkgname             (SoftAppThumbnail *thb);

void              soft_app_thumbnail_set_pkgname             (SoftAppThumbnail *thb,
                                                              const gchar      *pkgname);

const gchar      *soft_app_thumbnail_get_arch                (SoftAppThumbnail *thb);

void              soft_app_thumbnail_set_arch                (SoftAppThumbnail *thb,
                                                              const gchar      *arch);

const gchar      *soft_app_thumbnail_get_size                (SoftAppThumbnail *thb);

void              soft_app_thumbnail_set_size                (SoftAppThumbnail *thb,
                                                              const gchar      *size);

const gchar      *soft_app_thumbnail_get_version             (SoftAppThumbnail *thb);

void              soft_app_thumbnail_set_version             (SoftAppThumbnail *thb,
                                                              const gchar      *version);

const gchar      *soft_app_thumbnail_get_licenses            (SoftAppThumbnail *thb);

void              soft_app_thumbnail_set_licenses            (SoftAppThumbnail *thb,
                                                              const gchar      *licenses);

const gchar      *soft_app_thumbnail_get_homepage            (SoftAppThumbnail *thb);

void              soft_app_thumbnail_set_homepage            (SoftAppThumbnail *thb,
                                                              const gchar      *homepage);

const gchar      *soft_app_thumbnail_get_screenurl           (SoftAppThumbnail *thb);

void              soft_app_thumbnail_set_screenurl           (SoftAppThumbnail *thb,
                                                              const gchar      *screenshot_url);

float             soft_app_thumbnail_get_score               (SoftAppThumbnail *thb);

void              soft_app_thumbnail_set_score               (SoftAppThumbnail *thb,
                                                              float             soft_score);
G_END_DECLS

#endif
