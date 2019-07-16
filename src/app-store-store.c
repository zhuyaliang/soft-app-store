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
static GtkWidget *add_new (void)
{
    GtkWidget *button,*hbox,*image,*label;

    button = gtk_button_new ();
    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);  
    gtk_container_add (GTK_CONTAINER (button), hbox);
    image = gtk_image_new();
    gtk_image_set_from_icon_name (GTK_IMAGE (image),
                                  "audio-x-generic",
                                  GTK_ICON_SIZE_MENU);
	gtk_box_pack_start(GTK_BOX(hbox),image ,TRUE, TRUE, 0);
    label = gtk_label_new(NULL);
    gtk_label_set_label(GTK_LABEL(label),"test");
	gtk_box_pack_start(GTK_BOX(hbox),label ,TRUE, TRUE, 0);

    gtk_widget_show_all (button);

  return button;
}

GtkWidget *LoadStoreSoft(SoftAppStore *app)
{
    GtkWidget *vbox;
	GtkWidget *label;
    GtkWidget *flowbox;
    guint i;

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);  
	
	label = gtk_label_new(NULL);
	gtk_widget_set_halign (label, GTK_ALIGN_START);
	gtk_widget_set_valign (label, GTK_ALIGN_CENTER);
	SetLableFontType(label,"black",13,_("soft category"));
	gtk_box_pack_start(GTK_BOX(vbox),label ,TRUE, TRUE, 0);
    
    flowbox = gtk_flow_box_new ();
    gtk_widget_set_valign (flowbox, GTK_ALIGN_START);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX (flowbox), 3);
    gtk_flow_box_set_selection_mode (GTK_FLOW_BOX (flowbox), GTK_SELECTION_NONE);
    gtk_flow_box_set_column_spacing(GTK_FLOW_BOX (flowbox),12);
    gtk_flow_box_set_row_spacing(GTK_FLOW_BOX (flowbox),12);
	gtk_box_pack_start(GTK_BOX(vbox),flowbox ,TRUE, TRUE, 0);

    for (i = 0; i< 8 ; i++)
        gtk_container_add (GTK_CONTAINER (flowbox), add_new ());
	
    label = gtk_label_new(NULL);
	gtk_widget_set_halign (label, GTK_ALIGN_START);
	gtk_widget_set_valign (label, GTK_ALIGN_START);
	SetLableFontType(label,"black",13,_("soft recommend"));
	gtk_box_pack_start(GTK_BOX(vbox),label ,TRUE, TRUE, 0);

	return vbox;

}
