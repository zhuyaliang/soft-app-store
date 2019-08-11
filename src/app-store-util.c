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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define __USE_SOURCE
#include <time.h> 
#include "app-store-util.h"

#define  GSM_GSETTINGS_SCHEMA "org.mate.interface"
static int file_dp = 0;  /*Log file descriptor*/

static int CreateLogFile (void)
{
    time_t t;
    int    t_stamp;
    char  *file_name;

    if(file_dp > 0)
    {
        return file_dp;
    }    
    t_stamp = time(&t);
    file_name = g_strdup_printf("/tmp/log-%d",t_stamp);

    file_dp = open(file_name,O_RDWR|O_CREAT,0666);
    
    g_free(file_name);    
    return file_dp;
}
/*Record error information to log file*/
void SoftAppStoreLog (const char *level,const char *message,...)
{
    int     fd;
    va_list args;
    char   *file_data;
    char    buf[256]; 
    int     len;

    fd = CreateLogFile();
    if(fd < 0)
    {
        return;
    }    
    va_start(args,message);
    vsprintf(buf,message, args);
    va_end(args);
    file_data = g_strdup_printf("level:[%s]  message: %s\r\n",level,buf);
    len = write(fd,file_data,strlen(file_data));
    if(len <= 0)
    {
        MessageReport("write log","write log error",ERROR);	
    }
}    

void CloseLogFile (void)
{
    if(file_dp > 0 )
    {
        close(file_dp);
        file_dp = 0;
    }    
}    
/******************************************************************************
* Function:            WhetherUseHeadbar
*        
* Explain: Determine whether the system opens the hearbar option
*          
* Input:  NULL
*
* Output:
*		
*		TRUE   use hb
*		FALSE  not
*        
* Author:  zhuyaliang  14/07/2019
******************************************************************************/
gboolean WhetherUseHeadbar (void)
{
    GSettings *settings;
    const char *key = "gtk-dialogs-use-header";

    settings = g_settings_new (GSM_GSETTINGS_SCHEMA);
    return g_settings_get_boolean (settings, key);
}    
/******************************************************************************
* Function:            MessageReport
*        
* Explain: Prompt information dialog
*          
* Input:  @Title           Message title
*         @Msg             Message content           
*         @nType           Message type
* Output: 
*        
* Author:  zhuyaliang  25/05/2018
******************************************************************************/
int MessageReport(const char *Title,const char *Msg,int nType)
{
    GtkWidget *dialog = NULL;
    int nRet;

    switch(nType)
    {
        case ERROR:
        {
            dialog = gtk_message_dialog_new(NULL,
                                            GTK_DIALOG_DESTROY_WITH_PARENT,
                                            GTK_MESSAGE_ERROR,
                                            GTK_BUTTONS_OK,
                                            "%s",Title);
            break;
        }
        case WARING:
        {
            dialog = gtk_message_dialog_new(NULL,
                                            GTK_DIALOG_DESTROY_WITH_PARENT,
                                            GTK_MESSAGE_WARNING,
                                            GTK_BUTTONS_OK,
                                            "%s",Title);
            break;
        }
        case INFOR:
        {
            dialog = gtk_message_dialog_new(NULL,
                                            GTK_DIALOG_DESTROY_WITH_PARENT,
                                            GTK_MESSAGE_INFO,
                                            GTK_BUTTONS_OK,
                                            "%s",Title);
            break;
        }
        case QUESTION:
        {
            dialog = gtk_message_dialog_new(NULL,
                                            GTK_DIALOG_DESTROY_WITH_PARENT,
                                            GTK_MESSAGE_QUESTION,
                                            GTK_BUTTONS_YES_NO,
                                            "%s",Title);
            gtk_dialog_add_button (GTK_DIALOG (dialog),_("_Return"),
                                   GTK_RESPONSE_ACCEPT);
            break;
        }
        default :
            break;

    }
    gtk_message_dialog_format_secondary_markup(GTK_MESSAGE_DIALOG(dialog),
                                               TYPEMSG,
                                               Msg);
    gtk_window_set_title(GTK_WINDOW(dialog),_("Message"));
    nRet =  gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    return nRet;
}

/******************************************************************************
* Function:            SetLableFontType
*        
* Explain: Tips the style of the text
*          
* Input:  @GtkWidget       lable
*         @Color           
*         @FontSize
*         @Word            text
* Output: 
*        
* Author:  zhuyaliang  25/05/2018
******************************************************************************/
void SetLableFontType(GtkWidget  *Lable ,
                      const char *Color,
                      int         FontSzie,
                      const char *Word,
                      gboolean    Blod)        
{
    char *LableTypeBuf;
    
    if(Blod)
    {    
        LableTypeBuf = g_strdup_printf ("<span foreground=\'%s\'weight=\'light\'font_desc=\'%d\'><b>%s</b></span>",
                         Color,
                         FontSzie,
                         Word);
    }
    else
    {
        LableTypeBuf = g_strdup_printf("<span foreground=\'%s\'weight=\'light\'font_desc=\'%d\'>%s</span>",
                        Color,
                        FontSzie,
                        Word);
    
    } 
    gtk_label_set_markup(GTK_LABEL(Lable),LableTypeBuf);
    g_free(LableTypeBuf);
}        

GtkWidget *SetButtonIcon(const gchar *button_text,const gchar *icon_name)
{
    GtkWidget *button;
    GtkWidget *image;
    GtkStyleContext *Context;

    button = gtk_button_new_with_mnemonic (button_text);
    image  = gtk_image_new_from_icon_name (icon_name, GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image (GTK_BUTTON (button), image);
    gtk_button_set_use_underline (GTK_BUTTON (button), TRUE);
    Context = gtk_widget_get_style_context (button);
    gtk_style_context_add_class (Context, "text-button");
    gtk_widget_set_can_default (button, TRUE);

    return button;
}    
void SwitchPage (SoftAppStore *app)
{
	switch (app->page)
	{
		case MAIN_PAGE:
			gtk_stack_set_visible_child_name (GTK_STACK (app->StoreStack), "main-page");
		break;
		case CATEGORY_LIST_PAGE:
			gtk_stack_set_visible_child_name (GTK_STACK (app->StoreStack), "list-page");
		break;
		case INDIVIDUAL_SOFT_PAGE:
			gtk_stack_set_visible_child_name (GTK_STACK (app->StoreStack), "details-page");
		break;
		case SEARCH_SOFT_PAGE:
			gtk_stack_set_visible_child_name (GTK_STACK (app->StoreStack), "search-page");
		break;
		default:
			gtk_stack_set_visible_child_name (GTK_STACK (app->StoreStack), "main-page");
	}
}    
static void
soft_app_image_set_from_pixbuf_with_scale (GtkImage *image, 
                                           const GdkPixbuf *pixbuf, 
                                           int       scale)
{
    cairo_surface_t *surface;
    surface = gdk_cairo_surface_create_from_pixbuf (pixbuf, scale, NULL);
    if (surface == NULL)
        return;
    gtk_image_set_from_surface (image, surface);
    cairo_surface_destroy (surface);
}

void
soft_app_image_set_from_pixbuf (GtkImage        *image, 
                                const GdkPixbuf *pixbuf,
                                int              size)
{
    gint scale;
    scale = gdk_pixbuf_get_width (pixbuf) / size;
    soft_app_image_set_from_pixbuf_with_scale (image, pixbuf, scale);
}

static void
remove_all_cb (GtkWidget *widget, gpointer user_data)
{
    GtkContainer *container = GTK_CONTAINER (user_data);
    gtk_container_remove (container, widget);
}

void
soft_app_container_remove_all (GtkContainer *container)
{
    gtk_container_foreach (container, remove_all_cb, container);
}

void CreateCacheDir (const char *dirname)
{
	const char *home;
	char       *dname;

	home = getenv("HOME");
    dname = g_strconcat(home,"/.soft-app-store/",dirname,NULL);
    if(access(dname,F_OK) != 0)
    {
        mkdir(dname,0755);
    }    
	    
    g_free(dname);
}

char *CreateCacheFile(const char *dirname,const char *cname)
{
    char *home,*fname;
    int   fd = -1;

	home = getenv("HOME");
    fname = g_strconcat(home,"/.soft-app-store/",dirname,"/",cname,NULL);
    fd = open(fname,O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);
	if(fd > 0)
	{
		close(fd);
	}
	return fname;
}
int OpenCacheFile(const char *dirname,const char *cname)
{
    char *home,*fname;
    int   fd = -1;

	home = getenv("HOME");
    fname = g_strconcat(home,"/.soft-app-store/",dirname,"/",cname,NULL);
    fd = open(fname,O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);

	g_free(fname);
	return fd;
}		
gboolean CacheFileIsEmpty (GFile *file)
{
    g_autoptr(GFileInfo) info = NULL;

	info = g_file_query_info (file,
                              G_FILE_ATTRIBUTE_STANDARD_SIZE,
                              G_FILE_QUERY_INFO_NONE,
                              NULL,
                              NULL);
	if (info == NULL)
		return FALSE;

	if(g_file_info_get_size(info) <= 0)
		return FALSE;

	return TRUE;
}

gboolean CacheFileExpiration (GFile *file)
{
	GTimeVal time_val;
    g_autoptr(GDateTime) date_time = NULL;
    g_autoptr(GFileInfo) info = NULL;
    g_autofree gchar *mod_date = NULL;
    int    t_stamp;
    time_t ct;
	
    t_stamp = time(&ct);
	info = g_file_query_info (file,
                              G_FILE_ATTRIBUTE_TIME_MODIFIED,
                              G_FILE_QUERY_INFO_NONE,
                              NULL,
                              NULL);
	if (info == NULL)
		return FALSE;

	g_file_info_get_modification_time (info, &time_val);
	date_time = g_date_time_new_from_timeval_local (&time_val);
	mod_date = g_date_time_format (date_time, "%Y%m%d%H%M%S");
	struct tm* tmp_time = (struct tm*)malloc(sizeof(struct tm));
	strptime(mod_date,"%Y%m%d%H%M%S",tmp_time); 
	time_t t = mktime(tmp_time);  
	free(tmp_time);
	if(t_stamp - t >= 604800)
		return FALSE;
	
	return TRUE;
}
