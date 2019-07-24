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

#include "app-store-util.h"
#include "app-store-details.h"
#include "app-store-thumbnail.h"

G_DEFINE_TYPE (SoftAppDetails,     soft_app_details,  GTK_TYPE_FIXED)
G_DEFINE_TYPE (SoftAppInfo,        soft_app_info,     G_TYPE_OBJECT)

static void
soft_app_details_refresh (SoftAppDetails *details)
{
	GdkPixbuf *pixbuf;
	float      level;
	const char *icon_name,*screenshot_name;

	icon_name = soft_app_info_get_icon(details->info);
	pixbuf = gdk_pixbuf_new_from_file(icon_name,NULL);
	soft_app_image_set_from_pixbuf(GTK_IMAGE(details->soft_image),pixbuf,50);
    g_object_unref(pixbuf);
	SetLableFontType(details->label_name,
                    "black",
                     10,
                     soft_app_info_get_name (details->info),
                     TRUE);
	
	SetLableFontType(details->label_comment,
                    "black",
                     10,
                     soft_app_info_get_comment (details->info),
                     TRUE);
	level = soft_app_info_get_score(details->info);

    soft_app_star_widget_set_rating (details->stars1,level--);
    soft_app_star_widget_set_rating (details->stars2,level--);
    soft_app_star_widget_set_rating (details->stars3,level--);
    soft_app_star_widget_set_rating (details->stars4,level--);
    soft_app_star_widget_set_rating (details->stars5,level--);

    gtk_button_set_label(GTK_BUTTON(details->button),
                         soft_app_info_get_button(details->info));	
	
	screenshot_name = soft_app_info_get_screenshot(details->info);
	pixbuf = gdk_pixbuf_new_from_file(screenshot_name,NULL);
	soft_app_image_set_from_pixbuf(GTK_IMAGE(details->screenshot),pixbuf,200);
    g_object_unref(pixbuf);
    SetLableFontType(details->explain,
                    "black",
                     10,
                     soft_app_info_get_explain (details->info),
                     TRUE);
}    
void
soft_app_details_set_info (SoftAppDetails *details, SoftAppInfo *info)
{
    g_set_object (&details->info, info);
    soft_app_details_refresh (details);
}

static void
soft_app_details_destroy (GtkWidget *widget)
{
    SoftAppDetails *details = SOFT_APP_DETAILS (widget);
    g_clear_object (&details->info);
}

static void
soft_app_details_init (SoftAppDetails *details)
{
    GtkWidget *main_vbox;
    GtkWidget *hbox1,*vbox1,*hbox2,*hbox3;
    
    main_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_fixed_put(GTK_FIXED(details),main_vbox, 0, 0);
    
    hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(main_vbox),hbox1 ,FALSE,FALSE, 10);

    details->soft_image = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(hbox1),details->soft_image ,FALSE,FALSE, 10);
    
    vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(hbox1),vbox1 ,FALSE,FALSE, 10);
    details->label_name = gtk_label_new(NULL);
    gtk_box_pack_start(GTK_BOX(hbox1),details->label_name ,FALSE,FALSE, 6);
    details->label_comment = gtk_label_new(NULL);
    gtk_box_pack_start(GTK_BOX(hbox1),details->label_comment ,FALSE,FALSE, 6);
    
    hbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(hbox1),hbox2 ,FALSE,FALSE, 10);
    gtk_widget_set_halign (hbox2, GTK_ALIGN_END); 
    details->stars1 = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(hbox2),details->stars1 ,FALSE,FALSE, 0);
    details->stars2 = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(hbox2),details->stars2 ,FALSE,FALSE, 0);
    details->stars3 = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(hbox2),details->stars3 ,FALSE,FALSE, 0);
    details->stars4 = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(hbox2),details->stars4 ,FALSE,FALSE, 0);
    details->stars5 = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(hbox2),details->stars5 ,FALSE,FALSE, 0);
    
    hbox3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(main_vbox),hbox3 ,FALSE,FALSE, 10);
    details->button = gtk_button_new();
    gtk_box_pack_start(GTK_BOX(hbox3),details->button ,FALSE,FALSE, 10);

    details->screenshot = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(main_vbox),details->screenshot ,FALSE,FALSE, 10);
	
    details->explain = gtk_label_new(NULL);
	gtk_label_set_max_width_chars(GTK_LABEL(details->explain),70);
	gtk_widget_set_size_request(details->explain,100,10);
	gtk_label_set_line_wrap(GTK_LABEL(details->explain),TRUE);
	gtk_label_set_lines(GTK_LABEL(details->explain),3);
	gtk_box_pack_start(GTK_BOX(main_vbox),details->explain ,FALSE,FALSE, 6);

}
static void
soft_app_details_class_init (SoftAppDetailsClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    widget_class->destroy = soft_app_details_destroy;
}

GtkFixed *
soft_app_details_new (SoftAppInfo *info)
{
    SoftAppDetails *details;

    details = g_object_new (SOFT_APP_TYPE_DETAILS, NULL);
    soft_app_details_set_info (details, info);

    return GTK_FIXED (details);
}

const char	*soft_app_info_get_name	(SoftAppInfo *info)
{
	return info->soft_name; 
}

void soft_app_info_set_name (SoftAppInfo *info,
		                     const char  *name)
{
	g_free (info->soft_name);
    info->soft_name = g_strdup (name);

}
const char *soft_app_info_get_icon (SoftAppInfo *info)
{

	return info->icon_name; 
}
void soft_app_info_set_icon (SoftAppInfo *info,
		                     const char  *icon)
{
	g_free (info->icon_name);
    info->icon_name = g_strdup (icon);

}
const char *soft_app_info_get_comment (SoftAppInfo *info)

{
	return info->comment; 

}
void soft_app_info_set_comment (SoftAppInfo *info,
		                        const char  *comment)
{
	g_free (info->comment);
    info->comment = g_strdup (comment);

}
float soft_app_info_get_score (SoftAppInfo *info)
{
	return info->score; 

}
void soft_app_info_set_score (SoftAppInfo *info,
		                      float        score)
{
    info->score = score;
}
const char *soft_app_info_get_button (SoftAppInfo *info)
{
	return info->button_name; 

}
void soft_app_info_set_button (SoftAppInfo *info,
		                       const char  *button_name)
{
	g_free (info->button_name);
    info->button_name = g_strdup (button_name);
}
const char *soft_app_info_get_screenshot (SoftAppInfo *info)
{
	return info->screenshot; 
}
void soft_app_info_set_screenshot (SoftAppInfo *info,
		                           const char  *screenshot_name)
{
	g_free (info->screenshot);
    info->screenshot = g_strdup (screenshot_name);
}
const char *soft_app_info_get_explain (SoftAppInfo *info)
{
	return info->explain; 
}
void soft_app_info_set_explain (SoftAppInfo *info,
		                        const char  *explain)
{
	g_free (info->explain);
    info->explain = g_strdup (explain);
}
const char *soft_app_info_get_version (SoftAppInfo *info)
{
	return info->version; 
}
void soft_app_info_set_version (SoftAppInfo *info,
		                        const char  *version)
{
	g_free (info->version);
    info->version = g_strdup (version);
}
const char *soft_app_info_get_protocol (SoftAppInfo *info)
{
	return info->protocol; 
}

void soft_app_info_set_protocol (SoftAppInfo *info,
		                         const char  *protocol)
{
	g_free (info->protocol);
    info->protocol = g_strdup (protocol);

}

const char *soft_app_info_get_source (SoftAppInfo *info)
{
	return info->source; 
}
void soft_app_info_set_source (SoftAppInfo *info,
		                       const char  *source)
{
	g_free (info->source);
    info->source = g_strdup (source);
}

const char *soft_app_info_get_size (SoftAppInfo *info)
{
	return info->size; 
}
void soft_app_info_set_size (SoftAppInfo *info,
		                     const char  *size)
{
	g_free (info->size);
    info->size = g_strdup (size);
}
static void
soft_app_info_finalize (GObject *object)
{
    SoftAppInfo *info = SOFT_APP_INFO (object);

    g_free (info->soft_name);
    g_free (info->icon_name);
    g_free (info->comment);
    g_free (info->button_name);
    g_free (info->screenshot);
    g_free (info->explain);
    g_free (info->version);
    g_free (info->protocol);
    g_free (info->source);
    g_free (info->size);
}

static void
soft_app_info_class_init (SoftAppInfoClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = soft_app_info_finalize;
}

static void
soft_app_info_init (SoftAppInfo *info)
{

}

SoftAppInfo *
soft_app_info_new (const char *name)
{
    SoftAppInfo *info;
    info = g_object_new (SOFT_APP_TYPE_INFO, NULL);
	info->soft_name = g_strdup(name);

    return SOFT_APP_INFO (info);
}
void CreateRecommendDetails(SoftAppStore *app,gpointer data)
{
    SoftAppThumbnailTile *thumbnail = SOFT_APP_THUMBNAIL_TILE(data);

    GtkWidget *sw;
    GtkWidget *label;
	
    sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start (GTK_BOX (app->StackDetailsBox), sw, TRUE, TRUE, 0);
	label = gtk_label_new("pppppppppppppppppppp");
	gtk_container_add (GTK_CONTAINER (sw), label);
    gtk_widget_show_all(app->StackDetailsBox);
}    
