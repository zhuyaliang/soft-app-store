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
#include "app-store-screenshot.h"
#include "app-store-thumbnail.h"
#include "app-store-local.h"
#include "app-store-store.h"
#include "app-store.h"
#define   UNKNOWPNG               "mp.png"

G_DEFINE_TYPE (SoftAppDetails,     soft_app_details,  GTK_TYPE_FIXED)
G_DEFINE_TYPE (SoftAppInfo,        soft_app_info,     G_TYPE_OBJECT)

static void
soft_app_details_refresh (SoftAppDetails *details)
{
	float      level;
    int        action;
	const char *icon_name,*screenshot_url;

    SoupSession *SoupSso;
    SoupMessage *SoupMsg;
	
    icon_name = soft_app_info_get_icon(details->info);
    action = soft_app_info_get_action(details->info);
	if(action == LOCALINSTALL)
    {
        gtk_image_set_from_icon_name(GTK_IMAGE(details->soft_image),icon_name,GTK_ICON_SIZE_DIALOG);
    }
    else
    {   

	    SoupSso = soup_session_new ();
	    SoupMsg = soup_message_new (SOUP_METHOD_GET,icon_name);
        soup_session_queue_message (SoupSso,
	    					        SoupMsg,
								    SoupGetSoftIcon,
								    details->soft_image);
    }    
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
	
    screenshot_url = soft_app_info_get_screenshot_url(details->info);
    soft_app_screenshot_load_async (details->screenshot,
                                    screenshot_url);
	/*
	screenshot_name = soft_app_info_get_screenshot(details->info);
	pixbuf = gdk_pixbuf_new_from_file(screenshot_name,NULL);
    soft_app_image_set_from_pixbuf(GTK_IMAGE(details->screenshot),pixbuf,400);
    g_object_unref(pixbuf);
    */

    gtk_label_set_text(GTK_LABEL(details->explain),
			           soft_app_info_get_explain (details->info));
    
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

static void
soft_app_details_init (SoftAppDetails *details)
{
    GtkWidget *main_vbox;
    GtkWidget *hbox1,*vbox1,*vbox2,*hbox2,*hbox3,*hbox4,*screenshot_box,*explain_box;
    GtkWidget *table;
    GtkWidget *tile;
    GtkWidget *version_tile;
    GtkWidget *source_tile;
    GtkWidget *size_tile;
    GtkWidget *protocol_tile;
    GtkWidget *package_tile;
    GtkWidget *arch_tile;
    GtkWidget *ss;
    SoupSession *session;

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

    hbox4 = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(hbox3),hbox4 ,TRUE,TRUE, 10);
    gtk_container_set_border_width (GTK_CONTAINER (hbox4), 5);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbox4), GTK_BUTTONBOX_EDGE);
    gtk_box_set_spacing (GTK_BOX (hbox4), 40);
    details->button = gtk_button_new();
    gtk_box_pack_start(GTK_BOX(hbox4),details->button ,FALSE,TRUE, 10);
    
    details->files_button = gtk_button_new_with_label(_("file list"));
    gtk_box_pack_start(GTK_BOX(hbox4),details->files_button ,FALSE,TRUE, 10);

	details->progressbar = gtk_progress_bar_new();
    gtk_box_pack_start(GTK_BOX(hbox3),details->progressbar ,FALSE,FALSE, 10);
	details->label_progress = gtk_label_new(NULL);
    gtk_box_pack_start(GTK_BOX(hbox3),details->label_progress ,FALSE,FALSE, 10);

    screenshot_box = gtk_box_new (GTK_ORIENTATION_VERTICAL,6);
    gtk_widget_set_size_request(screenshot_box,600,-1);
    gtk_box_pack_start(GTK_BOX(main_vbox),screenshot_box ,FALSE,FALSE, 10);
    
    session = soup_session_new ();
    ss = soft_app_screenshot_new (session);
    soft_app_screenshot_set_size (SOFT_APP_SCREENSHOT (ss),
                                  AS_IMAGE_LARGE_WIDTH,
                                  AS_IMAGE_LARGE_HEIGHT);

    gtk_container_add(GTK_CONTAINER(screenshot_box),ss);
	g_set_object (&details->screenshot,SOFT_APP_SCREENSHOT(ss));

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
    
    gtk_widget_show_all (GTK_WIDGET(details));
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

const char *soft_app_info_get_cache (SoftAppInfo *info)
{
	return info->cache; 
}
void soft_app_info_set_cache (SoftAppInfo *info,
		                     const char   *cache)
{
	g_free (info->cache);
    info->cache = g_strdup (cache);
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
const char *soft_app_info_get_screenshot_url (SoftAppInfo *info)
{
	return info->screenshot_url; 
}
void soft_app_info_set_screenshot_url (SoftAppInfo *info,
		                               const char  *screenshot_url)
{
	g_free (info->screenshot_url);
    info->screenshot_url = g_strdup (screenshot_url);
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

int  soft_app_info_get_action (SoftAppInfo *info)
{
	return info->action; 
}
void soft_app_info_set_action (SoftAppInfo *info,
		                       int          action)
{
    info->action = action;
}

int  soft_app_info_get_state (SoftAppInfo *info)
{
	return info->state; 
}
void soft_app_info_set_state (SoftAppInfo *info,
		                      gboolean     state)
{
    info->state = state;
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
    g_free (info->screenshot_url);
    g_free (info->explain);
    g_free (info->version);
    g_free (info->protocol);
    g_free (info->source);
    g_free (info->size);
    g_free (info->pkgid);
    g_free (info->cache);
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
    const char  *summary;
	const char  *size;
	const char  *arch;
	const char  *licenses;
	const char  *homepage;
    const char  *pkgname;
    const char  *version;
	const char  *describing;
	const char  *screenshot;
	float        score;
	gboolean     state;
	GtkWidget   *install_button;
	GtkWidget   *install_bar;
	
    sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start (GTK_BOX (app->StackDetailsBox), sw, TRUE, TRUE, 0);
	
	name = soft_app_thumbnail_get_name (tile->thb);
	icon = soft_app_thumbnail_get_icon (tile->thb);
    summary = soft_app_thumbnail_get_sumary (tile->thb);
    pkgname = soft_app_thumbnail_get_pkgname (tile->thb);
    version = soft_app_thumbnail_get_version (tile->thb);
	score = soft_app_thumbnail_get_score (tile->thb);
	size  = soft_app_thumbnail_get_size (tile->thb);
	arch  = soft_app_thumbnail_get_arch (tile->thb);
	homepage  = soft_app_thumbnail_get_homepage (tile->thb);
	licenses  = soft_app_thumbnail_get_licenses (tile->thb);
    describing = soft_app_thumbnail_get_description (tile->thb); 
    screenshot = soft_app_thumbnail_get_screenurl (tile->thb);

	info = soft_app_info_new(name);
	soft_app_info_set_icon(info,icon);
	soft_app_info_set_comment(info,summary);
	state = soft_app_thumbnail_get_state (tile->thb);
	if (!state)
		soft_app_info_set_button(info,_("install"));
	else
		soft_app_info_set_button(info,_("installed"));
	soft_app_info_set_score(info,score);
	soft_app_info_set_screenshot_url(info,screenshot);
    soft_app_info_set_explain(info,describing);
	soft_app_info_set_version(info,version);
	soft_app_info_set_arch(info,arch);
	soft_app_info_set_protocol(info,licenses);
	soft_app_info_set_source(info,homepage);
	soft_app_info_set_size (info,size);
	soft_app_info_set_package (info,pkgname);
	soft_app_info_set_action (info,STOREAPPSOFT);
	soft_app_info_set_state (info,state);
	
	details = soft_app_details_new(info);
    gtk_widget_set_halign (GTK_WIDGET (details), GTK_ALIGN_CENTER);
    gtk_widget_set_valign (GTK_WIDGET (details), GTK_ALIGN_CENTER);
	app->details = SOFT_APP_DETAILS(details);

	install_button = soft_app_details_get_button(SOFT_APP_DETAILS(details));
	g_signal_connect (install_button, 
                     "clicked",
                      G_CALLBACK (InstallStoreSoftApp),
                      app);
    gtk_container_add (GTK_CONTAINER (sw), GTK_WIDGET(details));

    gtk_widget_show_all(app->StackDetailsBox);
	install_bar = soft_app_details_get_bar(SOFT_APP_DETAILS(details));
	gtk_widget_hide(install_bar);
}    
static void CreateLocalSoftDetails(SoftAppStore *app,SoftAppRow *row)
{
    GtkWidget   *sw;
	SoftAppInfo *info;
	GtkFixed    *details;
	const char  *name;
	const char  *icon;
	const char  *explain;
	const char  *version;
	const char  *license;
	const char  *url;
	const char  *size;
	const char  *pkgid;
	const char  *summary;
	const char  *arch;
	const char  *package;
	const char  *cache_file;
	float        score;
	GtkWidget   *remove_button;
	GtkWidget   *files_button;
	GtkWidget   *install_bar;
    GPtrArray   *screenshots;
	AsScreenshot *as_shot;
    AsImage     *im = NULL;
    const char  *screenshot_url = NULL;

    soft_app_container_remove_all (GTK_CONTAINER (app->StackDetailsBox));
    sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start (GTK_BOX (app->StackDetailsBox), sw, TRUE, TRUE, 0);

	name = as_app_get_name(AS_APP(row->Message),NULL); 
	cache_file = soft_app_message_get_cache(row->Message);
    icon = soft_app_message_get_icon(row->Message);
	score = soft_app_message_get_score(row->Message);
    screenshots = as_app_get_screenshots(AS_APP(row->Message));
    if (screenshots->len >= 1)
    {    
        as_shot = g_ptr_array_index (screenshots, 0);
        im = as_screenshot_get_image (as_shot,AS_IMAGE_LARGE_WIDTH,AS_IMAGE_LARGE_HEIGHT);
        screenshot_url = as_image_get_url (im);
    }
    explain = as_app_get_description(AS_APP(row->Message),NULL);
    version = soft_app_message_get_version(row->Message);
    license = soft_app_message_get_license(row->Message);
    url     = soft_app_message_get_url(row->Message);
    size    = soft_app_message_get_size(row->Message);
    pkgid   = soft_app_message_get_pkgid(row->Message);
    summary = as_app_get_comment(AS_APP(row->Message),NULL);
    arch    = soft_app_message_get_arch(row->Message);
    package = soft_app_message_get_package(row->Message);

	info = soft_app_info_new(name);
	soft_app_info_set_icon(info,icon);
	soft_app_info_set_comment(info,summary);
	soft_app_info_set_button(info,_("uninstall"));
	soft_app_info_set_score(info,score);
	soft_app_info_set_screenshot_url(info,screenshot_url);
	soft_app_info_set_explain(info,explain);
	soft_app_info_set_version(info,version);
	soft_app_info_set_protocol(info,license);
	soft_app_info_set_source(info,url);
	soft_app_info_set_size(info,size);
	soft_app_info_set_pkgid(info,pkgid);
	soft_app_info_set_cache(info,cache_file);
	soft_app_info_set_arch(info,arch);
	soft_app_info_set_package(info,package);
	soft_app_info_set_action(info,LOCALINSTALL);

	details = soft_app_details_new(info);
	app->details = SOFT_APP_DETAILS(details);
	remove_button = soft_app_details_get_button(SOFT_APP_DETAILS(details));
    g_signal_connect (remove_button,
                     "clicked",
                      G_CALLBACK (RemoveLocalSoftApp),
					  app);
	files_button = SOFT_APP_DETAILS(details)->files_button;
    g_signal_connect (files_button,
                     "clicked",
                      G_CALLBACK (ViewLocalSoftFiles),
					  app);
    gtk_widget_set_halign (GTK_WIDGET (details), GTK_ALIGN_CENTER);
    gtk_widget_set_valign (GTK_WIDGET (details), GTK_ALIGN_CENTER);
    gtk_container_add (GTK_CONTAINER (sw), GTK_WIDGET(details));

    gtk_widget_show_all(app->StackDetailsBox);
	install_bar = soft_app_details_get_bar(SOFT_APP_DETAILS(details));
	gtk_widget_hide(install_bar);
}
void SwitchPageToIndividualDetailsPage (GtkListBox    *list_box,
                                        GtkListBoxRow *Row,
                                        gpointer       data)

{
	SoftAppStore *app = (SoftAppStore *)data;
	SoftAppRow *row = SOFT_APP_ROW(Row);

    app->index = soft_app_message_get_index (row->Message);
    app->page = INDIVIDUAL_SOFT_PAGE;
	SwitchPage(app);
    CreateLocalSoftDetails(app,row);
}
