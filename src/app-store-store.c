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

GtkWidget *LoadStoreSoft(SoftAppStore *app)
{
    GtkWidget *vbox;
	GtkWidget *label;

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);  
	
	label = gtk_label_new(NULL);
	gtk_widget_set_halign (label, GTK_ALIGN_START);
	gtk_widget_set_valign (label, GTK_ALIGN_START);
	SetLableFontType(label,"black",13,_("soft category"));
	gtk_box_pack_start(GTK_BOX(vbox),label ,TRUE, TRUE, 0);


	label = gtk_label_new(NULL);
	gtk_widget_set_halign (label, GTK_ALIGN_START);
	gtk_widget_set_valign (label, GTK_ALIGN_START);
	SetLableFontType(label,"black",13,_("soft recommend"));
	gtk_box_pack_start(GTK_BOX(vbox),label ,TRUE, TRUE, 0);

	return vbox;

}
