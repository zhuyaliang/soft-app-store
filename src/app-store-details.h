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
#include "app-store-screenshot.h"
 
G_BEGIN_DECLS
#define SOFT_APP_TYPE_DETAILS                (soft_app_details_get_type ())
#define SOFT_APP_DETAILS(object)             (G_TYPE_CHECK_INSTANCE_CAST ((object),\
                                              SOFT_APP_TYPE_DETAILS,\
                                              SoftAppDetails))

typedef struct SoftAppDetails
{
    GtkFixed    parent_instance;
   
    SoftAppMessage *Message;
    GtkWidget   *soft_image;
	GtkWidget   *label_name;
	GtkWidget   *label_comment;
	GtkWidget   *stars1;
	GtkWidget   *stars2;
	GtkWidget   *stars3;
	GtkWidget   *stars4;
	GtkWidget   *stars5;
	GtkWidget   *button;
	GtkWidget   *files_button;
	GtkWidget   *progressbar;
	GtkWidget   *label_progress;
	SoftAppScreenshot *screenshot;
    GtkWidget   *explain;
	GtkWidget   *label_version;
	GtkWidget   *label_protocol;
	GtkWidget   *label_source;
	GtkWidget   *label_size;
	GtkWidget   *label_arch;
	GtkWidget   *label_package;
	GtkWidget   *button_discuss;
}SoftAppDetails;

typedef struct SoftAppDetailsClass
{
    GtkFixedClass parent_instance_class;
}SoftAppDetailsClass;


GType             soft_app_details_get_type         (void) G_GNUC_CONST;

GtkFixed         *soft_app_details_new              (SoftAppMessage *Message);


void              soft_app_details_set_info         (SoftAppDetails *details, 
                                                     SoftAppMessage *Message);

GtkWidget        *soft_app_details_get_button       (SoftAppDetails *details);

GtkWidget        *soft_app_details_get_bar          (SoftAppDetails *details);

void CreateRecommendDetails(gpointer app,gpointer      data);
void SwitchPageToIndividualDetailsPage (GtkListBox    *list_box,
                                        GtkListBoxRow *Row,
                                        gpointer       app);
G_END_DECLS
#endif
