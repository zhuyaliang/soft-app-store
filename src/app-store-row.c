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

#include "app-store-row.h"
#include "app-store-util.h"
#include "app-store-thumbnail.h"

G_DEFINE_TYPE (SoftAppRow,     soft_app_row,     GTK_TYPE_LIST_BOX_ROW)
G_DEFINE_TYPE (SoftAppMessage, soft_app_message, G_TYPE_OBJECT)

static void
soft_app_row_refresh (SoftAppRow *row)
{
	GdkPixbuf *pixbuf;
	float      level;
	const char *icon_name;

	icon_name = soft_app_message_get_icon(row->Message);
	pixbuf = gdk_pixbuf_new_from_file(icon_name,NULL);
	soft_app_image_set_from_pixbuf(GTK_IMAGE(row->image),pixbuf,50);

	SetLableFontType(row->label_name,
                    "black",
                     10,
                     soft_app_message_get_name (row->Message),
                     TRUE);
	
	level = soft_app_message_get_score(row->Message);

    soft_app_star_widget_set_rating (row->stars1,level--);
    soft_app_star_widget_set_rating (row->stars2,level--);
    soft_app_star_widget_set_rating (row->stars3,level--);
    soft_app_star_widget_set_rating (row->stars4,level--);
    soft_app_star_widget_set_rating (row->stars5,level--);

	SetLableFontType(row->label_describe,
                    "black",
                     10,
                     soft_app_message_get_describe (row->Message),
                     FALSE);
	
	if(soft_app_message_get_buttontype(row->Message))
	{
		gtk_button_set_label(GTK_BUTTON(row->button),_("uninstall"));
	}
	
	SetLableFontType(row->label_size,
                    "black",
                     10,
                     soft_app_message_get_size (row->Message),
                     TRUE);
}

void
soft_app_row_set_list (SoftAppRow *row, SoftAppMessage *Message)
{
	g_set_object (&row->Message, Message);
    soft_app_row_refresh (row);
}

static void
soft_app_row_destroy (GtkWidget *widget)
{
	SoftAppRow *row = SOFT_APP_ROW (widget);
    g_clear_object (&row->Message);
}

static void
soft_app_row_init (SoftAppRow *row)
{
    GtkWidget *box;
    GtkWidget *hbox;
    GtkWidget *vbox;
    GtkWidget *v_hbox;
    GtkWidget *h_vbox;
	
	gtk_widget_set_has_window (GTK_WIDGET (row), FALSE);

    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);  
    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);  
	gtk_box_pack_start(GTK_BOX(box),hbox ,FALSE,FALSE, 10);
    gtk_container_add (GTK_CONTAINER (row), box);
    
    row->image = gtk_image_new();
	gtk_box_pack_start(GTK_BOX(hbox),row->image ,FALSE,FALSE, 10);
    
    vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);  
	gtk_box_pack_start(GTK_BOX(hbox),vbox ,FALSE,FALSE, 10);

    row->label_name = gtk_label_new(NULL);
	gtk_box_pack_start(GTK_BOX(vbox),row->label_name ,FALSE,FALSE, 6);
    
    v_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);  
	gtk_box_pack_start(GTK_BOX(vbox),v_hbox ,FALSE,FALSE, 0);
	
	row->stars1 = gtk_image_new();
	gtk_box_pack_start(GTK_BOX(v_hbox),row->stars1 ,TRUE, TRUE, 0);
    row->stars2 = gtk_image_new();
	gtk_box_pack_start(GTK_BOX(v_hbox),row->stars2 ,TRUE, TRUE, 0);
    row->stars3 = gtk_image_new();
	gtk_box_pack_start(GTK_BOX(v_hbox),row->stars3 ,TRUE, TRUE, 0);
    row->stars4 = gtk_image_new();
	gtk_box_pack_start(GTK_BOX(v_hbox),row->stars4 ,TRUE, TRUE, 0);
    row->stars5 = gtk_image_new();
	gtk_box_pack_start(GTK_BOX(v_hbox),row->stars5 ,TRUE, TRUE, 0);
    
	row->label_describe = gtk_label_new(NULL);
	gtk_label_set_max_width_chars(GTK_LABEL(row->label_describe),70);
	gtk_widget_set_size_request(row->label_describe,100,10);
	gtk_label_set_line_wrap(GTK_LABEL(row->label_describe),TRUE);
	gtk_label_set_lines(GTK_LABEL(row->label_describe),2);
	gtk_box_pack_start(GTK_BOX(hbox),row->label_describe ,FALSE,FALSE, 6);

    h_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);  
	gtk_box_pack_start(GTK_BOX(hbox),h_vbox ,FALSE,FALSE, 6);

	row->button = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(h_vbox),row->button ,TRUE,TRUE, 6);
	
	row->label_size = gtk_label_new(NULL);
	gtk_box_pack_start(GTK_BOX(h_vbox),row->label_size ,FALSE,FALSE, 6);
	gtk_box_pack_start(GTK_BOX(box), gtk_separator_new (GTK_ORIENTATION_HORIZONTAL) ,FALSE,FALSE, 6);

}

static void
soft_app_row_class_init (SoftAppRowClass *klass)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	widget_class->destroy = soft_app_row_destroy;
}

GtkWidget *
soft_app_row_new (SoftAppMessage *Message)
{
	SoftAppRow *row;

	row = g_object_new (SOFT_APP_TYPE_ROW, NULL);
	soft_app_row_set_list (row, Message);

	return GTK_WIDGET (row);
}

gboolean
soft_app_message_get_buttontype (SoftAppMessage *Message)
{
    return Message->soft_type;
}

void
soft_app_message_set_buttontype (SoftAppMessage  *Message, 
                                 gboolean         soft_type)
{
    Message->soft_type = soft_type;
}

float
soft_app_message_get_score (SoftAppMessage *Message)
{
    return Message->soft_score;
}

void
soft_app_message_set_score (SoftAppMessage  *Message, 
                           float             soft_score)
{
    Message->soft_score = soft_score;
}  

const gchar*
soft_app_message_get_size (SoftAppMessage *Message)
{
    return Message->soft_size;
}

void
soft_app_message_set_size (SoftAppMessage  *Message, 
                           const gchar     *soft_size)
{
    g_free (Message->soft_size);
    Message->soft_size = g_strdup (soft_size);
}  

const gchar *
soft_app_message_get_describe (SoftAppMessage *Message)
{
    return Message->soft_describe;
}

void
soft_app_message_set_describe (SoftAppMessage  *Message, 
                               const gchar     *soft_describe)
{
    g_free (Message->soft_describe);
    Message->soft_describe = g_strdup (soft_describe);
}  

const gchar *
soft_app_message_get_name (SoftAppMessage *Message)
{
    return Message->soft_name;
}

void
soft_app_message_set_name (SoftAppMessage  *Message, 
                           const gchar     *name)
{
    g_free (Message->soft_name);
    Message->soft_name = g_strdup (name);
}  

const gchar *
soft_app_message_get_icon (SoftAppMessage *Message)
{
    return Message->icon_name;
}

void
soft_app_message_set_icon (SoftAppMessage  *Message, 
                           const gchar     *icon)
{
    g_free (Message->icon_name);
    Message->icon_name = g_strdup (icon);
}  

static void
soft_app_message_finalize (GObject *object)
{
    SoftAppMessage *Message = SOFT_APP_MESSAGE (object);

    g_free (Message->soft_name);
    g_free (Message->icon_name);
    g_free (Message->soft_describe);
    g_free (Message->soft_size);
}

static void
soft_app_message_class_init (SoftAppMessageClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = soft_app_message_finalize;
}

static void
soft_app_message_init (SoftAppMessage *Message)
{

}

SoftAppMessage *
soft_app_message_new (void)
{
    SoftAppMessage *Message;
    Message = g_object_new (SOFT_APP_TYPE_MESSAGE, NULL);
    
    return SOFT_APP_MESSAGE (Message);
}
