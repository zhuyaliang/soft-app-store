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

#include "app-store-category.h"
#include "app-store-util.h"

G_DEFINE_TYPE (SoftAppCategoryTile, soft_app_category_tile, GTK_TYPE_BUTTON)
G_DEFINE_TYPE (SoftAppCategory, soft_app_category, G_TYPE_OBJECT)

static void
soft_app_category_tile_refresh (SoftAppCategoryTile *tile)
{
    GPtrArray *key_colors;
	GtkStyleContext *context;
	g_autoptr(GtkCssProvider) provider = NULL;
	g_autofree gchar *css = NULL;
	g_autofree gchar *color = NULL;
    GError   *error = NULL;
    GdkRGBA  *tmp;

	context = gtk_widget_get_style_context (GTK_WIDGET (tile));
    gtk_widget_set_size_request(GTK_WIDGET (tile), 10, 10);

	SetLableFontType(tile->label,
                    "black",
                    12,
                    soft_app_category_get_name (tile->cate),
                    TRUE);

	gtk_image_set_from_icon_name (GTK_IMAGE (tile->image),
				                  soft_app_category_get_icon (tile->cate),
				                  GTK_ICON_SIZE_MENU);
    key_colors = soft_app_category_get_key_colors (tile->cate);
    tmp = g_ptr_array_index (key_colors, 0);
    color = gdk_rgba_to_string (tmp);

	css = g_strdup_printf ("button {border-bottom-color:%s;border-bottom-width:3px;padding-top:11px}",color);
    provider = gtk_css_provider_new ();
    gtk_css_provider_load_from_data (provider, css, -1, &error);
	gtk_style_context_add_provider (context, 
                                    GTK_STYLE_PROVIDER (provider),
						            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

void
soft_app_category_tile_set_category (SoftAppCategoryTile *tile, SoftAppCategory *cate)
{
	g_set_object (&tile->cate, cate);
    soft_app_category_tile_refresh (tile);
}

static void
soft_app_category_tile_destroy (GtkWidget *widget)
{
	SoftAppCategoryTile *tile = SOFT_APP_CATEGORY_TILE (widget);
    g_clear_object (&tile->cate);
}

static void
soft_app_category_tile_init (SoftAppCategoryTile *tile)
{
    GtkWidget *hbox;
	gtk_widget_set_has_window (GTK_WIDGET (tile), FALSE);

    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);  
    gtk_container_add (GTK_CONTAINER (tile), hbox);
    
    tile->image = gtk_image_new();
	gtk_box_pack_start(GTK_BOX(hbox),tile->image ,FALSE,FALSE, 10);
    
    tile->label = gtk_label_new(NULL);
	gtk_box_pack_start(GTK_BOX(hbox),tile->label ,FALSE,FALSE, 0);

}

static void
soft_app_category_tile_class_init (SoftAppCategoryTileClass *klass)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	widget_class->destroy = soft_app_category_tile_destroy;
}

GtkWidget *
soft_app_category_tile_new (SoftAppCategory *cate)
{
	SoftAppCategoryTile *tile;

	tile = g_object_new (SOFT_APP_TYPE_CATEGORY_TILE, NULL);
	soft_app_category_tile_set_category (tile, cate);

	return GTK_WIDGET (tile);
}

const gchar *
soft_app_category_get_name (SoftAppCategory *category)
{
    return category->soft_name;
}

const gchar *
soft_app_category_get_icon (SoftAppCategory *category)
{
    return category->icon_name;
}

void
soft_app_category_set_icon (SoftAppCategory *category, 
                            const gchar     *icon)
{
    g_free (category->icon_name);
    category->icon_name = g_strdup (icon);
}  

GPtrArray *
soft_app_category_get_key_colors (SoftAppCategory *category)
{
    return category->key_colors;
}

void
soft_app_category_add_key_color (SoftAppCategory *category, 
                                 const GdkRGBA   *key_color)
{
    g_return_if_fail (key_color != NULL);
    g_ptr_array_add (category->key_colors, gdk_rgba_copy (key_color));
}

static void
soft_app_category_finalize (GObject *object)
{
    SoftAppCategory *category = SOFT_APP_CATEGORY (object);

    g_ptr_array_unref (category->key_colors);
    g_free (category->soft_name);
    g_free (category->icon_name);
}

static void
soft_app_category_class_init (SoftAppCategoryClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = soft_app_category_finalize;
}

static void
soft_app_category_init (SoftAppCategory *category)
{
    category->key_colors = g_ptr_array_new_with_free_func ((GDestroyNotify) gdk_rgba_free);
}

SoftAppCategory *
soft_app_category_new (const gchar *name)
{
    SoftAppCategory *category;
    category = g_object_new (SOFT_APP_TYPE_CATEGORY, NULL);
    category->soft_name = g_strdup (name);
    
    return SOFT_APP_CATEGORY (category);
}
