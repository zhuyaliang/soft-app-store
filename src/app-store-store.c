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

typedef struct {
    const gchar *name;
    const gchar *icon;  
    const gchar *key_colors;
}SoftAppDesktopData;    

static const SoftAppDesktopData msdata[] = 
{
    {("Audio"),        "folder-music-symbolic",             "#215d9c"},
    {("Developer"),    "applications-engineering-symbolic", "#43d7c7"},
    {("Education"),    "system-help-symbolic",              "#5631c7"},
    {("Games"),        "applications-games-symbolic",       "#c4a000"},
    {("Graphics"),     "applications-graphics-symbolic",    "#75507b"},
    {("Productivity"), "text-editor-symbolic",              "#cc0000"},
    {("Communication"),"user-available-symbolic",           "#729fcf"},
    {("Reference"),    "view-dual-symbolic",                "#6a1354"},
    {("Utilities"),    "applications-utilities-symbolic",   "#8fc1ac"},
    { NULL }
};

static GPtrArray *GetCategoryInfo(SoftAppStore *app)
{
    SoftAppCategory *cate;
    GdkRGBA          key_color;
    gint             i;
    GPtrArray       *list = NULL;
    
    list = g_ptr_array_new ();
    for (i = 0; msdata[i].name != NULL; i++)
    {
        cate = soft_app_category_new (msdata[i].name);
        soft_app_category_set_icon (cate, msdata[i].icon);
        gdk_rgba_parse (&key_color, msdata[i].key_colors);
        soft_app_category_add_key_color (cate, &key_color);
        g_ptr_array_add (list, cate);
    }   

    return list;
}

static void GetRecommendSoftInfo(SoftAppThumbnail *thum)
{

    thum->image_name = g_strdup("/tmp/user-admin.png");
    thum->app_name   = g_strdup(_("user-admin"));
    thum->level      = 3.5;
}   
static void category_tile_clicked (SoftAppCategoryTile *tile, GtkWidget *stack)
{
    gtk_stack_set_visible_child_name (GTK_STACK (stack), "info");
    g_print("sssssssssssssssss\r\n");

}    

static GtkWidget *LoadSoftInfo(void)
{
    GtkWidget *vbox;
	GtkWidget *label;

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);  
    	
	label = gtk_label_new(NULL);
	gtk_widget_set_halign (label, GTK_ALIGN_START);
	gtk_widget_set_valign (label, GTK_ALIGN_END);
	SetLableFontType(label,"black",14,_("soft category"),TRUE);
	gtk_box_pack_start(GTK_BOX(vbox),label ,TRUE, TRUE, 0);
    
    label = gtk_label_new(NULL);
	gtk_widget_set_halign (label, GTK_ALIGN_START);
	gtk_widget_set_valign (label, GTK_ALIGN_END);
	SetLableFontType(label,"black",14,_("soft recommend"),TRUE);
	gtk_box_pack_start(GTK_BOX(vbox),label ,TRUE, TRUE, 0);
    
    return vbox;
}   

static GtkWidget *CategorySoftWindow(GtkWidget *vbox)
{
	GtkWidget *label;
    GtkWidget *flowbox;
    	
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
   
    return flowbox;
}   

static GtkWidget *RecommendSoftWindow (GtkWidget *vbox)
{
	GtkWidget *label;
    GtkWidget *hbox;
    
    label = gtk_label_new(NULL);
	gtk_widget_set_halign (label, GTK_ALIGN_START);
	gtk_widget_set_valign (label, GTK_ALIGN_END);
	SetLableFontType(label,"black",14,_("soft recommend"),TRUE);
	gtk_box_pack_start(GTK_BOX(vbox),label ,TRUE, TRUE, 0);
    
    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_box_pack_start(GTK_BOX(vbox),hbox ,TRUE, TRUE, 0);

    return hbox;

}    
static GtkWidget *CreateStoreMainWindow(SoftAppStore *app)
{
    GtkWidget *vbox;
    GtkWidget *flowbox;
    GPtrArray *list;
    GtkWidget *tile;
    SoftAppCategory *cate;
    SoftAppThumbnail soft_app;
    GtkWidget *Recom;
    GtkWidget *hbox;
    guint      i;

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);  
    
    flowbox = CategorySoftWindow(vbox);
    list = GetCategoryInfo(app);
     
    for (i = 0; i < list->len; i++)
    {
        cate = SOFT_APP_CATEGORY (g_ptr_array_index (list, i));
        tile = soft_app_category_tile_new (cate);
        g_signal_connect (tile, 
                         "clicked",
                          G_CALLBACK (category_tile_clicked), 
                          app->StoreStack);
        gtk_container_add (GTK_CONTAINER (flowbox),tile);
    }

    hbox = RecommendSoftWindow(vbox);
    GetRecommendSoftInfo(&soft_app);
    for (i = 0; i < 8; i++)
    {    
        Recom = soft_app_thumbnail_tile_new (&soft_app);
	    gtk_box_pack_start(GTK_BOX(hbox),Recom ,FALSE, FALSE, 16);
    }

    return vbox;
}    
GtkWidget *LoadStoreSoft(SoftAppStore *app)
{
    GtkWidget *vbox;

    app->StoreStack = gtk_stack_new ();
    
    vbox = CreateStoreMainWindow(app);
    gtk_stack_add_named (GTK_STACK (app->StoreStack), vbox, "formatted");
    //CreateStoreCategorySoft();
    gtk_stack_add_named (GTK_STACK (app->StoreStack), LoadSoftInfo(),"info");
    //CreateStoreIndividualSoft();
    //gtk_stack_add_named (GTK_STACK (stack), LoadSoftInfo(),"info");

	return app->StoreStack;

}
