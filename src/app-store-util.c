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
#include <glib/gstdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define __USE_SOURCE
#include <time.h> 
#include "app-store-util.h"

#define  GSM_GSETTINGS_SCHEMA "org.mate.interface"
#define ERRORICON     "Error.png"
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
    char    buf[1024]; 
    int     len;

    fd = CreateLogFile();
    if(fd < 0)
    {
        return;
    }   
    if (strlen (message) > 1024)
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
	g_free (file_data);
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

uint GetCacheFileAge (GFile *file)
{
	GTimeVal time_val;
    g_autoptr(GDateTime) date_time = NULL;
    g_autoptr(GFileInfo) info = NULL;
    int    t_stamp;
    time_t ct;
    int    file_time;

    t_stamp = time(&ct);
	info = g_file_query_info (file,
                              G_FILE_ATTRIBUTE_TIME_MODIFIED,
                              G_FILE_QUERY_INFO_NONE,
                              NULL,
                              NULL);
	if (info == NULL)
		return G_MAXUINT;

	g_file_info_get_modification_time (info, &time_val);
	date_time = g_date_time_new_from_timeval_local (&time_val);
    file_time = g_date_time_to_unix (date_time);
	
	return t_stamp - file_time;
}
static gboolean
SoftAppRealrmtree (const gchar *directory, GError **error)
{
    const gchar *filename;
    g_autoptr(GDir) dir = NULL;

    dir = g_dir_open (directory, 0, error);
    if (dir == NULL) 
        return FALSE;

    while ((filename = g_dir_read_name (dir))) 
	{
        g_autofree gchar *src = NULL;
        src = g_build_filename (directory, filename, NULL);
        if (g_file_test (src, G_FILE_TEST_IS_DIR) && 
            !g_file_test (src, G_FILE_TEST_IS_SYMLINK)) 
		{
            if (!SoftAppRealrmtree (src, error))
                return FALSE;
        } 
		else 
		{
            if (g_unlink (src) != 0) 
			{
                return FALSE;
            }
        }
    }

    if (g_rmdir (directory) != 0) 
	{
        return FALSE;
    }
    return TRUE;
}

gboolean
SoftApprmtree (const gchar *directory, GError **error)
{
    SoftAppStoreLog ("Debug","recursively removing directory '%s'", directory);
    return SoftAppRealrmtree (directory, error);
}
GPtrArray *GetJsonCategory(const char *data)
{
    json_object *js,*jvalue;
    int i,len;
    GPtrArray *array;

    if(data == NULL)
    {
        SoftAppStoreLog ("Warning","GetJsonCategory data is NULL");
        return NULL;
    }    
    js = json_tokener_parse(data);
    if (is_error (js))
    {
        SoftAppStoreLog ("Warning","GetJsonCategory data in wrong format");
        return NULL;
    }
    array = g_ptr_array_new ();
    len = json_object_array_length (js);
    for (i = 0; i < len; i++)
    {
        jvalue = json_object_array_get_idx(js, i);
        g_ptr_array_add (array,jvalue);
    }
    return array;
}  
static json_object *json_parse_array(json_object *js, const char *SpecifiedData) 
{
    enum json_type type;
    int arraylen,i;
    json_object *j;
   
    arraylen = json_object_array_length(js); /*Getting the length of the array*/
    for (i = 0; i < arraylen; i++)
    {
        j = json_object_array_get_idx(js, i); /*Getting the array element at position i*/
        type = json_object_get_type(j);
        if (type == json_type_array)
        {
            json_parse_array(j, SpecifiedData);
        }
        else if(type != json_type_object)
        {
            return j;
        }
        else
        {
            GetJsonSpecifiedData(j,SpecifiedData);
        }
   }
    
    return NULL;
}
static const char *JsonValue (enum json_type type,json_object *js)
{
    switch (type)
    {
        case json_type_boolean:
            return g_strdup_printf ("%d",json_object_get_boolean(js));    
        case json_type_double:
            return g_strdup_printf ("%f",json_object_get_double(js));
        case json_type_int:
            return g_strdup_printf ("%d",json_object_get_int(js));
        case json_type_string:
            return json_object_get_string(js);
        case json_type_null:
            return NULL;
        default:
            return NULL;
    }           
}    
const char *GetJsonSpecifiedData (json_object *js,const char *SpecifiedData)
{
    enum json_type type;
    json_object *j,*ja;

    if (js == NULL)
    {
        SoftAppStoreLog ("Warning","GetJsonSpecifiedData %s is NULL",SpecifiedData);
        return NULL;
    }    
    json_object_object_foreach(js, key, value)  /*Passing through every array element*/
    {
        type = json_object_get_type(value);
        if (type == json_type_array)
        {
            ja = json_parse_array(value, SpecifiedData);
            if (g_strcmp0(key,SpecifiedData) != 0)
            {
                continue;
            }    
            type = json_object_get_type (ja);
            return JsonValue (type,ja);
        }
        else if (type == json_type_object)
        {
            j = json_object_object_get(js, key);
            GetJsonSpecifiedData (j,SpecifiedData); 
        }    
        if (g_strcmp0(key,SpecifiedData) != 0)
        {
            continue;
        }   
        return JsonValue(type,value);

    }
    return NULL;
}    
GPtrArray *GetJsonSubArrayType (const char *data)
{
    json_object *js,*jvalue;
    enum json_type type;
    GPtrArray *array;
    int i,len;

    if(data == NULL)
    {
        SoftAppStoreLog ("Warning","GetJsonSubCategory data is NULL");
        return NULL;
    }    
    js = json_tokener_parse(data);
    if (is_error (js))
    {
        SoftAppStoreLog ("Warning","GetJsonSubCategory data in wrong format");
        return NULL;
    }
    array = g_ptr_array_new ();
    json_object_object_foreach(js, key, value)  /*Passing through every array element*/
    {
        type = json_object_get_type(value);
        if (type == json_type_array)
        {
            len = json_object_array_length (value);
            for (i = 0; i < len; i++)
            {
                jvalue = json_object_array_get_idx(value, i);
                g_ptr_array_add (array,jvalue);
            }
        }
    }
    return array;
}    
static void SetErrorIcon (GtkImage *image)
{    
    g_autoptr(GdkPixbuf) pixbuf = NULL;
    
    pixbuf = gdk_pixbuf_new_from_file(ICONDIR ERRORICON,NULL);
    soft_app_image_set_from_pixbuf(GTK_IMAGE(image),pixbuf,64);
    gtk_widget_show (GTK_WIDGET (image));
}
void
SoupGetSoftIcon (SoupSession *session,
                 SoupMessage *msg,
                 gpointer     data)
{
    GtkImage *image = GTK_IMAGE (data);
    g_autoptr(GInputStream) stream = NULL;
    g_autoptr(GdkPixbuf) pixbuf = NULL;
    
    if (msg->status_code == SOUP_STATUS_NOT_MODIFIED)
    {
        SoftAppStoreLog ("Warning","get icon fail");
        SetErrorIcon (image);
        return;
    }
    if (msg->status_code != SOUP_STATUS_OK)
    {
        SoftAppStoreLog ("Warning","get icon fail"
                         "status code '%u': %s",
                          msg->status_code,
                          msg->reason_phrase);
        SetErrorIcon (image);
        return;
    }
    stream = g_memory_input_stream_new_from_data (msg->response_body->data,
                                                  msg->response_body->length,
                                                  NULL);
    if (stream == NULL)
    {
        SetErrorIcon (image);
        return;
    }    
    pixbuf = gdk_pixbuf_new_from_stream (stream, NULL, NULL);
    if (pixbuf == NULL) 
    {
        SetErrorIcon (image);
        return;
    }
    soft_app_image_set_from_pixbuf(GTK_IMAGE(image),pixbuf,64);
    gtk_widget_show (GTK_WIDGET (image));
}
void InitStorePkCtx (SoftAppStore *app)
{
    gboolean ret;
    g_autoptr(GError) error = NULL;

    app->Ctx = g_new0 (PkCtx, 1);
    app->Ctx->cancellable = g_cancellable_new ();
    app->Ctx->control = pk_control_new ();
    ret = pk_control_get_properties (app->Ctx->control, app->Ctx->cancellable, &error);
    if (!ret) 
    {
        SoftAppStoreLog ("Error","%s: %s\n",
                       _("Failed to contact PackageKit"), 
                         error->message);
        app->Ctx->retval = PK_EXIT_CODE_CANNOT_SETUP;
        return ;
    }
    app->Ctx->task = pk_task_new ();
}   
static gchar *
pk_get_resolve_package (PkCtx *ctx, const gchar *package_name, GError **error)
{
    gchar *package_id = NULL;
    gboolean valid;
    PkPackage *package;
    g_autoptr(GPtrArray) array = NULL;
    g_autoptr(PkError) error_code = NULL;
    g_autoptr(PkResults) results = NULL;
    g_auto(GStrv) tmp = NULL;

    valid = pk_package_id_check (package_name);
    if (valid)
        return g_strdup (package_name);

    tmp = g_strsplit (package_name, ",", -1);
    results = pk_client_resolve (PK_CLIENT (ctx->task),
                                 ctx->filters, tmp,
                                 ctx->cancellable,
                                 NULL,NULL,
                                 error);
    if (results == NULL)
        return NULL;

    error_code = pk_results_get_error_code (results);
    if (error_code != NULL) 
    {
        SoftAppStoreLog ("Warning","get pakcge ids %s fails %d %s",
                          package_name,
                          pk_error_get_code (error_code),
                          pk_error_get_details (error_code));
        return NULL;
    }

    /* nothing found */
    array = pk_results_get_package_array (results);
    if (array->len == 0) 
    {
        SoftAppStoreLog ("Warning","could not find %s", package_name);
        return NULL;
    }

    package = g_ptr_array_index (array, 0);
    g_object_get (package,
                 "package-id", &package_id,
                  NULL);
    return g_strdup (pk_package_get_id (package));
}

gchar **PackageNameToPackageids (const char *pname,SoftAppStore *app)
{
    gchar *package_id;
    GError *error_local = NULL;
    g_autoptr(GPtrArray) array = NULL;

    array = g_ptr_array_new ();
    package_id = pk_get_resolve_package (app->Ctx,
                                         pname,
                                        &error_local);
    if (package_id == NULL) 
    {
        SoftAppStoreLog ("Warning","%s: %s", 
                         "Package not found",
                          pname);
        return NULL;
    }
    g_ptr_array_add (array, package_id);

    if (array->len == 0) 
    {
        SoftAppStoreLog ("Warning","%s: %s", 
                         "No packages were found",
                          pname);
        return NULL;
    }

    g_ptr_array_add (array, NULL);
    return g_strdupv ((gchar **) array->pdata);
}
gboolean DetermineStoreSoftInstalled (const char *SoftName)
{
    g_autoptr(GPtrArray) parent_appdata = g_ptr_array_new_with_free_func (g_free);
    g_autoptr(GDir)   dir;
    g_autoptr(GError) error = NULL;
	AsApp *app;
	const char *name;
    const char *fn;

    g_ptr_array_add (parent_appdata,
                     g_build_filename ("/usr/share", "appdata", NULL));
    g_ptr_array_add (parent_appdata,
                     g_build_filename ("/usr/share", "metainfo", NULL));

    
    for (guint i = 0; i < parent_appdata->len; i++) 
    {
        const gchar *fn1 = g_ptr_array_index (parent_appdata, i);
		dir = g_dir_open (fn1, 0, &error);
		if (dir == NULL)
		{
			return FALSE;
		}
		while ((fn = g_dir_read_name (dir)) != NULL) 
		{
			if (g_str_has_suffix (fn, ".appdata.xml") ||
				g_str_has_suffix (fn, ".metainfo.xml")) 
			{
				g_autofree gchar *filename = g_build_filename (fn1, fn, NULL);
				app = as_app_new();
				as_app_parse_file(app,
								  filename,
								  AS_APP_PARSE_FLAG_USE_HEURISTICS,
								  NULL);
				name = as_app_get_name (app,"en_GB");
				if (g_strcmp0 (SoftName,name) == 0)
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
