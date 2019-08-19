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
G_DEFINE_TYPE (SoftAppMessage, soft_app_message, AS_TYPE_APP)

static void
soft_app_row_refresh (SoftAppRow *row)
{
	float      level;
	const char *icon_name;
	const char *soft_name;
	const char *sumary;
	icon_name = soft_app_message_get_icon(row->Message);
	gtk_image_set_from_icon_name(GTK_IMAGE(row->image),icon_name,GTK_ICON_SIZE_DIALOG);
	
	soft_name = as_app_get_name(AS_APP(row->Message),NULL);
	gtk_label_set_text(GTK_LABEL(row->label_name),soft_name);
	
	level = soft_app_message_get_score(row->Message);

    soft_app_star_widget_set_rating (row->stars1,level--);
    soft_app_star_widget_set_rating (row->stars2,level--);
    soft_app_star_widget_set_rating (row->stars3,level--);
    soft_app_star_widget_set_rating (row->stars4,level--);
    soft_app_star_widget_set_rating (row->stars5,level--);
   
	sumary = as_app_get_comment (AS_APP(row->Message),NULL);
	gtk_label_set_text(GTK_LABEL(row->label_describe),sumary);
	
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
	
	gtk_widget_set_has_window (GTK_WIDGET (row), FALSE);

    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);  
    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);  
	gtk_box_pack_start(GTK_BOX(box),hbox ,FALSE,FALSE, 10);
    gtk_container_add (GTK_CONTAINER (row), box);
    
    row->image = gtk_image_new();
	gtk_box_pack_start(GTK_BOX(hbox),row->image ,FALSE,FALSE, 10);
    
    vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);  
	gtk_widget_set_size_request(vbox,150,-1);
	gtk_box_pack_start(GTK_BOX(hbox),vbox ,FALSE,FALSE, 10);

    row->label_name = gtk_label_new(NULL);
	gtk_box_pack_start(GTK_BOX(vbox),row->label_name ,FALSE,FALSE, 6);
    
    v_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);  
    gtk_widget_set_halign(v_hbox, GTK_ALIGN_CENTER);
	gtk_box_pack_start(GTK_BOX(vbox),v_hbox ,FALSE,FALSE, 0);
	
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
    
    described_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);  
	gtk_widget_set_size_request(described_box,400,-1);
	gtk_box_pack_start(GTK_BOX(hbox),described_box ,FALSE,FALSE, 6);
	row->label_describe = gtk_label_new(NULL);
	gtk_label_set_max_width_chars(GTK_LABEL(row->label_describe),50);
	gtk_widget_set_size_request(row->label_describe,100,10);
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

int
soft_app_message_get_index (SoftAppMessage *Message)
{
    return Message->index;
}

void
soft_app_message_set_index (SoftAppMessage  *Message, 
                            int              index)
{
    Message->index = index;
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
soft_app_message_get_version (SoftAppMessage *Message)
{
    return Message->soft_version;
}

void
soft_app_message_set_version (SoftAppMessage  *Message, 
                              const gchar     *version)
{
    g_free (Message->soft_version);
    Message->soft_version = g_strdup (version);
} 

const gchar *
soft_app_message_get_license (SoftAppMessage *Message)
{
    return Message->soft_license;
}

void
soft_app_message_set_license (SoftAppMessage  *Message, 
                              const gchar     *licenses)
{
    g_free (Message->soft_license);
    Message->soft_license = g_strdup (licenses);
}  

const gchar *
soft_app_message_get_url (SoftAppMessage *Message)
{
    return Message->soft_url;
}

void
soft_app_message_set_url (SoftAppMessage  *Message, 
                          const gchar     *url)
{
    g_free (Message->soft_url);
    Message->soft_url = g_strdup (url);
} 

const gchar *
soft_app_message_get_arch (SoftAppMessage *Message)
{
    return Message->soft_arch;
}

void
soft_app_message_set_arch (SoftAppMessage  *Message, 
                           const gchar     *arch)
{
    g_free (Message->soft_arch);
    Message->soft_arch = g_strdup (arch);
}  
const gchar *
soft_app_message_get_package (SoftAppMessage *Message)
{
    return Message->soft_package;
}

void
soft_app_message_set_package (SoftAppMessage  *Message, 
                           const gchar     *package)
{
    g_free (Message->soft_package);
    Message->soft_package = g_strdup (package);
}  

const gchar *
soft_app_message_get_pkgid (SoftAppMessage *Message)
{
    return Message->soft_ids;
}

void
soft_app_message_set_pkgid (SoftAppMessage  *Message, 
                              const gchar     *ids)
{
    g_free (Message->soft_ids);
    Message->soft_ids = g_strdup(ids);
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

const gchar *
soft_app_message_get_cache (SoftAppMessage *Message)
{
    return Message->cache;
}

void
soft_app_message_set_cache (SoftAppMessage  *Message, 
                           const gchar      *cache)
{
    g_free (Message->cache);
    Message->cache = g_strdup (cache);
}  
gchar **
soft_app_message_get_files (SoftAppMessage *Message)
{
    return Message->soft_files;
}

void
soft_app_message_set_files (SoftAppMessage  *Message, 
                            gchar          **files)
{
    g_strfreev (Message->soft_files);
    Message->soft_files = g_strdupv (files);
}    
void soft_app_local_soft_detalis   (SoftAppMessage *Message,
		                            PkClient     *client,
			                        const char  **package_ids,
								   	GCancellable *cancellable,
                                    GAsyncReadyCallback callback_ready,
                                    gpointer      user_data)
	
{
	SoftAppMessageClass *klass;

    klass = SOFT_APP_MESSAGE_CLASS (Message);
    //g_assert (klass->get_local_soft_detalis != NULL);


    return klass->get_local_soft_detalis (Message, 
			                              client, 
										  package_ids, 
										  cancellable,
										  NULL,
										  NULL,
										  callback_ready,
										  user_data);
}


static void
soft_app_message_finalize (GObject *object)
{
    SoftAppMessage *Message = SOFT_APP_MESSAGE (object);

    g_free (Message->icon_name);
    g_free (Message->soft_size);
    g_free (Message->soft_url);
    g_free (Message->soft_version);
    g_free (Message->soft_arch);
    g_free (Message->soft_package);
    g_free (Message->soft_ids);
    g_free (Message->soft_license);
    g_free (Message->cache);
    g_strfreev (Message->soft_files);
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
