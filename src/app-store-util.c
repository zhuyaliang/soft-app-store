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
void SetLableFontType(GtkWidget *Lable ,const char *Color,int FontSzie,const char *Word)        
{
    char LableTypeBuf[200] = { 0 };
    
    sprintf(LableTypeBuf,
           "<span foreground=\'%s\'weight=\'light\'font_desc=\'%d\'><b>%s</b></span>",
            Color,FontSzie,Word);
    gtk_label_set_markup(GTK_LABEL(Lable),LableTypeBuf);

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
