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

#ifndef __APP_STORE_ROW_H__
#define __APP_STORE_ROW_H__

#define I_KNOW_THE_PACKAGEKIT_GLIB2_API_IS_SUBJECT_TO_CHANGE
#include <packagekit-glib2/packagekit.h>
#include <gtk/gtk.h>
#include <appstream-glib.h>
#include "app-store-message.h"
G_BEGIN_DECLS

#define SOFT_APP_TYPE_ROW                (soft_app_row_get_type ())
#define SOFT_APP_ROW(object)             (G_TYPE_CHECK_INSTANCE_CAST ((object),\
                                          SOFT_APP_TYPE_ROW,\
                                          SoftAppRow))

typedef struct SoftAppRow
{
	GtkListBoxRow	parent_instance;

    SoftAppMessage *Message;
	GtkWidget	   *label_name;
	GtkWidget	   *image;
	GtkWidget      *stars1;
	GtkWidget      *stars2;
	GtkWidget      *stars3;
	GtkWidget      *stars4;
	GtkWidget      *stars5;
	GtkWidget      *label_describe;
	GtkWidget      *label_size;

}SoftAppRow;
typedef struct SoftAppRowClass
{
	GtkListBoxRowClass	 parent_instance_class;

}SoftAppRowClass;

GType             soft_app_row_get_type                (void) G_GNUC_CONST;

GtkWidget        *soft_app_row_new                     (SoftAppMessage *Message);

void              soft_app_row_set_list                (SoftAppRow     *Row,
                                                        SoftAppMessage *Message);

G_END_DECLS

#endif
