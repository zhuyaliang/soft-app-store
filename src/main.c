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
#define  WINWIDTH               1200
#define  WINHIGTH               640 

static gboolean on_window_quit (GtkWidget    *widget, 
                                GdkEvent     *event, 
                                SoftAppStore *app)
{
    CloseLogFile();  
    if (app->server_addr != NULL)
    {
        g_free (app->server_addr);
    }    
    if (app->server_port != NULL)
    {
        g_free (app->server_port);
    }    
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
    gtk_widget_set_size_request(app->MainWindow, 1200, 600);
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
static gboolean
draw_text (GtkWidget *da,
           cairo_t   *cr,
           gpointer   data)
{
	cairo_pattern_t *pattern;
	PangoLayout *layout;
	PangoFontDescription *desc;
	int text_w;
	int text_h;
	int width,higth;
	const char *text = (const char *)data; 
	cairo_save (cr);

	layout = gtk_widget_create_pango_layout (da, text);
	desc = pango_font_description_from_string ("sans bold 22");
	pango_layout_set_font_description (layout, desc);
	pango_font_description_free (desc);

	pango_layout_get_size (layout,&text_w,&text_h); 
	width = (WINWIDTH  - PANGO_PIXELS_FLOOR(text_w)) / 2;
	higth = ((WINHIGTH - PANGO_PIXELS_FLOOR(text_h)) / 2 + 80);
	cairo_move_to (cr, width, higth);
	pango_cairo_layout_path (cr, layout);
	g_object_unref (layout);

	pattern = cairo_pattern_create_linear (0.0, 0.0,
		                                   gtk_widget_get_allocated_width (da),
                                           gtk_widget_get_allocated_height (da));
	cairo_pattern_add_color_stop_rgb (pattern, 0.0, 1.0, 0.0, 0.0);
	cairo_pattern_add_color_stop_rgb (pattern, 0.2, 1.0, 0.0, 0.0);
	cairo_pattern_add_color_stop_rgb (pattern, 0.3, 1.0, 1.0, 0.0);
	cairo_pattern_add_color_stop_rgb (pattern, 0.4, 0.0, 1.0, 0.0);
	cairo_pattern_add_color_stop_rgb (pattern, 0.6, 0.0, 1.0, 1.0);
	cairo_pattern_add_color_stop_rgb (pattern, 0.7, 0.0, 0.0, 1.0);
	cairo_pattern_add_color_stop_rgb (pattern, 0.8, 1.0, 0.0, 1.0);
	cairo_pattern_add_color_stop_rgb (pattern, 1.0, 1.0, 0.0, 1.0);

	cairo_set_source (cr, pattern);
	cairo_fill_preserve (cr);

	cairo_pattern_destroy (pattern);

	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_set_line_width (cr, 0.5);
	cairo_stroke (cr);

	cairo_restore (cr);

	return TRUE;
}
static gboolean UpdateProgress (gpointer data)
{
	static double i = 0.0;
	SoftAppStore *app = (SoftAppStore *)data;

	if (i <= 1.0)
	{
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(app->WelcomeBar),i);
		i += 0.2;
		return TRUE;
	}
	gtk_stack_set_visible_child_name (GTK_STACK(app->StoreStack),"main-page");
    
    gtk_widget_show (app->button_search);
    gtk_widget_show (app->button_return);
	
    return FALSE;
}

static void InitWelcomeInterface (SoftAppStore *app)
{
    GtkWidget *overlay;
    g_autoptr(GdkPixbuf) pixbuf = NULL;
    GtkWidget *image;
	GtkWidget *da;
    
    overlay = gtk_overlay_new ();
    pixbuf = gdk_pixbuf_new_from_file(ICONDIR APPICON,NULL);
    image = gtk_image_new ();
    soft_app_image_set_from_pixbuf(GTK_IMAGE(image),pixbuf,128);
    gtk_overlay_add_overlay (GTK_OVERLAY (overlay), image);
    gtk_overlay_set_overlay_pass_through (GTK_OVERLAY (overlay), image, TRUE);
    gtk_widget_set_halign (image, GTK_ALIGN_CENTER);
    gtk_widget_set_valign (image, GTK_ALIGN_CENTER);
	da = gtk_drawing_area_new ();
	
    gtk_overlay_add_overlay (GTK_OVERLAY (overlay), da);
	g_signal_connect (da, 
					 "draw",
                      G_CALLBACK (draw_text), 
					 _("Loading software catalog"));

    app->WelcomeBar =  gtk_progress_bar_new ();
    gtk_overlay_add_overlay (GTK_OVERLAY (overlay), app->WelcomeBar);
	g_timeout_add (250,UpdateProgress,app);
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
    gtk_widget_hide (app->button_search);
    gtk_widget_hide (app->button_return);
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
static gboolean LoadServerConfig (SoftAppStore *app)
{
    GKeyFile         *Kconfig = NULL;
    g_autoptr(GError) error = NULL;

    Kconfig = g_key_file_new();
    if(Kconfig == NULL)
    {
        SoftAppStoreLog ("Warning","g_key_file_new %s fail",SERVERCONFIG);
        return FALSE;
    }
    if(!g_key_file_load_from_file(Kconfig, SERVERCONFIG, G_KEY_FILE_NONE, &error))
    {
        SoftAppStoreLog ("Warning","Error loading key file: %s", error->message);
        g_key_file_free(Kconfig);
        return FALSE;
    }
    app->server_addr = g_key_file_get_string (Kconfig,"server","ipaddr",&error);
    app->server_port = g_key_file_get_string (Kconfig,"server","port",&error);

    if (app->server_addr == NULL || app->server_port == NULL)
    {
        g_key_file_free(Kconfig);
        return FALSE;
    }    

    return TRUE;
}    
int main(int argc, char **argv)
{
	
	SoftAppStore app;
	
    bindtextdomain (GETTEXT_PACKAGE,LUNAR_CALENDAR_LOCALEDIR);   
    textdomain (GETTEXT_PACKAGE); 
    
    gtk_init(&argc, &argv);
   
    /*read config soft-app-store/appstore-server.ini*/
    if (!LoadServerConfig (&app))
    {
        MessageReport ("Read appstore-server.ini",
                      _("Check whether /etc/soft-app-store/appstore-server.ini is set first or not"),
                       WARING);
    }    
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
