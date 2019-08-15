/*  soft-app-store 
*   Copyright (C) 2018  zhuyaliang https://github.com/zhuyaliang/
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "app-store-util.h"
#include "app-store-store.h"
#include "app-store-local.h"
#include "app-store-update.h"
#include "app-store-details.h"
#include "app-store-pkgkit.h"
#include <sys/stat.h>
#define  LOCKFILE              "/tmp/soft-app-store.pid"
#define  APPICON               "soft-app-store.png"

static gboolean on_window_quit (GtkWidget    *widget, 
                                GdkEvent     *event, 
                                SoftAppStore *app)
{
    CloseLogFile();  

	if (app->pkg->control != NULL)
        g_object_unref (app->pkg->control);
    if (app->pkg->task != NULL)
        g_object_unref (app->pkg->task);
    if (app->pkg->cancellable != NULL)
        g_object_unref (app->pkg->cancellable);
    if (app->pkg->package_sack != NULL)
        g_object_unref (app->pkg->package_sack);
    if (app->pkg->repos != NULL)
        g_hash_table_destroy (app->pkg->repos);

    gtk_main_quit();
    return TRUE;
}
static GdkPixbuf * GetAppIcon(void)
{
    GdkPixbuf *Pixbuf;
    GError    *Error = NULL;

    Pixbuf = gdk_pixbuf_new_from_file(ICONDIR APPICON,&Error);
    if(!Pixbuf)
    {
        MessageReport(_("Get Icon Fail"),Error->message,ERROR);
        SoftAppStoreLog("Warning","mate-user-admin user-admin.png %s",Error->message);
        g_error_free(Error);
    }   
    
    return Pixbuf;
}    
static void InitMainWindow(SoftAppStore *app)
{
    GdkPixbuf *AppIcon;
    char      *home,*dname;

    app->MainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(app->MainWindow), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(app->MainWindow),_("Soft App Store")); 
    gtk_container_set_border_width(GTK_CONTAINER(app->MainWindow),10);
    gtk_widget_set_size_request(app->MainWindow, 1000, 600);
    g_signal_connect(G_OBJECT(app->MainWindow), 
                    "delete-event",
                     G_CALLBACK(on_window_quit),
                     app);
    
    app->Header = gtk_header_bar_new (); 
    gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (app->Header), TRUE);
    gtk_header_bar_set_has_subtitle (GTK_HEADER_BAR (app->Header), TRUE);
    gtk_window_set_titlebar (GTK_WINDOW (app->MainWindow), app->Header);

    AppIcon = GetAppIcon();
    if(AppIcon)
    {
        gtk_window_set_icon(GTK_WINDOW(app->MainWindow),AppIcon);
        g_object_unref(AppIcon);
    }   
    app->StoreStack = gtk_stack_new ();
    gtk_container_add(GTK_CONTAINER(app->MainWindow), app->StoreStack);
    
	home = getenv("HOME");
    dname = g_strconcat(home,"/.soft-app-store",NULL);
    if(access(dname,F_OK) != 0)
    {
		SoftAppStoreLog ("Debug","Create cache dir %s",dname);
        mkdir(dname,0755);
    }    
    g_free(dname);
}

static void InitWelcomeInterface (SoftAppStore *app)
{
    GtkWidget *overlay;
    g_autoptr(GdkPixbuf) pixbuf = NULL;
    GtkWidget *image;

    overlay = gtk_overlay_new ();
    pixbuf = gdk_pixbuf_new_from_file(ICONDIR APPICON,NULL);
    image = gtk_image_new ();
    soft_app_image_set_from_pixbuf(GTK_IMAGE(image),pixbuf,128);
    gtk_overlay_add_overlay (GTK_OVERLAY (overlay), image);
    gtk_overlay_set_overlay_pass_through (GTK_OVERLAY (overlay), image, TRUE);
    gtk_widget_set_halign (image, GTK_ALIGN_CENTER);
    gtk_widget_set_valign (image, GTK_ALIGN_CENTER);
    gtk_stack_add_named (GTK_STACK (app->StoreStack), 
                         overlay,
                        "welcome-page"); 
}    

static void InitNoteBook (SoftAppStore *app)
{
    GtkWidget *NoteName;
    
    app->NoteBook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK (app->NoteBook), GTK_POS_TOP);
    
    NoteName = gtk_label_new(_("all store"));
    app->StoreBox = LoadStoreSoft(app);
    gtk_notebook_append_page(GTK_NOTEBOOK (app->NoteBook),
			                 app->StoreBox,
							 NoteName);

    NoteName = gtk_label_new(_("local install"));
    app->LocalBox = LoadLocalInstall(app);
    gtk_notebook_append_page(GTK_NOTEBOOK (app->NoteBook),
			                 app->LocalBox,
							 NoteName);

    NoteName = gtk_label_new(_("local update"));
    app->UpdateBox = LoadLocalUpdate(app);
    gtk_notebook_append_page(GTK_NOTEBOOK (app->NoteBook),
			                 app->UpdateBox,
							 NoteName);
    
    gtk_stack_add_named (GTK_STACK (app->StoreStack),
                         app->NoteBook,
                        "main-page");
	
    app->StackCategoryBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    gtk_stack_add_named (GTK_STACK (app->StoreStack), 
                         app->StackCategoryBox,
                        "list-page");
    
    app->StackDetailsBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    gtk_stack_add_named (GTK_STACK (app->StoreStack), 
                         app->StackDetailsBox,
                        "details-page");
    
    app->StackSearchBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    gtk_stack_add_named (GTK_STACK (app->StoreStack), 
                         app->StackSearchBox,
                        "search-page");
	gtk_widget_show_all(app->MainWindow);
}    
static int RecordPid(void)
{
    int pid = 0;
    int fd;
    int Length = 0; 
    char WriteBuf[30] = { 0 };
    fd = open(LOCKFILE,O_WRONLY|O_CREAT|O_TRUNC,0777);
    if(fd < 0)
    {
         MessageReport(_("open file"),_("Create pid file failed"),ERROR);
         return -1;      
    }       
    chmod(LOCKFILE,0777); 
    pid = getpid();
    sprintf(WriteBuf,"%d",pid);
    Length = write(fd,WriteBuf,strlen(WriteBuf));
    if(Length <= 0 )
    {
        MessageReport(_("write file"),_("write pid file failed"),ERROR);
        return -1;      
    }			
    close(fd);

    return 0;
}        
/******************************************************************************
* Function:              ProcessRuning      
*        
* Explain: Check whether the process has been started,If the process is not started, 
*          record the current process ID =====>"/tmp/user-admin.pid"
*        
* Input:         
*        
*        
* Output:  start        :TRUE
*          not start    :FALSE
*        
* Author:  zhuyaliang  31/07/2018
******************************************************************************/
static gboolean ProcessRuning(void)
{
    int fd;
    int pid = 0;
    gboolean Run = FALSE;
    char ReadBuf[30] = { 0 };

    if(access(LOCKFILE,F_OK) == 0)
    {
        fd = open(LOCKFILE,O_RDONLY);
        if(fd < 0)
        {
             MessageReport(_("open file"),_("open pid file failed"),ERROR);
             return TRUE;
        }        
        if(read(fd,ReadBuf,sizeof(ReadBuf)) <= 0)
        {
             MessageReport(_("read file"),_("read pid file failed"),ERROR);
             goto ERROREXIT;
        }        
        pid = atoi(ReadBuf);
        if(kill(pid,0) == 0)
        {        
             goto ERROREXIT;
        }
    }
    
    if(RecordPid() < 0)
        Run = TRUE;
    
    return Run;
ERROREXIT:
    close(fd);
    return TRUE;

}        

int main(int argc, char **argv)
{
	
	SoftAppStore app;
	
    bindtextdomain (GETTEXT_PACKAGE,LUNAR_CALENDAR_LOCALEDIR);   
    textdomain (GETTEXT_PACKAGE); 
    
    gtk_init(&argc, &argv);
    
    /* Create the main window */
	InitMainWindow(&app);

    /* Create Welcome interface*/
    InitWelcomeInterface (&app);

	/* Create book page*/
	InitNoteBook(&app);

    /* Check whether the process has been started */
    if(ProcessRuning() == TRUE)
    {
        SoftAppStoreLog("Info","The mate-user-admin process already exists");
        exit(0);        
    }

    gtk_main();

}
