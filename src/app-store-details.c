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

//G_DEFINE_TYPE (SoftAppDetails,     soft_app_details,  G_TYPE_OBJECT)
G_DEFINE_TYPE (SoftAppInfo,        soft_app_info,     G_TYPE_OBJECT)


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

GtkWidget *CreateStoreIndividualDetails (SoftAppStore *app)
{
    GtkWidget *vbox;
    GtkWidget *sw;
    GtkWidget *label;

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
	sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
	label = gtk_label_new("sssss");
	gtk_container_add (GTK_CONTAINER (sw), label);

	return vbox;
}
