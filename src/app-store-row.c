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

static void
soft_app_row_refresh (SoftAppRow *row)
{
	float        level;
	const char  *icon_name;
	const char  *soft_name;
	const char  *summary;
    
    gboolean     mode;
    SoupSession *SoupSso;
    SoupMessage *SoupMsg;

    icon_name = soft_app_message_get_icon(row->Message);
    mode = soft_app_message_get_mode (row->Message);
	if(mode)
    {
	    gtk_image_set_from_icon_name(GTK_IMAGE(row->image),
                                     icon_name,
                                     GTK_ICON_SIZE_DIALOG);
    }
    else
    {   
	    SoupSso = soup_session_new ();
	    SoupMsg = soup_message_new (SOUP_METHOD_GET,icon_name);
        soup_session_queue_message (SoupSso,
	    					        SoupMsg,
								    SoupGetSoftIcon,
								    row->image);
    }   
	soft_name = soft_app_message_get_name(row->Message);
	gtk_label_set_text(GTK_LABEL(row->label_name),soft_name);
	
	level = soft_app_message_get_score(row->Message);

    soft_app_star_widget_set_rating (row->stars1,level--);
    soft_app_star_widget_set_rating (row->stars2,level--);
    soft_app_star_widget_set_rating (row->stars3,level--);
    soft_app_star_widget_set_rating (row->stars4,level--);
    soft_app_star_widget_set_rating (row->stars5,level--);
   
	summary = soft_app_message_get_summary (row->Message);
	gtk_label_set_text(GTK_LABEL(row->label_describe),summary);
	
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
    GtkWidget *described_box;
    GtkWidget *vbox;
    GtkWidget *v_hbox;
    GtkWidget *h_vbox;
	
    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);  
    gtk_container_add (GTK_CONTAINER (row), box);
    
    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);  
	gtk_box_pack_start(GTK_BOX(box),hbox ,FALSE,FALSE, 10);
    
    row->image = gtk_image_new();
	gtk_box_pack_start(GTK_BOX(hbox),row->image ,FALSE,FALSE, 10);
   
    vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);  
	gtk_widget_set_size_request(vbox,150,-1);
	gtk_box_pack_start(GTK_BOX(hbox),vbox ,FALSE,FALSE, 10);

    row->label_name = gtk_label_new(NULL);
    gtk_label_set_ellipsize (GTK_LABEL(row->label_name),PANGO_ELLIPSIZE_END);
    gtk_label_set_max_width_chars (GTK_LABEL(row->label_name),10);
    gtk_box_pack_start(GTK_BOX(vbox),row->label_name ,FALSE,FALSE, 6);
    
    v_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);  
    gtk_widget_set_halign(v_hbox, GTK_ALIGN_CENTER);
	gtk_box_pack_start(GTK_BOX(vbox),v_hbox ,FALSE,FALSE, 3);
	
	row->stars1 = gtk_image_new();
	gtk_box_pack_start(GTK_BOX(v_hbox),row->stars1 ,FALSE, FALSE, 0);
    row->stars2 = gtk_image_new();
	gtk_box_pack_start(GTK_BOX(v_hbox),row->stars2 ,FALSE, FALSE, 0);
    row->stars3 = gtk_image_new();
	gtk_box_pack_start(GTK_BOX(v_hbox),row->stars3 ,FALSE, FALSE, 0);
    row->stars4 = gtk_image_new();
	gtk_box_pack_start(GTK_BOX(v_hbox),row->stars4 ,FALSE, FALSE, 0);
    row->stars5 = gtk_image_new();
	gtk_box_pack_start(GTK_BOX(v_hbox),row->stars5 ,FALSE, FALSE, 0);
    
    described_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);  
	gtk_widget_set_size_request(described_box,400,-1);
	gtk_box_pack_start(GTK_BOX(hbox),described_box ,FALSE,FALSE, 6);
	row->label_describe = gtk_label_new(NULL);
	gtk_label_set_max_width_chars(GTK_LABEL(row->label_describe),50);
	gtk_label_set_line_wrap(GTK_LABEL(row->label_describe),TRUE);
	gtk_label_set_lines(GTK_LABEL(row->label_describe),2);
	gtk_box_pack_start(GTK_BOX(described_box),row->label_describe ,FALSE,FALSE, 6);

    h_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);  
	gtk_box_pack_start(GTK_BOX(hbox),h_vbox ,FALSE,FALSE, 6);

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
