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
#include "app-store-details.h"
#include "app-store-pkgkit.h"


static char *GetMetadataFileName(char *path)
{
    g_auto(GStrv)   split = NULL;
    
	split = g_strsplit(path,"/",-1);
	
	return g_strdup(split[4]);
}
static void UpdateLocalInstallPage(SoftAppStore *app)
{
	SoftAppMessage *Message;
	GtkWidget  *row;
	guint       i;
	app->page = MAIN_PAGE;
	
	SoftAppStoreLog ("Debug","start load all local soft %u to listrowbox",app->pkg->list->len);
    for (i = 0; i < app->pkg->list->len; i++)
    {
        Message = SOFT_APP_MESSAGE (g_ptr_array_index (app->pkg->list, i));
		row = soft_app_row_new(Message);
		gtk_widget_set_halign(row, GTK_ALIGN_CENTER);
		gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(row),TRUE);
		gtk_list_box_insert (GTK_LIST_BOX(app->LocalSoftListBox), row, i);
		gtk_widget_show_all(row);
	}
	//gtk_widget_show(app->LocalSoftListBox);
	app->pkg->action = GPK_ACTION_DONE;
	SoftAppStoreLog ("Debug","load all local soft %u to listrowboxi Successfu",i);
}
static char *GetCacheFileIcon(char *dname)
{
	GKeyFile         *kconfig;
    g_autofree gchar *cache_file = NULL;
	char             *icon;
    
	kconfig = g_key_file_new();
	cache_file = CreateCacheFile(dname,"soft_icon");
	g_key_file_load_from_file(kconfig, cache_file, G_KEY_FILE_NONE,NULL);

	icon = g_key_file_get_string(kconfig,"soft-app-store","icon",NULL);
	g_key_file_free(kconfig);
	
	return g_strdup(icon);
}
static void
GetLocalSoftAppDetails (PkClient       *client, 
                        GAsyncResult   *res, 
                        SoftAppPkgkit  *pkg)
{
    g_autoptr(PkResults) results = NULL;
    g_autoptr(GError)    error = NULL;
    g_autoptr(PkError)   error_code = NULL;
    g_autoptr(GPtrArray) array = NULL;
    PkDetails        *item;
    guint64           size;
    char             *xml;
    g_autofree gchar *dname = NULL;
	g_autofree gchar *icon = NULL;
    g_autofree gchar *license = NULL;
    g_autofree gchar *package_id = NULL;
    g_autofree gchar *package = NULL;
    g_autofree gchar *url = NULL;
    g_autofree gchar *s_size = NULL;
    g_auto(GStrv)     split = NULL;
	SoftAppMessage   *Message;
    g_autofree gchar *cache_file = NULL;
    uint              soft_sum = 0;
	GKeyFile         *kconfig;

    results = pk_client_generic_finish (client, res, &error);
    if (results == NULL) 
    {
        SoftAppStoreLog("Warning","get package details faild: %s", error->message);
        return;
    }
    error_code = pk_results_get_error_code (results);
    if (error_code != NULL) 
    {
        SoftAppStoreLog("Warning","failed to get details: %s, %s", 
                         pk_error_enum_to_string (pk_error_get_code (error_code)), 
                         pk_error_get_details (error_code));
        return;
    }
   
    array = pk_results_get_details_array (results);
    if (array->len != 1) 
    {
        SoftAppStoreLog("Warning","get package details not one entry %u", array->len);
        return;
    }
    item = g_ptr_array_index (array, 0);
    g_object_get (item,
                 "package-id",&package_id,
                 "url",       &url,
                 "license",   &license,
                 "size",      &size,
                  NULL);
    split = pk_package_id_split (package_id);
	
	/*get metadata file path eg /usr/share/metainfo/yelp.appdata.xml */
	xml = g_hash_table_lookup(pkg->phash,package_id);
	/*get metadata file name eg yelp.appdata.xml */
	dname = GetMetadataFileName(xml);
	/*get cache data path eg /home/user/.soft-app-store/yelp.appdata.xml/soft_msg */
	cache_file = CreateCacheFile(dname,"soft_msg");
    
	kconfig = g_key_file_new();
	Message = soft_app_message_new ();

	soft_app_message_set_cache (Message,dname);
    as_app_parse_file(AS_APP(Message),
                      xml,
                      AS_APP_PARSE_FLAG_USE_HEURISTICS,
                      NULL);

	icon = GetCacheFileIcon(dname);
	if(icon == NULL)
	{
		soft_app_message_set_icon(Message,split[PK_PACKAGE_ID_NAME]);
		g_key_file_set_string(kconfig,
							 "soft-app-store",
						     "icon",
							  split[PK_PACKAGE_ID_NAME]);

	}
	else
	{
		soft_app_message_set_icon(Message,icon);
		g_key_file_set_string(kconfig,
				             "soft-app-store",
							 "icon",
							  icon);
	}
    soft_app_message_set_pkgid(Message,package_id);
	g_key_file_set_string(kconfig,
			             "soft-app-store",
						 "package_id",
						  package_id);

	soft_app_message_set_version(Message,split[PK_PACKAGE_ID_VERSION]);
	soft_app_message_set_arch(Message,split[PK_PACKAGE_ID_ARCH]);
	soft_app_message_set_score(Message,0.5);
	g_key_file_set_double(kconfig,
			             "soft-app-store",
						 "score",
						  0.5);

    s_size = g_strdup_printf ("%.2f MB",(float)size/(1024*1024));
	soft_app_message_set_size(Message,s_size);
	g_key_file_set_string(kconfig,
			             "soft-app-store",
						 "size",
						  s_size);

	soft_app_message_set_url(Message,url);
	g_key_file_set_string(kconfig,
			             "soft-app-store",
						 "url",
						  url);

	soft_app_message_set_license(Message,license);
	g_key_file_set_string(kconfig,
			             "soft-app-store",
						 "license",
						  license);

    package = pk_package_id_to_printable (package_id);
	soft_app_message_set_package(Message,package);
	g_key_file_set_string(kconfig,
			             "soft-app-store",
						 "package",
						  package);
	
	g_key_file_save_to_file(kconfig,cache_file,NULL);
	SoftAppStoreLog ("Debug","wriet cache %s Successfu",cache_file);
	g_key_file_free(kconfig);

	pkg->cache_cnt +=1;
	soft_sum = pkg->metadata_cnt + pkg->cache_cnt;

    g_ptr_array_add (pkg->list, Message);
    if(soft_sum >= pkg->phashlen)
    {
		SoftAppStoreLog ("Debug","not caceh emmit signal package count %u",pkg->metadata_cnt);
        emit_details_complete(pkg);
        soft_sum = 0;
    }

}    
static void
GetLocalDetailsProgress(PkProgress    *progress, 
                        PkProgressType type, 
                        SoftAppStore  *app)
{
    PkStatusEnum status;
    gint percentage;
    gboolean allow_cancel;
    uint soft_sum;

    SoftAppPkgkit *pkg = app->pkg;

	soft_sum = pkg->metadata_cnt + pkg->cache_cnt;
    g_object_get (progress,
                 "status", &status,
                 "percentage", &percentage,
                 "allow-cancel", &allow_cancel,
                  NULL);

    if (type == PK_PROGRESS_TYPE_STATUS)
    {
        if (status == PK_STATUS_ENUM_FINISHED)
        {    
            if(soft_sum <= pkg->phashlen)
            {
                gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(app->LocalSoftBar),
						                      (gfloat) soft_sum /pkg->phashlen);
				gtk_progress_bar_set_text(GTK_PROGRESS_BAR(app->LocalSoftBar),NULL);
            }
			else 
				soft_sum = 0;
        }
    } 
    else if (type == PK_PROGRESS_TYPE_PERCENTAGE) 
    {
        if (percentage > 0) 
        {
            //g_print("percentage = %.2f\r\n",(float) percentage / 100.0f);
        } 
    } 
}
static void SetSoftIconMsg(const char *dirname,const char *desktop_name)
{
    FILE *s_fp = NULL;
    char *fname;
    char  ReadBuf[1024] = { 0 };
	GKeyFile         *kconfig;

    s_fp = fopen(desktop_name,"r");
    if(s_fp == NULL)
    {
        return;
    }   
    kconfig = g_key_file_new();
	fname = CreateCacheFile(dirname,"soft_icon");
    while((fgets(ReadBuf,1024,s_fp)) != NULL)
    {
        if(g_strrstr(ReadBuf,"Icon=") != NULL)
        {
			ReadBuf[strlen(ReadBuf)-1] = '\0';
			g_key_file_set_string(kconfig,
			                     "soft-app-store",
						         "icon",
								 &ReadBuf[5]);
            break;
        }    
    }   
	g_key_file_save_to_file(kconfig,fname,NULL);
	g_key_file_free(kconfig);
    fclose(s_fp);
    g_free(fname);

}
static void
GetLocalSoftFilesProgress(PkProgress    *progress, 
                          PkProgressType type, 
                          SoftAppStore  *app)
{
    
}    
static void
GetLocalSoftFilesFinished (PkClient       *client, 
                           GAsyncResult   *res, 
                           SoftAppPkgkit  *pkg)
{
    g_autoptr(GError)    error = NULL;
    g_autoptr(GPtrArray) array = NULL;
    g_autoptr(PkError)   error_code = NULL;
	g_autofree gchar    *dname = NULL;
    PkFiles *item;
    char   **files = NULL;
    const char    *package_id;
    char          *xml;
    int      i = 0;
    int      fd;
    g_autoptr(PkResults) results = NULL;

    /* get the results */
    results = pk_client_generic_finish (client, res, &error);
    if (results == NULL) 
    {
        SoftAppStoreLog ("Error","failed to get files: %s", error->message);
        return;
    }
    /* check error code */
    error_code = pk_results_get_error_code (results);
    if (error_code != NULL) 
    {
        SoftAppStoreLog ("Error","failed to get files: %s, %s", 
                          pk_error_enum_to_string (pk_error_get_code (error_code)), 
                          pk_error_get_details (error_code));

        return;
    }

    /* get data */
    array = pk_results_get_files_array (results);
    if (array->len != 1)
        return;

    /* assume only one option */
    item = g_ptr_array_index (array, 0);
    files = pk_files_get_files (item);
    package_id = pk_files_get_package_id(item); 
	xml = g_hash_table_lookup(pkg->phash,package_id);
	dname = GetMetadataFileName(xml);
    fd = OpenCacheFile(dname,"files");
    
	while(files[i] != NULL)
    {
        write(fd,files[i],strlen(files[i]));
        write(fd,"\r\n",2);
        i++;
    }

    i = 0;
    while(files[i] != NULL)
    {
        if (g_strrstr(files[i],".desktop") != NULL)
        {
            SetSoftIconMsg(dname,files[i]); 
            break;
        }    
        i++;
    } 	
    close(fd);
}
static void 
soft_app_get_package_details_use_cache (char *package_id, SoftAppStore *app)
{
    char             *xml;
    g_autofree gchar *license = NULL;
    g_autofree gchar *package = NULL;
    g_autofree gchar *icon = NULL;
    g_autofree gchar *url = NULL;
    g_autofree gchar *s_size = NULL;
    g_autofree gchar *dname = NULL;
    g_auto(GStrv)     split = NULL;
	SoftAppMessage   *Message;
    g_autofree gchar *cache_file = NULL;
    uint              soft_sum = 0;
	GKeyFile         *kconfig;

    split = pk_package_id_split (package_id);
	
	xml = g_hash_table_lookup(app->pkg->phash,package_id);
	if(xml == NULL)
	{
		return;
	}
	Message = soft_app_message_new ();

    as_app_parse_file(AS_APP(Message),
                      xml,
                      AS_APP_PARSE_FLAG_USE_HEURISTICS,
                      NULL);
	dname = GetMetadataFileName(xml);
	cache_file = CreateCacheFile(dname,"soft_msg");
    kconfig = g_key_file_new();
	g_key_file_load_from_file(kconfig, cache_file, G_KEY_FILE_NONE, NULL);

	soft_app_message_set_cache (Message,dname);
    soft_app_message_set_pkgid(Message,package_id);
	soft_app_message_set_version(Message,split[PK_PACKAGE_ID_VERSION]);
	soft_app_message_set_arch(Message,split[PK_PACKAGE_ID_ARCH]);
	soft_app_message_set_score(Message,0.5);

	icon = g_key_file_get_string(kconfig,"soft-app-store","icon",NULL);
    soft_app_message_set_icon(Message,icon);

	s_size =  g_key_file_get_string(kconfig,"soft-app-store","size",NULL);
	soft_app_message_set_size(Message,s_size);
	
	url = g_key_file_get_string(kconfig,"soft-app-store","url",NULL);
	soft_app_message_set_url(Message,url);
	
	license = g_key_file_get_string(kconfig,"soft-app-store","licenses",NULL);
	soft_app_message_set_license(Message,license);
	
	package = g_key_file_get_string(kconfig,"soft-app-store","package",NULL);
	soft_app_message_set_package(Message,package);
	g_key_file_free(kconfig);

    g_ptr_array_add (app->pkg->list, Message);
	soft_sum = app->pkg->cache_cnt + app->pkg->metadata_cnt;
    if(soft_sum >= app->pkg->phashlen)
    {
		SoftAppStoreLog ("Debug","use caceh emmit signal package count %u",app->pkg->cache_cnt);
        emit_details_complete(app->pkg);
        soft_sum = 0;
    }
}
static void
soft_app_get_package_details (char         *package_id, 
		                      char         *dname,
							  SoftAppStore *app)
{
    g_auto(GStrv)   package_ids = NULL;
    
	SoftAppPkgkit *pkg = app->pkg;

	/* Create Cache dir eg /home/user/.soft-app-store/mate-desktop.metainfo.xml */
	CreateCacheDir(dname);
	SoftAppStoreLog ("Debug","Cache Directory %s Creation Successfu",dname);	
	/*Get detailed information about the software*/
    package_ids = pk_package_ids_from_id (package_id);
    pk_client_get_details_async (PK_CLIENT(pkg->task), 
                                 package_ids, 
                                 pkg->cancellable,
                                (PkProgressCallback) GetLocalDetailsProgress,
                                 app,
                                (GAsyncReadyCallback) GetLocalSoftAppDetails,
                                 pkg);
    pk_client_get_files_async (PK_CLIENT (pkg->task), 
                               package_ids, 
                               pkg->cancellable,
                              (PkProgressCallback)  GetLocalSoftFilesProgress, 
                               app,
                              (GAsyncReadyCallback) GetLocalSoftFilesFinished, 
                               pkg);
}

static void get_local_soft_details_ready (SoftAppPkgkit *pkg,
                                          SoftAppStore  *app)
{
	SoftAppStoreLog ("Debug","get_local_soft_details_ready all package count %u",app->pkg->list->len);
    UpdateLocalInstallPage(app);
	gtk_widget_hide(app->LocalSoftBar);
	gtk_widget_hide(app->LocalSoftLabel);
	gtk_spinner_stop (GTK_SPINNER (app->LocalSoftSpinner));
	gtk_widget_hide(app->LocalSoftSpinner);
}

static gchar *
soft_app_file_get_packageid (SoftAppPkgkit *pkg, const gchar *file_name)
{
    g_autofree gchar *package_id = NULL;
    PkPackage *package;
    g_autoptr(GPtrArray) array = NULL;
    g_autoptr(PkError) error_code = NULL;
    g_autoptr(PkResults) results = NULL;
    g_auto(GStrv) tmp = NULL;

    tmp = g_strsplit (file_name, ",", -1);
    results = pk_client_search_files (PK_CLIENT (pkg->task),
                                 4,
                                 tmp,
                                 pkg->cancellable,
                                 NULL, 
                                 NULL,
                                 NULL);
    if (results == NULL)
        return NULL;
    /* check error code */
    error_code = pk_results_get_error_code (results);
    if (error_code != NULL) 
	{
        return NULL;
    }
    /* nothing found */
    array = pk_results_get_package_array (results);
    if (array->len == 0) 
	{
        return NULL;
    }

    package = g_ptr_array_index (array, 0);
    g_object_get (package,
                 "package-id", &package_id,
                  NULL);

    return g_strdup (pk_package_get_id (package));
}
static char *soft_app_file_get_packageid_use_cache(const char *dname)
{
	GKeyFile         *kconfig;
    g_autofree gchar *cache_file = NULL;
	g_autofree gchar *package_id = NULL;
    
	kconfig = g_key_file_new();
	cache_file = CreateCacheFile(dname,"soft_msg");
	g_key_file_load_from_file(kconfig, cache_file, G_KEY_FILE_NONE,NULL);
	package_id = g_key_file_get_string(kconfig,"soft-app-store","package_id",NULL);
	g_key_file_free(kconfig);
	
	return g_strdup(package_id);
}
static gboolean HavingCache(const char *dname)
{
    const char       *home; 
	g_autofree gchar *cache_file = NULL;

	home = getenv("HOME");
	cache_file = g_build_filename (home,".soft-app-store",dname,"soft_msg", NULL);
	if (g_file_test (cache_file, G_FILE_TEST_EXISTS)) 
	{
        g_autoptr(GFile) file = g_file_new_for_path (cache_file);
		if(!CacheFileIsEmpty(file))
        {    
			return FALSE;
        }    
		if(GetCacheFileAge(file) >= 604800)
        {    
			return FALSE;
        }    
		return TRUE;
	}

	return FALSE;
}
static gboolean soft_app_load_appdata(SoftAppStore *app,const char *path)
{
    const gchar *fn;
    g_autoptr(GError) error = NULL;
    g_autoptr(GDir)   dir;
    char        *package_id;
	static double i = 0.0;

    dir = g_dir_open (path, 0, &error);
    if (dir == NULL)
	{
		MessageReport(_("open metadata"),error->message,ERROR);	
		SoftAppStoreLog("Error","open metadata error %s",error->message);
        return FALSE;
	}
    while ((fn = g_dir_read_name (dir)) != NULL) 
    {
        if (g_str_has_suffix (fn, ".appdata.xml") ||
            g_str_has_suffix (fn, ".metainfo.xml")) 
        {
            g_autofree gchar *filename = g_build_filename (path, fn, NULL);
            if(HavingCache(fn))
			{
				package_id = soft_app_file_get_packageid_use_cache(fn);
			}
			else
			{
				package_id = soft_app_file_get_packageid (app->pkg,filename);
			}	
			if(package_id == NULL)
                continue;
            g_hash_table_insert(app->pkg->phash,package_id,g_strdup(filename));
			gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(app->LocalSoftBar),
						                                  (i += 0.01));
        }
    }
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(app->LocalSoftBar),1);
    return TRUE;
}  
static void list_hash_table(gpointer key,gpointer value,gpointer data)
{
    SoftAppStore *app = (SoftAppStore *)data;
	g_autofree gchar *dname = NULL;

	dname = GetMetadataFileName((char *)value);
	app->pkg->action = GPK_ACTION_DETAILS;
	if(!HavingCache(dname))
	{
		SoftAppStoreLog ("Debug","metadata not cache %s",dname);
		soft_app_get_package_details ((char *)key,dname,app);
	}
	else
	{
		SoftAppStoreLog ("Debug","metadata use cache %s",dname);
		app->pkg->metadata_cnt +=1;
		soft_app_get_package_details_use_cache((char *)key,app);
	}
}    
static gpointer ParseLocalSoft (SoftAppStore *app)
{
    g_autoptr(GPtrArray) parent_appdata = g_ptr_array_new_with_free_func (g_free);

    g_ptr_array_add (parent_appdata,
                     g_build_filename ("/usr/share", "appdata", NULL));
    g_ptr_array_add (parent_appdata,
                     g_build_filename ("/usr/share", "metainfo", NULL));

    
    for (guint i = 0; i < parent_appdata->len; i++) 
    {
        const gchar *fn = g_ptr_array_index (parent_appdata, i);
		SoftAppStoreLog("Debug","Acquiring Metadata %s",fn);
        soft_app_load_appdata (app,fn);
    }
    app->pkg->phashlen = g_hash_table_size(app->pkg->phash);
	SoftAppStoreLog ("Debug","soft app count %u",app->pkg->phashlen);
	g_hash_table_foreach(app->pkg->phash,list_hash_table,app);
    return NULL;
}    
static void GetLocalSoftMessage(SoftAppStore *app)
{
	app->pkg = g_new0 (SoftAppPkgkit, 1);
	app->pkg = soft_app_pkgkit_new();
	app->pkg->list = g_ptr_array_new ();
    app->pkg->phash = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	g_signal_connect (app->pkg,
                     "details-ready",
                      G_CALLBACK (get_local_soft_details_ready),
                      app);
    g_thread_new("GetLocalSoft",(GThreadFunc)ParseLocalSoft,app);
}
static void soft_app_remove_progress_cb (PkProgress     *progress, 
		                                 PkProgressType  type, 
								   	     SoftAppStore   *app)
{
    PkStatusEnum status;
    gint percentage;
    gboolean allow_cancel;

    g_object_get (progress,
                 "status", &status,
                 "percentage", &percentage,
                 "allow-cancel", &allow_cancel,
                  NULL);

    if (type == PK_PROGRESS_TYPE_STATUS)
    {
        if (status == PK_STATUS_ENUM_FINISHED)
		{
			g_ptr_array_remove_index (app->pkg->list,app->index);    
			gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(app->details->progressbar),
										  1.00);
			UpdateLocalInstallPage(app);
            app->page = app->parent_page--;
            SwitchPage(app);
		}
	} 
    else if (type == PK_PROGRESS_TYPE_PERCENTAGE) 
    {
        if (percentage > 0) 
        {
			g_print("percentage = %d\r\n",percentage);
			gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(app->details->progressbar),
										 (float) percentage / 100.0f);
		}
	}	
}
static void
soft_app_remove_packages_cb (PkTask *task, GAsyncResult *res, SoftAppStore *app)
{
    g_autoptr(PkResults) results = NULL;
    g_autoptr(GError)    error = NULL;
    g_autoptr(PkError)   error_code = NULL;

    /* get the results */
    results = pk_task_generic_finish (task, res, &error);
    if (results == NULL) {
        g_warning ("failed to remove packages: %s", error->message);
        return;
    }

    /* check error code */
    error_code = pk_results_get_error_code (results);
    if (error_code != NULL) 
	{
		SoftAppStoreLog ("failed to remove packages: %s, %s", 
				          pk_error_enum_to_string (pk_error_get_code (error_code)), 
						  pk_error_get_details (error_code));

        return;
    }
}
static char *GetLocalFiles(const char *dname)
{
    FILE *fp = NULL;
    char *home,*fname;
	char *files = NULL;
    int   flen;

    home = getenv("HOME");
    fname = g_strconcat(home,"/.soft-app-store/",dname,"/files",NULL);
    
    fp = fopen(fname,"r");
    fseek(fp,0L,SEEK_END);
    flen = ftell(fp);
	files = (char *)malloc(flen+1);
    memset(files,'\0',flen);
	fseek(fp,0L,SEEK_SET);
    fread(files,1,flen-2,fp);
    fclose(fp);
    g_free(fname);
    return files;
}    
static void SetDialogTextView(const char *dname,GtkDialog *dialog)
{
    GtkWidget     *scroll;
    GtkWidget     *widget;
    GtkTextBuffer *buffer;
    g_autofree char *files = NULL;
    
	/* create a text view to hold the store */
    widget = gtk_text_view_new ();
    gtk_text_view_set_editable (GTK_TEXT_VIEW (widget), FALSE);
    gtk_text_view_set_left_margin (GTK_TEXT_VIEW (widget), 5);
    gtk_text_view_set_right_margin (GTK_TEXT_VIEW (widget), 5);

    /* scroll the treeview */
    scroll = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
                    GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_container_add (GTK_CONTAINER (scroll), widget);
    gtk_widget_show (scroll);

	buffer = gtk_text_buffer_new (NULL);
    files = GetLocalFiles(dname);
    gtk_text_buffer_set_text (buffer, files, -1);
	gtk_text_view_set_buffer (GTK_TEXT_VIEW (widget), buffer);
	gtk_widget_show (widget);
    /* add some spacing to conform to the GNOME HIG */
    gtk_container_set_border_width (GTK_CONTAINER (scroll), 6);
    gtk_widget_set_size_request (GTK_WIDGET (scroll), -1, 300);

    /* add scrolled window */
    widget = gtk_dialog_get_content_area (GTK_DIALOG(dialog));
    gtk_box_pack_start (GTK_BOX (widget), scroll, TRUE, TRUE, 0);

}   

void ViewLocalSoftFiles (GtkWidget *button, SoftAppStore *app)
{
    const char *name;
	GtkWidget  *dialog;

    name  = soft_app_info_get_cache(app->details->info);
    dialog = gtk_message_dialog_new (GTK_WINDOW(app->MainWindow), 
                                     GTK_DIALOG_DESTROY_WITH_PARENT,
                                     GTK_MESSAGE_INFO, 
                                     GTK_BUTTONS_OK, 
                                     "%s", name);

    SetDialogTextView(name,GTK_DIALOG (dialog));
    gtk_window_set_resizable (GTK_WINDOW (dialog), TRUE);
    gtk_window_set_default_size (GTK_WINDOW (dialog), 600, 250);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (GTK_WIDGET (dialog));
    
}    
void RemoveLocalSoftApp (GtkWidget *button, SoftAppStore *app)
{
	g_auto(GStrv) package_ids = NULL;
    g_autofree const gchar *package_id = NULL;

	gtk_widget_hide(button);
	gtk_widget_show(app->details->progressbar);
	package_id = soft_app_info_get_pkgid(app->details->info);
    g_print("package_id = %s\r\n",package_id);
    package_ids = pk_package_ids_from_id (package_id);
    /* remove */
    pk_task_remove_packages_async (app->pkg->task, 
								   package_ids, 
								   TRUE, 
								   FALSE, 
								   app->pkg->cancellable,
								  (PkProgressCallback) soft_app_remove_progress_cb, 
								   app,
                                  (GAsyncReadyCallback) soft_app_remove_packages_cb, 
								   app);

}
GtkWidget *LoadLocalInstall(SoftAppStore *app)
{
    GtkWidget *vbox;
    GtkWidget *sw;

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
	sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
	app->LocalSoftListBox = gtk_list_box_new ();
	gtk_container_add (GTK_CONTAINER (sw), app->LocalSoftListBox);
    g_signal_connect (app->LocalSoftListBox, 
                     "row-activated",
                      G_CALLBACK (SwitchPageToIndividualDetailsPage), 
                      app);
	app->LocalSoftLabel = gtk_label_new(NULL);
	SetLableFontType(app->LocalSoftLabel,
                    "black",
                     18,
                     _("Efforts are being made to load the local software list"),
                     TRUE);
	gtk_box_pack_start (GTK_BOX (vbox), app->LocalSoftLabel, TRUE, TRUE, 0);
	app->LocalSoftSpinner = gtk_spinner_new ();
	gtk_spinner_start (GTK_SPINNER (app->LocalSoftSpinner));
	gtk_box_pack_start (GTK_BOX (vbox), app->LocalSoftSpinner, TRUE, TRUE, 0);
	app->LocalSoftBar =  gtk_progress_bar_new ();
	gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(app->LocalSoftBar),TRUE);
	g_object_set(app->LocalSoftBar,"show-text",TRUE,NULL);
	gtk_box_pack_start (GTK_BOX (vbox), app->LocalSoftBar, TRUE, TRUE, 0);
	gtk_widget_show_all(app->LocalSoftListBox);

	GetLocalSoftMessage(app);
	return vbox;
}
