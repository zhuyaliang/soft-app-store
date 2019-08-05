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
#include "app-store.h"

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
	soft_app_image_set_from_pixbuf(GTK_IMAGE(details->soft_image),pixbuf,65);
    g_object_unref(pixbuf);
	SetLableFontType(details->label_name,
                    "black",
                     12,
                     soft_app_info_get_name (details->info),
                     TRUE);
	
	SetLableFontType(details->label_comment,
                    "black",
                     11,
                     soft_app_info_get_comment (details->info),
                     FALSE);
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

    soft_app_image_set_from_pixbuf(GTK_IMAGE(details->screenshot),pixbuf,500);
    g_object_unref(pixbuf);
    SetLableFontType(details->explain,
                    "black",
                     11,
                     soft_app_info_get_explain (details->info),
                     FALSE);
    
    SetLableFontType(details->label_version,
                    "black",
                     10,
                     soft_app_info_get_version (details->info),
                     FALSE);
    SetLableFontType(details->label_protocol,
                    "black",
                     10,
                     soft_app_info_get_protocol (details->info),
                     FALSE);
    SetLableFontType(details->label_source,
                    "black",
                     10,
                     soft_app_info_get_source (details->info),
                     FALSE);
    SetLableFontType(details->label_size,
                    "black",
                     10,
                     soft_app_info_get_size (details->info),
                     FALSE);
    SetLableFontType(details->label_arch,
                    "black",
                     10,
                     soft_app_info_get_arch (details->info),
                     FALSE);
    SetLableFontType(details->label_package,
                    "black",
                     10,
                     soft_app_info_get_package (details->info),
                     FALSE);
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

static guint soft_app_get_progress(SoftAppStore *app)
{
	usleep(10000);
	return app->per++;
}
static void
soft_app_details_page_refresh_progress (SoftAppStore *app)
{
	guint percentage;

	SoftAppDetails *details = app->details;
	percentage = soft_app_get_progress (app);
    if (percentage <= 100) 
	{
		g_autofree gchar *str = g_strdup_printf ("%u%%", percentage);
        gtk_label_set_label (GTK_LABEL (details->label_progress), str);
        gtk_widget_set_visible (details->label_progress, TRUE);
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (details->progressbar),
                           (gdouble) percentage / 100.f);
        gtk_widget_set_visible (details->progressbar, TRUE);
    }

}
static gboolean
soft_app_details_page_refresh_progress_idle (gpointer user_data)
{
    SoftAppStore *app = (SoftAppStore *)user_data;
    soft_app_details_page_refresh_progress (app);
    
	return TRUE;
}

static void
soft_app_details_page_progress_changed_cb (SoftAppStore *app)
{
    g_idle_add (soft_app_details_page_refresh_progress_idle, app);
}

static void
soft_app_details_page_app_install_button_cb (GtkWidget *widget, SoftAppStore *app)
{
	SoftAppDetails *details = app->details;
	
	app->per = 0;
	gtk_widget_hide(widget);
	soft_app_details_page_progress_changed_cb(app);
	gtk_widget_show(details->progressbar);
}

static void
soft_app_details_init (SoftAppDetails *details)
{
    GtkWidget *main_vbox;
    GtkWidget *hbox1,*vbox1,*vbox2,*hbox2,*hbox3,*screenshot_box,*explain_box;
    GtkWidget *table;
    GtkWidget *tile;
    GtkWidget *version_tile;
    GtkWidget *source_tile;
    GtkWidget *size_tile;
    GtkWidget *protocol_tile;
    GtkWidget *package_tile;
    GtkWidget *arch_tile;

    main_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_fixed_put(GTK_FIXED(details),main_vbox, 0, 0);
    
    hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 18);
    gtk_box_pack_start(GTK_BOX(main_vbox),hbox1 ,FALSE,FALSE, 10);

    details->soft_image = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(hbox1),details->soft_image ,FALSE,FALSE, 0);
    
    vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(hbox1),vbox1 ,TRUE,TRUE, 0);
    gtk_widget_set_halign(vbox1,GTK_ALIGN_START);
    gtk_widget_set_valign(vbox1,GTK_ALIGN_START);
    
    details->label_name = gtk_label_new(NULL);
    gtk_widget_set_halign(details->label_name,GTK_ALIGN_START);
    gtk_widget_set_hexpand (details->label_name,TRUE);
    gtk_box_pack_start(GTK_BOX(vbox1),details->label_name ,FALSE,FALSE, 0);
    details->label_comment = gtk_label_new(NULL);
    gtk_box_pack_start(GTK_BOX(vbox1),details->label_comment ,FALSE,FALSE, 0);
    
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
    
    hbox3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(main_vbox),hbox3 ,FALSE,FALSE, 10);
    details->button = gtk_button_new();
    gtk_widget_set_halign(details->button,GTK_ALIGN_START);
    gtk_widget_set_valign(details->button,GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(hbox3),details->button ,FALSE,FALSE, 10);
	details->progressbar = gtk_progress_bar_new();
    gtk_box_pack_start(GTK_BOX(hbox3),details->progressbar ,FALSE,FALSE, 10);
	details->label_progress = gtk_label_new(NULL);
    gtk_box_pack_start(GTK_BOX(hbox3),details->label_progress ,FALSE,FALSE, 10);

    screenshot_box = gtk_box_new (GTK_ORIENTATION_VERTICAL,6);
    gtk_widget_set_size_request(screenshot_box,600,-1);
    gtk_box_pack_start(GTK_BOX(main_vbox),screenshot_box ,FALSE,FALSE, 10);
    details->screenshot = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(screenshot_box),details->screenshot ,FALSE,FALSE, 10);

    explain_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_pack_start(GTK_BOX(main_vbox),explain_box ,FALSE,FALSE, 6);
    details->explain = gtk_label_new(NULL);
	gtk_label_set_max_width_chars(GTK_LABEL(details->explain),50);
	gtk_widget_set_size_request(details->explain,50,5);
	gtk_label_set_line_wrap(GTK_LABEL(details->explain),TRUE);
	gtk_label_set_lines(GTK_LABEL(details->explain),5);
	gtk_box_pack_start(GTK_BOX(explain_box),details->explain ,FALSE,FALSE, 6);
    
    tile = gtk_label_new(NULL);
	SetLableFontType(tile,
                    "black",
                     15,
                    _("details"),
                     TRUE);
    gtk_widget_set_halign(tile,GTK_ALIGN_START);
    gtk_widget_set_valign(tile,GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(main_vbox),tile ,FALSE,FALSE, 6);
    
    table = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(table),TRUE);
    gtk_widget_set_halign(table,GTK_ALIGN_START);
    gtk_widget_set_valign(table,GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(main_vbox),table ,FALSE,FALSE, 6);
    
    version_tile = gtk_label_new(NULL);
	SetLableFontType(version_tile,
                    "black",
                     12,
                    _("version"),
                     TRUE);
    gtk_grid_attach(GTK_GRID(table) , version_tile , 0 , 0 , 1 , 1);
    details->label_version = gtk_label_new(NULL);
    gtk_grid_attach(GTK_GRID(table) , details->label_version , 1 , 0 , 1 , 1);

    protocol_tile = gtk_label_new(NULL);
	SetLableFontType(protocol_tile,
                    "black",
                     12,
                    _("protocol"),
                     TRUE);
    gtk_grid_attach(GTK_GRID(table) , protocol_tile , 0 , 1 , 1 , 1);
    details->label_protocol = gtk_label_new(NULL);
    gtk_grid_attach(GTK_GRID(table) , details->label_protocol , 1 , 1 , 1 , 1);
    
    source_tile = gtk_label_new(NULL);
	SetLableFontType(source_tile,
                    "black",
                     12,
                    _("sources"),
                     TRUE);
    gtk_grid_attach(GTK_GRID(table) , source_tile , 0 , 2 , 1 , 1);
    details->label_source = gtk_label_new(NULL);
    gtk_grid_attach(GTK_GRID(table) , details->label_source , 1 , 2 , 1 , 1);
    
    size_tile = gtk_label_new(NULL);
	SetLableFontType(size_tile,
                    "black",
                     12,
                    _("size"),
                     TRUE);
    gtk_grid_attach(GTK_GRID(table) , size_tile , 0 , 3 , 1 , 1);
    details->label_size = gtk_label_new(NULL);
    gtk_grid_attach(GTK_GRID(table) , details->label_size , 1 , 3 , 1 , 1);

    arch_tile = gtk_label_new(NULL);
	SetLableFontType(arch_tile,
                    "black",
                     12,
                    _("arch"),
                     TRUE);
    gtk_grid_attach(GTK_GRID(table) , arch_tile , 0 , 4 , 1 , 1);
    details->label_arch = gtk_label_new(NULL);
    gtk_grid_attach(GTK_GRID(table) , details->label_arch , 1 , 4 , 1 , 1);
    
    package_tile = gtk_label_new(NULL);
	SetLableFontType(package_tile,
                    "black",
                     12,
                    _("package"),
                     TRUE);
    gtk_grid_attach(GTK_GRID(table) , package_tile , 0 , 5 , 1 , 1);
    details->label_package = gtk_label_new(NULL);
    gtk_grid_attach(GTK_GRID(table) , details->label_package , 1 , 5 , 1 , 1);
    
    gtk_grid_set_row_spacing(GTK_GRID(table), 12);
    gtk_grid_set_column_spacing(GTK_GRID(table), 12);
    
    vbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(main_vbox),vbox2 ,FALSE,FALSE, 10);

    details->button_discuss = gtk_button_new_with_label(_("discuss"));
    gtk_box_pack_start(GTK_BOX(vbox2),details->button_discuss ,FALSE,FALSE, 10);

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

const char *soft_app_info_get_pkgid (SoftAppInfo *info)
{
	return info->pkgid; 
}
void soft_app_info_set_pkgid (SoftAppInfo *info,
		                      const char  *pkgid)
{
	g_free (info->pkgid);
    info->pkgid = g_strdup (pkgid);
}

const char *soft_app_info_get_arch (SoftAppInfo *info)
{
	return info->arch; 
}
void soft_app_info_set_arch (SoftAppInfo *info,
		                      const char  *arch)
{
	g_free (info->arch);
    info->arch = g_strdup (arch);
}

const char *soft_app_info_get_package (SoftAppInfo *info)
{
	return info->package; 
}
void soft_app_info_set_package (SoftAppInfo *info,
		                        const char  *package)
{
	g_free (info->package);
    info->package = g_strdup (package);
}

GtkWidget *soft_app_details_get_button(SoftAppDetails *details)
{
	return details->button;
}
GtkWidget *soft_app_details_get_bar(SoftAppDetails *details)
{
	return details->progressbar;
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
    g_free (info->pkgid);
    g_free (info->arch);
    g_free (info->package);
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
void CreateRecommendDetails(gpointer d,gpointer data)
{
    SoftAppThumbnailTile *tile = SOFT_APP_THUMBNAIL_TILE(data);
	SoftAppStore *app = (SoftAppStore *)d;

    GtkWidget   *sw;
	SoftAppInfo *info;
	GtkFixed    *details;
	const char  *name;
	const char  *icon;
	float        score;
	GtkWidget   *install_button;
	GtkWidget   *install_bar;
	
    sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start (GTK_BOX (app->StackDetailsBox), sw, TRUE, TRUE, 0);
	
	name = soft_app_thumbnail_get_name(tile->thb);
	icon = soft_app_thumbnail_get_icon(tile->thb);
	score = soft_app_thumbnail_get_score(tile->thb);
	info = soft_app_info_new(name);
	soft_app_info_set_icon(info,icon);
	soft_app_info_set_comment(info,"manage local time and time zone");
	soft_app_info_set_button(info,"install");
	soft_app_info_set_score(info,score);
	soft_app_info_set_screenshot(info,"/tmp/time.png");
	soft_app_info_set_explain(info,"This function simply calls local time zone date ntp sync net time hellow world world hello  get depends on some condition.");
	soft_app_info_set_version(info,"v1.1.1");
	soft_app_info_set_protocol(info,"GPL-3.0");
	soft_app_info_set_source(info,"github.com/zhuyaliang");
	soft_app_info_set_size(info,"12M");
	
	details = soft_app_details_new(info);
    gtk_widget_set_halign (GTK_WIDGET (details), GTK_ALIGN_CENTER);
    gtk_widget_set_valign (GTK_WIDGET (details), GTK_ALIGN_CENTER);
	app->details = SOFT_APP_DETAILS(details);	
	install_button = soft_app_details_get_button(SOFT_APP_DETAILS(details));
	g_signal_connect (install_button, "clicked",
                      G_CALLBACK (soft_app_details_page_app_install_button_cb),
                      app);
    gtk_container_add (GTK_CONTAINER (sw), GTK_WIDGET(details));

    gtk_widget_show_all(app->StackDetailsBox);
	install_bar = soft_app_details_get_bar(SOFT_APP_DETAILS(details));
	gtk_widget_hide(install_bar);
}    
