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

#include "app-store-thumbnail.h"
#include "app-store-util.h"

G_DEFINE_TYPE (SoftAppThumbnailTile, soft_app_thumbnail_tile, GTK_TYPE_BUTTON)
G_DEFINE_TYPE (SoftAppThumbnail,     soft_app_thumbnail,      G_TYPE_OBJECT)

/*
static gboolean
app_state_changed_idle (gpointer user_data)
{
    GsPopularTile *tile = GS_POPULAR_TILE (user_data);
    AtkObject *accessible;
    gboolean installed;
    g_autofree gchar *name = NULL;

    tile->app_state_changed_id = 0;

    accessible = gtk_widget_get_accessible (GTK_WIDGET (tile));

    switch (gs_app_get_state (tile->app)) {
    case AS_APP_STATE_INSTALLED:
    case AS_APP_STATE_REMOVING:
    case AS_APP_STATE_UPDATABLE:
    case AS_APP_STATE_UPDATABLE_LIVE:
        installed = TRUE;
        name = g_strdup_printf (_("%s (Installed)"),
                    gs_app_get_name (tile->app));
        break;
    case AS_APP_STATE_AVAILABLE:
    case AS_APP_STATE_INSTALLING:
    default:
        installed = FALSE;
        name = g_strdup (gs_app_get_name (tile->app));
        break;
    }

    gtk_widget_set_visible (tile->eventbox, installed);

    if (GTK_IS_ACCESSIBLE (accessible)) {
        atk_object_set_name (accessible, name);
        atk_object_set_description (accessible, gs_app_get_summary (tile->app));
    }

    return G_SOURCE_REMOVE;
}

static void
app_state_changed (GsApp *app, GParamSpec *pspec, GsPopularTile *tile)
{
    g_clear_handle_id (&tile->app_state_changed_id, g_source_remove);
    tile->app_state_changed_id = g_idle_add (app_state_changed_idle, tile);
}
*/
void soft_app_star_widget_set_rating (GtkWidget *stars,float level)
{
    const float EPSINON = 0.00001;

    if(level >= 1)
    {    
        gtk_image_set_from_icon_name (GTK_IMAGE (stars),
                                     "starred",
                                      GTK_ICON_SIZE_MENU);
    }
    else if(level < 1 && level >= EPSINON)
    {
        gtk_image_set_from_icon_name (GTK_IMAGE (stars),
                                     "semi-starred",
                                      GTK_ICON_SIZE_MENU);
    }
    else
    {
        gtk_image_set_from_icon_name (GTK_IMAGE (stars),
                                     "non-starred",
                                      GTK_ICON_SIZE_MENU);
    }    
}    
static void
soft_app_thumbnail_tile_set_app (SoftAppThumbnailTile *tile)
{
    GdkPixbuf *pixbuf;
    float      level;
	const char *icon_name;
	const char *soft_name;

	level = soft_app_thumbnail_get_score(tile->thb);
    soft_app_star_widget_set_rating (tile->stars1,level--);
    soft_app_star_widget_set_rating (tile->stars2,level--);
    soft_app_star_widget_set_rating (tile->stars3,level--);
    soft_app_star_widget_set_rating (tile->stars4,level--);
    soft_app_star_widget_set_rating (tile->stars5,level--);

	icon_name = soft_app_thumbnail_get_icon(tile->thb);
    pixbuf = gdk_pixbuf_new_from_file(icon_name,NULL);
    soft_app_image_set_from_pixbuf(GTK_IMAGE(tile->image),pixbuf,64);
	
    soft_name = soft_app_thumbnail_get_name(tile->thb);
    SetLableFontType(tile->label,
                    "black",
                     11,
                     soft_name,
                     FALSE);
}

static void soft_app_thumbnail_tile_set_data(SoftAppThumbnailTile *tile, SoftAppThumbnail *app)
{
	g_set_object (&tile->thb, app);
    soft_app_thumbnail_tile_set_app (tile);
}    
static void
soft_app_thumbnail_tile_destroy (GtkWidget *widget)
{
    SoftAppThumbnailTile *tile = SOFT_APP_THUMBNAIL_TILE(widget);
    g_clear_object (&tile->thb);
}

static void
soft_app_thumbnail_tile_init (SoftAppThumbnailTile *tile)
{
    GtkWidget *vbox,*hbox;
    
    gtk_widget_set_size_request(GTK_WIDGET (tile), 110,100);
    
    tile->event_box = gtk_event_box_new();
    gtk_container_add (GTK_CONTAINER (tile), tile->event_box);
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    gtk_container_add (GTK_CONTAINER (tile->event_box), vbox);
    
    tile->image = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(vbox),tile->image ,TRUE, TRUE, 0);
    
    tile->label = gtk_label_new(NULL);
	gtk_box_pack_start(GTK_BOX(vbox),tile->label ,TRUE, TRUE, 0);
    
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	gtk_box_pack_start(GTK_BOX(vbox),hbox ,TRUE, TRUE, 0);

    tile->stars1 = gtk_image_new();
	gtk_box_pack_start(GTK_BOX(hbox),tile->stars1 ,TRUE, TRUE, 0);
    tile->stars2 = gtk_image_new();
	gtk_box_pack_start(GTK_BOX(hbox),tile->stars2 ,TRUE, TRUE, 0);
    tile->stars3 = gtk_image_new();
	gtk_box_pack_start(GTK_BOX(hbox),tile->stars3 ,TRUE, TRUE, 0);
    tile->stars4 = gtk_image_new();
	gtk_box_pack_start(GTK_BOX(hbox),tile->stars4 ,TRUE, TRUE, 0);
    tile->stars5 = gtk_image_new();
	gtk_box_pack_start(GTK_BOX(hbox),tile->stars5 ,TRUE, TRUE, 0);
}

static void
soft_app_thumbnail_tile_class_init (SoftAppThumbnailTileClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
    
    widget_class->destroy = soft_app_thumbnail_tile_destroy;

}

GtkWidget *
soft_app_thumbnail_tile_new (SoftAppThumbnail *app)
{
    SoftAppThumbnailTile *tile;

    tile = g_object_new (SOFT_APP_TYPE_THUMBNAIL_TILE, NULL);
    if (app != NULL)
        soft_app_thumbnail_tile_set_data (tile, app);

    return GTK_WIDGET (tile);
}

float
soft_app_thumbnail_get_score (SoftAppThumbnail *thb)
{
    return thb->soft_score;
}

void
soft_app_thumbnail_set_score (SoftAppThumbnail  *thb, 
                              float              soft_score)
{
    thb->soft_score = soft_score;
}  

const gchar *
soft_app_thumbnail_get_name (SoftAppThumbnail *thb)
{
    return thb->soft_name;
}

void
soft_app_thumbnail_set_name (SoftAppThumbnail  *thb,
                             const char      *name)
{
    g_free (thb->soft_name);
    thb->soft_name = g_strdup (name);
}  

const gchar *
soft_app_thumbnail_get_icon (SoftAppThumbnail *thb)
{
    return thb->icon_name;
}

void
soft_app_thumbnail_set_icon (SoftAppThumbnail  *thb, 
                             const gchar     *icon)
{
    g_free (thb->icon_name);
    thb->icon_name = g_strdup (icon);
}  

static void
soft_app_thumbnail_finalize (GObject *object)
{
    SoftAppThumbnail *thb = SOFT_APP_THUMBNAIL (object);

    g_free (thb->soft_name);
    g_free (thb->icon_name);
}

static void
soft_app_thumbnail_class_init (SoftAppThumbnailClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = soft_app_thumbnail_finalize;
}

static void
soft_app_thumbnail_init (SoftAppThumbnail *thb)
{

}

SoftAppThumbnail *
soft_app_thumbnail_new (void)
{
    SoftAppThumbnail *thb;
    thb = g_object_new (SOFT_APP_TYPE_THUMBNAIL, NULL);
    
    return SOFT_APP_THUMBNAIL (thb);
}    
