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
#include "app-store-search.h"
#include "app-store-row.h"
#include "app-store-details.h"
#include "app-store-pkgkit.h"

static void GetLocalSoftSearchResult (const char *SoftName,SoftAppStore *app)
{
	SoftAppMessage *Message;
	const char *package_id;
	GtkWidget  *row;
	guint       i;
    g_auto(GStrv)     split = NULL;

    for (i = 0; i < app->pkg->list->len; i++)
    {
        Message = SOFT_APP_MESSAGE (g_ptr_array_index (app->pkg->list, i));
		package_id = soft_app_message_get_pkgname (Message);
		split = pk_package_id_split (package_id);

		if (strstr(split[PK_PACKAGE_ID_NAME],SoftName) == NULL)
		{
			continue;
		}
		row = soft_app_row_new(Message);
		gtk_widget_set_halign(row, GTK_ALIGN_CENTER);
		gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(row),TRUE);
		gtk_list_box_insert (GTK_LIST_BOX(app->SearchSoftListBox), row, i);
	}
	gtk_widget_show_all(app->SearchSoftListBox);
}
static void StartSearchSoftName (GtkSearchEntry *entry,SoftAppStore *app)
{
	const char *SoftName;
	int         current_page;

	SoftName = gtk_entry_get_text (GTK_ENTRY(entry));
	if (strlen(SoftName) <= 2)
	{
		return;
	}
	current_page = gtk_notebook_get_current_page (GTK_NOTEBOOK(app->NoteBook));
	if (current_page == 0)
	{
		;
	}
	else if (current_page == 1)
	{
		SoftAppStoreLog ("Debug","start search local soft %s",SoftName);
		soft_app_container_remove_all (GTK_CONTAINER (app->SearchSoftListBox));
		GetLocalSoftSearchResult (SoftName,app);
		SoftAppStoreLog ("Debug","finish Successfu search local soft");
	}
}
static void LoadSearchSoftPage(SoftAppStore *app,int current_page)
{
	GtkWidget *label;
	GtkWidget *entry;
	GtkWidget *container;
	GtkWidget *searchbar;
    GtkWidget *sw;

	label = gtk_label_new (NULL);
	if (current_page == 1)
	{
		SetLableFontType (label,"black",15,_("Search for local packages enter the correct software name"),TRUE);
	}
	else if (current_page == 0)
	{
		SetLableFontType (label,"black",15,_("Search for store packages enter the correct software name"),TRUE);
	}
	else
	{
		return;
	}
    gtk_box_pack_start (GTK_BOX (app->StackSearchBox), label, FALSE, FALSE, 0);
	entry = gtk_search_entry_new ();
    container = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign (container, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (container), entry, FALSE, FALSE, 0);
    searchbar = gtk_search_bar_new ();
    gtk_search_bar_connect_entry (GTK_SEARCH_BAR (searchbar), GTK_ENTRY (entry));
    gtk_search_bar_set_show_close_button (GTK_SEARCH_BAR (searchbar), FALSE);
    gtk_container_add (GTK_CONTAINER (searchbar), container);
    gtk_box_pack_start (GTK_BOX (app->StackSearchBox), searchbar, FALSE, FALSE, 0);
	gtk_search_bar_set_search_mode (GTK_SEARCH_BAR (searchbar), TRUE);
	g_signal_connect (entry, 
                     "search-changed",
                      G_CALLBACK (StartSearchSoftName), 
                      app);
	
	sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start (GTK_BOX (app->StackSearchBox), sw, TRUE, TRUE, 0);
	app->SearchSoftListBox = gtk_list_box_new ();
	gtk_container_add (GTK_CONTAINER (sw), app->SearchSoftListBox);
    g_signal_connect (app->SearchSoftListBox, 
                     "row-activated",
                      G_CALLBACK (SwitchPageToIndividualDetailsPage), 
                      app);
	
	gtk_widget_show (app->StackSearchBox);
	gtk_widget_show (label);
	gtk_widget_show (entry);
	gtk_widget_show (container);
	gtk_widget_show (searchbar);
	gtk_widget_show (sw);
	gtk_widget_show (app->SearchSoftListBox);
}
void SwitchPageToSearch (GtkWidget *button, SoftAppStore *app)
{
	int        current_page;
	
	current_page = gtk_notebook_get_current_page (GTK_NOTEBOOK(app->NoteBook));
	if (current_page == 0)
	{
		;
	}
	else if (current_page == 1 && app->pkg->action != GPK_ACTION_DONE)
	{
		MessageReport (_("Search soft"),_("The local package was not loaded successfully. Please try again later"),INFOR);
		return;
	}
	else if (current_page == 2)
	{
		return;
	}
	soft_app_container_remove_all (GTK_CONTAINER (app->StackSearchBox));
	app->page = SEARCH_SOFT_PAGE;
	SwitchPage(app);
    LoadSearchSoftPage(app,current_page);
}
