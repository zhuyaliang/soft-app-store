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

static void
soft_app_category_tile_refresh (SoftAppCategoryTile *tile)
{
	GtkStyleContext *context;
	g_autoptr(GtkCssProvider) provider = NULL;
	g_autofree gchar *css = NULL;
    GError *error = NULL;

	context = gtk_widget_get_style_context (GTK_WIDGET (tile));
    gtk_widget_set_size_request(GTK_WIDGET (tile), 10, 10);
	//gtk_label_set_label (GTK_LABEL (tile->label),
	//		             tile->soft_name);

	SetLableFontType(tile->label,"black",12,tile->soft_name,TRUE);
	gtk_image_set_from_icon_name (GTK_IMAGE (tile->image),
				                  tile->image_name,
				                  GTK_ICON_SIZE_MENU);
	css = g_strdup_printf ("button {border-bottom-color:rgb(111,11,111);border-bottom-width:3px;padding-top:11px}");
    provider = gtk_css_provider_new ();
    gtk_css_provider_load_from_data (provider, css, -1, &error);
	gtk_style_context_add_provider (context, 
                                    GTK_STYLE_PROVIDER (provider),
						            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

void
soft_app_category_tile_set_category (SoftAppCategoryTile *tile, SoftAppCategory *cate)
{
	tile->soft_name = g_strdup(cate->soft_name);
	tile->image_name = g_strdup(cate->image_name);
	tile->color = g_strdup(cate->color);
	soft_app_category_tile_refresh (tile);
}

static void
soft_app_category_tile_destroy (GtkWidget *widget)
{
	SoftAppCategoryTile *tile = SOFT_APP_CATEGORY_TILE (widget);
    g_free(tile->soft_name);
    g_free(tile->image_name);
    g_free(tile->color);
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
