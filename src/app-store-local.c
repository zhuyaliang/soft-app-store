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
#include "app-store-local.h"
#include "app-store-row.h"

static GPtrArray *GetLocalSoftMessage(SoftAppStore *app)
{
	gint i;
	SoftAppMessage *Message;
    GPtrArray       *list = NULL;
    
    list = g_ptr_array_new ();
	for(i = 0; i < 100; i++)
	{
		Message = soft_app_message_new ();
		soft_app_message_set_name(Message,_("time-admin"));
		soft_app_message_set_icon(Message,"/tmp/time-admin.png");
		soft_app_message_set_score(Message,(i%5)+0.5);
		soft_app_message_set_describe(Message,_("Marks row as changed, causing any state that depends on this to be updated.This affects sorting, filtering and headers."));
		soft_app_message_set_size(Message,"35.6M");
		soft_app_message_set_buttontype(Message,TRUE);
        g_ptr_array_add (list, Message);
	}

	return list;
}
static void SwitchPageToIndividualDetailsPage (GtkListBox    *list_box,
                                               GtkListBoxRow *Row,
                                               SoftAppStore  *app)

{
	SoftAppRow *row = SOFT_APP_ROW(Row); 
    app->page = INDIVIDUAL_SOFT_PAGE;
	g_print("abcdefg = %f\r\n",soft_app_message_get_score(row->Message));
	SwitchPage(app);
}
GtkWidget *LoadLocalInstall(SoftAppStore *app)
{
    GtkWidget *vbox;
    GtkWidget *sw;
    GPtrArray *list;
	GtkWidget *listbox;
	SoftAppMessage *Message;
	GtkWidget *row;
	guint       i;

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
	sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
	listbox = gtk_list_box_new ();
	gtk_container_add (GTK_CONTAINER (sw), listbox);
	
    g_signal_connect (listbox, 
                     "row-activated",
                      G_CALLBACK (SwitchPageToIndividualDetailsPage), 
                      app);
	list = GetLocalSoftMessage(app);
    for (i = 0; i < list->len; i++)
    {
        Message = SOFT_APP_MESSAGE (g_ptr_array_index (list, i));
		row = soft_app_row_new(Message);
		gtk_widget_set_halign(row, GTK_ALIGN_CENTER);
		gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(row),TRUE);

		app->page = MAIN_PAGE;
		gtk_list_box_insert (GTK_LIST_BOX(listbox), row, i);
	}
	return vbox;
}
