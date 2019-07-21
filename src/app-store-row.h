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

#include <gtk/gtk.h>

G_BEGIN_DECLS
#define SOFT_APP_TYPE_MESSAGE            (soft_app_message_get_type ())
#define SOFT_APP_MESSAGE(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object),\
                                          SOFT_APP_TYPE_MESSAGE,\
                                          SoftAppMessage))


#define SOFT_APP_TYPE_ROW                (soft_app_row_get_type ())
#define SOFT_APP_ROW(object)             (G_TYPE_CHECK_INSTANCE_CAST ((object),\
                                          SOFT_APP_TYPE_ROW,\
                                          SoftAppRow))


typedef struct SoftAppMessage
{
    GObject      parent_instance;
    
    char        *soft_name;
    char        *icon_name;
    float        soft_score;
	char        *soft_describe;
	gboolean     soft_type;
	char        *soft_size;
}SoftAppMessage;

typedef struct SoftAppMessageClass
{
    GObjectClass parent_instance_class;
}SoftAppMessageClass;

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
	GtkWidget      *button;
	GtkWidget      *label_size;

}SoftAppRow;
typedef struct SoftAppRowClass
{
	GtkListBoxRowClass	 parent_instance_class;

}SoftAppRowClass;

GType             soft_app_message_get_type            (void) G_GNUC_CONST;
GType             soft_app_row_get_type                (void) G_GNUC_CONST;

SoftAppMessage   *soft_app_message_new                 (void);

GtkWidget        *soft_app_row_new                     (SoftAppMessage *Message);

const gchar      *soft_app_message_get_name            (SoftAppMessage *Message);

void              soft_app_message_set_name            (SoftAppMessage *Message,
                                                        const gchar    *name);

const gchar      *soft_app_message_get_icon            (SoftAppMessage *Message);

void              soft_app_message_set_icon            (SoftAppMessage *Message,
                                                        const gchar    *soft_con);

float             soft_app_message_get_score           (SoftAppMessage *Message);

void              soft_app_message_set_score           (SoftAppMessage *Message,
                                                        float           soft_score);

const gchar      *soft_app_message_get_describe        (SoftAppMessage *Message);

void              soft_app_message_set_describe        (SoftAppMessage *Message,
                                                        const gchar    *soft_describe);

const gchar      *soft_app_message_get_size            (SoftAppMessage *Message);

void              soft_app_message_set_size            (SoftAppMessage *Message,
                                                        const gchar    *soft_size);

gboolean          soft_app_message_get_buttontype      (SoftAppMessage *Message);

void              soft_app_message_set_buttontype      (SoftAppMessage *Message,
                                                        gboolean        soft_type);

void              soft_app_row_set_list                (SoftAppRow     *Row,
                                                        SoftAppMessage *Message);

G_END_DECLS

#endif
