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
#include "app-store-store.h"
#include "app-store-category.h"

static void GetSoftCate(SoftAppCategory *cate)
{

    cate->image_name = g_strdup("input-gaming");
    cate->soft_name  = g_strdup(_("game"));
    cate->color      = g_strdup("red");
}
void
gs_image_set_from_pixbuf_with_scale (GtkImage *image, const GdkPixbuf *pixbuf, gint scale)
{
    cairo_surface_t *surface;
    surface = gdk_cairo_surface_create_from_pixbuf (pixbuf, scale, NULL);
    if (surface == NULL)
        return;
    gtk_image_set_from_surface (image, surface);
    cairo_surface_destroy (surface);
}

void
gs_image_set_from_pixbuf (GtkImage *image, const GdkPixbuf *pixbuf)
{
    gint scale;
    scale = gdk_pixbuf_get_width (pixbuf) / 64;
    gs_image_set_from_pixbuf_with_scale (image, pixbuf, scale);
}
static GtkWidget *get_recent(void)
{
    GtkWidget *fixed,*button,*event_box,*vbox,*image,*label,*hbox;
    GdkPixbuf *pixbuf;

    fixed = gtk_fixed_new();
    button = gtk_button_new ();
    gtk_fixed_put(GTK_FIXED(fixed),button, 0, 0);
    gtk_widget_set_size_request(button, 110,100);
    event_box = gtk_event_box_new();
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    //gtk_widget_set_size_request (vbox, 80, 80);
    gtk_container_add (GTK_CONTAINER (button), event_box);
    gtk_container_add (GTK_CONTAINER (event_box), vbox);
    image = gtk_image_new();
    pixbuf = gdk_pixbuf_new_from_file("/tmp/user-admin.png",NULL);
    gs_image_set_from_pixbuf(GTK_IMAGE(image),pixbuf);
    gtk_box_pack_start(GTK_BOX(vbox),image ,TRUE, TRUE, 0);
    label = gtk_label_new(NULL);
	SetLableFontType(label,"black",10,_("user admin"),FALSE);
    //gtk_label_set_label(GTK_LABEL(label),"test");
	gtk_box_pack_start(GTK_BOX(vbox),label ,TRUE, TRUE, 0);
    
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	gtk_box_pack_start(GTK_BOX(vbox),hbox ,TRUE, TRUE, 0);
    int i = 5;
    while(i--)
    {     
    image = gtk_image_new();
    gtk_image_set_from_icon_name (GTK_IMAGE (image),
                                 "starred",
                                  GTK_ICON_SIZE_MENU);
	gtk_box_pack_start(GTK_BOX(hbox),image ,TRUE, TRUE, 0);
  //gtk_widget_show_all (button);
    }
  return fixed;
}    
GtkWidget *LoadStoreSoft(SoftAppStore *app)
{
    GtkWidget *vbox;
	GtkWidget *label;
    GtkWidget *flowbox;
    GtkWidget *tile;
    SoftAppCategory cate;
    guint i;
    GtkWidget *hbox;
    GtkWidget *recent;
    
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);  
	
	label = gtk_label_new(NULL);
	gtk_widget_set_halign (label, GTK_ALIGN_START);
	gtk_widget_set_valign (label, GTK_ALIGN_END);
	SetLableFontType(label,"black",14,_("soft category"),TRUE);
	gtk_box_pack_start(GTK_BOX(vbox),label ,TRUE, TRUE, 0);
    
    flowbox = gtk_flow_box_new ();
    gtk_widget_set_valign (flowbox, GTK_ALIGN_START);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX (flowbox), 3);
    gtk_flow_box_set_selection_mode (GTK_FLOW_BOX (flowbox), GTK_SELECTION_NONE);
    gtk_flow_box_set_column_spacing(GTK_FLOW_BOX (flowbox),12);
    gtk_flow_box_set_row_spacing(GTK_FLOW_BOX (flowbox),12);
	gtk_box_pack_start(GTK_BOX(vbox),flowbox ,TRUE, TRUE, 0);
    
    GetSoftCate(&cate);
    for (i = 0; i< 8 ; i++)
    {  
        tile = soft_app_category_tile_new (&cate); 
        gtk_container_add (GTK_CONTAINER (flowbox),tile);
    }
    label = gtk_label_new(NULL);
	gtk_widget_set_halign (label, GTK_ALIGN_START);
	gtk_widget_set_valign (label, GTK_ALIGN_END);
	SetLableFontType(label,"black",14,_("soft recommend"),TRUE);
	gtk_box_pack_start(GTK_BOX(vbox),label ,TRUE, TRUE, 0);
    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_box_pack_start(GTK_BOX(vbox),hbox ,TRUE, TRUE, 0);
    for (i = 0; i < 8; i++)
    {    
        recent = get_recent();
	    gtk_box_pack_start(GTK_BOX(hbox),recent ,FALSE, FALSE, 16);
    }
	return vbox;

}
