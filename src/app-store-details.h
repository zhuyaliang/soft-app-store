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

#ifndef __APP_STORE_DETAILS_H__
#define __APP_STORE_DETAILS_H__

#include "app-store.h"
 
G_BEGIN_DECLS
#define SOFT_APP_TYPE_DETAILS                (soft_app_details_get_type ())
#define SOFT_APP_DETAILS(object)             (G_TYPE_CHECK_INSTANCE_CAST ((object),\
                                              SOFT_APP_TYPE_DETAILS,\
                                              SoftAppDetails))

#define SOFT_APP_TYPE_INFO                   (soft_app_info_get_type ())
#define SOFT_APP_INFO(object)                (G_TYPE_CHECK_INSTANCE_CAST ((object),\
                                              SOFT_APP_TYPE_INFO,\
                                              SoftAppInfo))

typedef struct SoftAppInfo
{
    GObject      parent_instance;
	char        *soft_name;
	char        *icon_name;
	char        *comment;
	char        *button_name;
	float       score;
	char        *screenshot;
	char        *explain;
	char        *version;
	char        *protocol;
	char        *source;
	char        *size;
}SoftAppInfo;

typedef struct SoftAppInfoClass
{
    GObjectClass parent_instance_class;
}SoftAppInfoClass;

typedef struct SoftAppDetails
{
    GtkFixed    parent_instance;
   
	SoftAppInfo *info;
    GtkWidget   *soft_image;
	GtkWidget   *label_name;
	GtkWidget   *label_comment;
	GtkWidget   *stars1;
	GtkWidget   *stars2;
	GtkWidget   *stars3;
	GtkWidget   *stars4;
	GtkWidget   *stars5;
	GtkWidget   *button;
	GtkWidget   *screenshot;
	GtkWidget   *explain;
	GtkWidget   *title;
	GtkWidget   *label_version_title;
	GtkWidget   *label_version;
	GtkWidget   *label_protocol_title;
	GtkWidget   *label_protocol;
	GtkWidget   *label_source_title;
	GtkWidget   *label_source;
	GtkWidget   *label_size_title;
	GtkWidget   *label_size;
	GtkWidget   *label_discuss_title;
	GtkWidget   *button_discuss;
}SoftAppDetails;

typedef struct SoftAppDetailsClass
{
    GtkFixedClass parent_instance_class;
}SoftAppDetailsClass;


GType             soft_app_details_get_type         (void) G_GNUC_CONST;

GtkFixed         *soft_app_details_new              (SoftAppInfo *info);

GType             soft_app_info_get_type            (void) G_GNUC_CONST;

SoftAppInfo      *soft_app_info_new                 (const char  *soft_name);

void              soft_app_details_set_info         (SoftAppDetails *details, 
                                                     SoftAppInfo *info);

const char       *soft_app_info_get_name            (SoftAppInfo *info);

void              soft_app_info_set_name            (SoftAppInfo *info,
		                                             const char  *name);

const char       *soft_app_info_get_icon            (SoftAppInfo *info);

void              soft_app_info_set_icon            (SoftAppInfo *info,
		                                             const char  *icon);

const char       *soft_app_info_get_comment         (SoftAppInfo *info);


void              soft_app_info_set_comment         (SoftAppInfo *info,
		                                             const char  *name);
 
float             soft_app_info_get_score           (SoftAppInfo *info);

void              soft_app_info_set_score           (SoftAppInfo *info,
		                                             float        score);

const char       *soft_app_info_get_button          (SoftAppInfo *info);

void              soft_app_info_set_button          (SoftAppInfo *info,
		                                             const char  *button_name);

const char       *soft_app_info_get_screenshot      (SoftAppInfo *info);

void              soft_app_info_set_screenshot      (SoftAppInfo *info,
		                                             const char  *screenshot_name);

const char       *soft_app_info_get_explain         (SoftAppInfo *info);

void              soft_app_info_set_explain         (SoftAppInfo *info,
		                                             const char  *explain);

const char       *soft_app_info_get_version         (SoftAppInfo *info);

void              soft_app_info_set_version         (SoftAppInfo *info,
		                                             const char  *version);

const char       *soft_app_info_get_protocol        (SoftAppInfo *info);

void              soft_app_info_set_protocol        (SoftAppInfo *info,
		                                             const char  *protovol);

const char       *soft_app_info_get_source          (SoftAppInfo *info);

void              soft_app_info_set_source          (SoftAppInfo *info,
		                                             const char  *source);

const char       *soft_app_info_get_size            (SoftAppInfo *info);

void              soft_app_info_set_size            (SoftAppInfo *info,
		                                             const char  *size);

GtkWidget *CreateStoreIndividualDetails (SoftAppStore *app);
GtkWidget *CreateRecommendDetails(SoftAppStore *app,gpointer      data);
G_END_DECLS
#endif
