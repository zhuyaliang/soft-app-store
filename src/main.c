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

#define  LOCKFILE              "/tmp/soft-app-store.pid"
#define  APPICON               "soft-app-store.png"

static gboolean on_window_quit (GtkWidget    *widget, 
                                GdkEvent     *event, 
                                SoftAppStore *app)
{
    CloseLogFile();    
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

    app->MainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(app->MainWindow), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(app->MainWindow),_("Soft App Store")); 
    gtk_container_set_border_width(GTK_CONTAINER(app->MainWindow),10);
    gtk_widget_set_size_request(app->MainWindow, 1000, 600);
    g_signal_connect(G_OBJECT(app->MainWindow), 
                    "delete-event",
                     G_CALLBACK(on_window_quit),
                     app);
    
    AppIcon = GetAppIcon();
    if(AppIcon)
    {
        gtk_window_set_icon(GTK_WINDOW(app->MainWindow),AppIcon);
        g_object_unref(AppIcon);
    }   
}

static void InitNoteBook (SoftAppStore *app)
{
    GtkWidget *NoteName;
    
    app->NoteBook = gtk_notebook_new();
    gtk_container_add(GTK_CONTAINER(app->MainWindow), app->NoteBook);
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
    
    InitPackageKit(&app);
    /* Create the main window */
	InitMainWindow(&app);
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
