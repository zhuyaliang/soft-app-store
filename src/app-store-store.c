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
#include "app-store-thumbnail.h"

static void GetSoftCate(SoftAppCategory *cate)
{

    cate->image_name = g_strdup("input-gaming");
    cate->soft_name  = g_strdup(_("game"));
    cate->color      = g_strdup("red");
}

static void GetSoftInfo(SoftAppThumbnail *thum)
{

    thum->image_name = g_strdup("/tmp/user-admin.png");
    thum->app_name   = g_strdup(_("time-admin"));
    thum->level      = 3.5;
}    
GtkWidget *LoadStoreSoft(SoftAppStore *app)
{
    GtkWidget *vbox;
	GtkWidget *label;
    GtkWidget *flowbox;
    GtkWidget *tile;
    SoftAppCategory cate;
    SoftAppThumbnail soft_app;
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

    GetSoftInfo(&soft_app);
    for (i = 0; i < 8; i++)
    {    
        recent = soft_app_thumbnail_tile_new (&soft_app);
	    gtk_box_pack_start(GTK_BOX(hbox),recent ,FALSE, FALSE, 16);
    }
	return vbox;

}
