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
#include "app-store-screenshot.h"

//G_DEFINE_TYPE (SoftAppScreenshot, soft_app_screenshot, GTK_TYPE_BIN)
G_DEFINE_TYPE (SoftAppScreenshot, soft_app_screenshot, GTK_TYPE_STACK)

typedef enum 
{
    UTILS_CACHE_FLAG_NONE        = 0,
    UTILS_CACHE_FLAG_WRITEABLE       = 1 << 0,
    UTILS_CACHE_FLAG_USE_HASH        = 1 << 1,
    UTILS_CACHE_FLAG_ENSURE_EMPTY    = 1 << 2,
    UTILS_CACHE_FLAG_LAST
} UtilsCacheFlags;
static gchar * filename_array_return_newest (GPtrArray *array)
{
    const gchar *filename_best = NULL;
    guint age_lowest = G_MAXUINT;
    guint i;
    for (i = 0; i < array->len; i++) 
	{
        const gchar *fn = g_ptr_array_index (array, i);
        g_autoptr(GFile) file = g_file_new_for_path (fn);
        guint age_tmp = GetCacheFileAge (file);
        if (age_tmp < age_lowest) {
            age_lowest = age_tmp;
            filename_best = fn;
        }
    }
    return g_strdup (filename_best);
}

static char *soft_app_get_screenshot_cache_filename (const gchar *kind,
													 const gchar *resource,
													 UtilsCacheFlags flags,
													 GError **error)
{
    g_autofree gchar *basename = NULL;
    g_autofree gchar *cachedir = NULL;
    g_autoptr(GFile) cachedir_file = NULL;
    g_autoptr(GPtrArray) candidates = g_ptr_array_new_with_free_func (g_free);

    /* get basename */
    if (flags & UTILS_CACHE_FLAG_USE_HASH) 
	{
        g_autofree gchar *basename_tmp = g_path_get_basename (resource);
        g_autofree gchar *hash = g_compute_checksum_for_string (G_CHECKSUM_SHA1,
                                    resource, -1);
        basename = g_strdup_printf ("%s-%s", hash, basename_tmp);
    } 
	else 
	{
        basename = g_path_get_basename (resource);
    }

    /* not writable, so try the system cache first */
    if ((flags & UTILS_CACHE_FLAG_WRITEABLE) == 0) 
	{
        g_autofree gchar *cachefn = NULL;
        cachefn = g_build_filename (LOCALSTATEDIR,
                        "cache",
                        "soft-app-store",
                         kind,
                         basename,
                         NULL);
        if (g_file_test (cachefn, G_FILE_TEST_EXISTS)) 
		{
            g_ptr_array_add (candidates,
                             g_steal_pointer (&cachefn));
        }
    }

    /* not writable, so try the system cache first */
    if ((flags & UTILS_CACHE_FLAG_WRITEABLE) == 0) 
	{
        g_autofree gchar *cachefn = NULL;
        cachefn = g_build_filename (DATADIR,
                        "soft-app-store",
                        "cache",
                        kind,
                        basename,
                        NULL);
        if (g_file_test (cachefn, G_FILE_TEST_EXISTS)) 
		{
            g_ptr_array_add (candidates,
                             g_steal_pointer (&cachefn));
        }
    }

    /* create the cachedir in a per-release location, creating
     * if it does not already exist */
    cachedir = g_build_filename (g_get_user_cache_dir (),
                     "soft-app-store",
                      kind,
                      NULL);
    cachedir_file = g_file_new_for_path (cachedir);
    if (g_file_query_exists (cachedir_file, NULL) &&
        flags & UTILS_CACHE_FLAG_ENSURE_EMPTY) 
	{
        if (!SoftApprmtree (cachedir, error))
            return NULL;
    }
    if (!g_file_query_exists (cachedir_file, NULL) &&
        !g_file_make_directory_with_parents (cachedir_file, NULL, error))
        return NULL;
    g_ptr_array_add (candidates, g_build_filename (cachedir, basename, NULL));

    /* common case: we only have one option */
    if (candidates->len == 1)
        return g_strdup (g_ptr_array_index (candidates, 0));

    /* return the newest (i.e. one with least age) */
    return filename_array_return_newest (candidates);
}

void 
soft_app_screenshot_set_size (SoftAppScreenshot *ss,
                              guint width, 
                              guint height)
{    
    g_return_if_fail (width != 0);
    g_return_if_fail (height != 0);

    ss->width = width;
    ss->height = height;
    gtk_widget_set_size_request (GTK_WIDGET(ss), (gint) width, (gint) height);
}
static gchar *
soft_app_screenshot_get_cachefn_for_url (const gchar *url)
{
    g_autofree gchar *basename = NULL;
    g_autofree gchar *checksum = NULL;
    checksum = g_compute_checksum_for_string (G_CHECKSUM_SHA256, url, -1);
    basename = g_path_get_basename (url);
    return g_strdup_printf ("%s-%s", checksum, basename);
}    
static void
soft_app_screenshot_set_error (SoftAppScreenshot *ss, const gchar *message)
{
    gint width, height;

    gtk_stack_set_visible_child_name (GTK_STACK (ss), "error");
    gtk_label_set_label (GTK_LABEL (ss->label_error), message);
    gtk_widget_get_size_request (GTK_WIDGET(ss), &width, &height);
    if (width < 200)
        gtk_widget_hide (ss->label_error);
    else
        gtk_widget_show (ss->label_error);
    gtk_widget_show_all (GTK_WIDGET(ss));
    ss->showing_image = FALSE;
}
static void
soft_app_screenshot_show_image (SoftAppScreenshot *ss)
{
    g_autoptr(GdkPixbuf) pixbuf = NULL;

    /* no need to composite */
    if (ss->width == G_MAXUINT || ss->height == G_MAXUINT) 
    {
        pixbuf = gdk_pixbuf_new_from_file (ss->filename, NULL);
    }
    else
    {
        /* this is always going to have alpha */
        pixbuf = gdk_pixbuf_new_from_file_at_scale (ss->filename,
                                (gint) (ss->width * ss->scale),
                                (gint) (ss->height * ss->scale),
                                FALSE, NULL);
    }

    /* show icon */
    if (g_strcmp0 (ss->current_image, "image1") == 0) 
    {
        if (pixbuf != NULL) 
        {
            soft_app_image_set_from_pixbuf (GTK_IMAGE (ss->image2),
                                            pixbuf, 
                                            gdk_pixbuf_get_width (pixbuf));
        }
        gtk_stack_set_visible_child_name (GTK_STACK (ss), "image2");
        ss->current_image = "image2";
    } 
    else 
    {
        if (pixbuf != NULL) 
        {
            soft_app_image_set_from_pixbuf (GTK_IMAGE (ss->image1),
                                            pixbuf, 
                                            gdk_pixbuf_get_width (pixbuf));
        }
        gtk_stack_set_visible_child_name (GTK_STACK (ss), "image1");
        ss->current_image = "image1";
    }

    gtk_widget_show (GTK_WIDGET (ss));
    ss->showing_image = TRUE;
}
static gboolean
soft_app_screenshot_save_downloaded (SoftAppScreenshot *ss,
									 GdkPixbuf *pixbuf,
									 GError **error)
{
	g_autoptr(AsImage) im = NULL;
    gboolean ret;
    g_autoptr(GError) error_local = NULL;
    g_autofree char *filename = NULL;
    g_autofree char *size_dir = NULL;
    g_autofree char *cache_kind = NULL;
    g_autofree char *basename = NULL;
    guint width = ss->width;
    guint height = ss->height;

    /* save to file, using the same code as the AppStream builder
     * so the preview looks the same */
    im = as_image_new ();
    as_image_set_pixbuf (im, pixbuf);
    ret = as_image_save_filename (im, 
			                      ss->filename,
								  ss->width * ss->scale,
                                  ss->height * ss->scale,
                                  AS_IMAGE_SAVE_FLAG_PAD_16_9,
                                  error);

    if (!ret)
        return FALSE;

    if (width == AS_IMAGE_THUMBNAIL_WIDTH &&
        height == AS_IMAGE_THUMBNAIL_HEIGHT) 
	{
        width = AS_IMAGE_NORMAL_WIDTH;
        height = AS_IMAGE_NORMAL_HEIGHT;
    } 
	else 
	{
        width = AS_IMAGE_THUMBNAIL_WIDTH;
        height = AS_IMAGE_THUMBNAIL_HEIGHT;
    }

    width *= ss->scale;
    height *= ss->scale;
    basename = g_path_get_basename (ss->filename);
    size_dir = g_strdup_printf ("%ux%u", width, height);
    cache_kind = g_build_filename ("screenshots", size_dir, NULL);
    filename = soft_app_get_screenshot_cache_filename (cache_kind, basename,
                        UTILS_CACHE_FLAG_WRITEABLE,
                        &error_local);

    if (filename == NULL) 
	{
		SoftAppStoreLog ("Warning","Failed to get cache filename for counterpart "
                         "screenshot '%s' in folder '%s': %s", basename,
                          cache_kind, error_local->message);
        return TRUE;
    }

    ret = as_image_save_filename (im, filename, width, height,
                      AS_IMAGE_SAVE_FLAG_PAD_16_9,
                      &error_local);

    if (!ret) 
	{
        SoftAppStoreLog ("Warning","Failed to save screenshot '%s': %s", filename,
                           error_local->message);
    }

    return TRUE;
}

static void
soft_app_screenshot_image_complete_cb (SoupSession *session,
									   SoupMessage *msg,
									   gpointer user_data)
{
    SoftAppScreenshot * ss = SOFT_APP_SCREENSHOT (user_data);
    gboolean ret;
    g_autoptr(GError) error = NULL;
    g_autoptr(GdkPixbuf) pixbuf = NULL;
    g_autoptr(GInputStream) stream = NULL;

    /* return immediately if the message was cancelled or if we're in destruction */
    if (msg->status_code == SOUP_STATUS_CANCELLED || ss->session == NULL)
        return;

    if (msg->status_code == SOUP_STATUS_NOT_MODIFIED) 
	{
        SoftAppStoreLog ("Debug","screenshot has not been modified");
        soft_app_screenshot_show_image (ss);
        return;
    }
    if (msg->status_code != SOUP_STATUS_OK) 
	{
		SoftAppStoreLog("Warning","Result of screenshot downloading attempt with "
                        "status code '%u': %s", msg->status_code,
                         msg->reason_phrase);
        /* if we're already showing an image, then don't set the error
         * as having an image (even if outdated) is better */
        if (ss->showing_image)
            return;
        /* TRANSLATORS: this is when we try to download a screenshot and
         * we get back 404 */
        soft_app_screenshot_set_error (ss, _("Screenshot not found"));
        return;
    }

    /* create a buffer with the data */
    stream = g_memory_input_stream_new_from_data (msg->response_body->data,
                              msg->response_body->length,
                              NULL);
    if (stream == NULL)
        return;

    /* load the image */
    pixbuf = gdk_pixbuf_new_from_stream (stream, NULL, NULL);
    if (pixbuf == NULL) 
	{
        /* TRANSLATORS: possibly image file corrupt or not an image */
        soft_app_screenshot_set_error (ss, _("Failed to load image"));
        return;
    }

    /* is image size destination size unknown or exactly the correct size */
    if (ss->width == G_MAXUINT || ss->height == G_MAXUINT ||
        (ss->width * ss->scale == (guint) gdk_pixbuf_get_width (pixbuf) &&
         ss->height * ss->scale == (guint) gdk_pixbuf_get_height (pixbuf))) 
	{
        ret = g_file_set_contents (ss->filename,
                       msg->response_body->data,
                       msg->response_body->length,
                       &error);
        if (!ret) 
		{
            soft_app_screenshot_set_error (ss, error->message);
            return;
        }
    } 
	else if (!soft_app_screenshot_save_downloaded (ss, pixbuf,&error)) 
	{
        soft_app_screenshot_set_error (ss, error->message);
        return;
    }

    /* got image, so show */
    soft_app_screenshot_show_image (ss);
}


static void
soft_app_screenshot_soup_msg_set_modified_request (SoupMessage *msg, GFile *file)
{
    GTimeVal time_val;
    g_autoptr(GDateTime) date_time = NULL;
    g_autoptr(GFileInfo) info = NULL;
    g_autofree gchar *mod_date = NULL;

    info = g_file_query_info (file,
                  G_FILE_ATTRIBUTE_TIME_MODIFIED,
                  G_FILE_QUERY_INFO_NONE,
                  NULL,
                  NULL);
    if (info == NULL)
        return;
    g_file_info_get_modification_time (info, &time_val);
    date_time = g_date_time_new_from_timeval_local (&time_val);
    mod_date = g_date_time_format (date_time, "%a, %d %b %Y %H:%M:%S %Z");
    soup_message_headers_append (msg->request_headers,
                     "If-Modified-Since",
                     mod_date);
}

void soft_app_screenshot_load_async (SoftAppScreenshot *ss,
				                     const char        *url)
{
	g_autofree gchar *basename = NULL;
	g_autofree gchar *cache_kind = NULL;
	g_autofree gchar *sizedir = NULL;
	g_autoptr(SoupURI) base_uri = NULL;

	if (url == NULL) 
    {
		soft_app_screenshot_set_error (ss, _("Screenshot size not found"));
		return;
	}
	ss->scale = (guint) gtk_widget_get_scale_factor (GTK_WIDGET (ss));

	/* check if the URL points to a local file */
    if (g_str_has_prefix (url, "file://")) 
    {
		g_free (ss->filename);
		ss->filename = g_strdup (url + 7);
		if (g_file_test (ss->filename, G_FILE_TEST_EXISTS)) 
        {
			soft_app_screenshot_show_image (ss);
			return;
		}
	}
    
	basename = soft_app_screenshot_get_cachefn_for_url (url);
	if (ss->width == G_MAXUINT || ss->height == G_MAXUINT) 
    {
		sizedir = g_strdup ("unknown");
	} 
    else 
    {
		sizedir = g_strdup_printf ("%ux%u", ss->width * ss->scale, ss->height * ss->scale);
	}
	cache_kind = g_build_filename ("screenshots", sizedir, NULL);
	g_free (ss->filename);
	ss->filename = soft_app_get_screenshot_cache_filename (cache_kind,
														   basename,
														   UTILS_CACHE_FLAG_NONE,
														   NULL);
    if (ss->filename == NULL) 
	{
        /* TRANSLATORS: this is when we try create the cache directory
         * but we were out of space or permission was denied */
        soft_app_screenshot_set_error (ss, _("Could not create cache"));
        return;
    }
    /* does local file already exist and has recently been downloaded */
    if (g_file_test (ss->filename, G_FILE_TEST_EXISTS)) 
	{
        g_autoptr(GFile) file = NULL;
        soft_app_screenshot_show_image (ss);
        file = g_file_new_for_path (ss->filename);
        if (GetCacheFileAge (file) < 604800)
            return;
    }
    g_free (ss->filename);
    ss->filename = soft_app_get_screenshot_cache_filename (cache_kind,
                                                           basename,
                                                           UTILS_CACHE_FLAG_WRITEABLE,
                                                           NULL);

    /* download file */
    SoftAppStoreLog ("Debug","downloading %s to %s", url, ss->filename);
    base_uri = soup_uri_new (url);
    if (base_uri == NULL || !SOUP_URI_VALID_FOR_HTTP (base_uri)) 
	{
        soft_app_screenshot_set_error (ss, _("Screenshot not valid"));
        return;
    }

    /* cancel any previous messages */
    if (ss->message != NULL) 
	{
        soup_session_cancel_message (ss->session,
                                     ss->message,
                                     SOUP_STATUS_CANCELLED);
        g_clear_object (&ss->message);
    }
	ss->message = soup_message_new_from_uri (SOUP_METHOD_GET, base_uri);
    if (ss->message == NULL) 
	{
        /* TRANSLATORS: this is when networking is not available */
        soft_app_screenshot_set_error (ss, _("Screenshot not available"));
        return;
    }
    if (g_file_test (ss->filename, G_FILE_TEST_EXISTS)) 
	{
        g_autoptr(GFile) file = g_file_new_for_path (ss->filename);
        soft_app_screenshot_soup_msg_set_modified_request (ss->message, file);
    }

    /* send async */
    soup_session_queue_message (ss->session,
								g_object_ref (ss->message) /* transfer full */,
								soft_app_screenshot_image_complete_cb,
								g_object_ref (ss));

}
static void
soft_app_screenshot_init (SoftAppScreenshot *ss)
{
    GtkWidget *error_image;
	GtkStyleContext *context;
	g_autoptr(GtkCssProvider) provider = NULL;
    const char *css = "stack {background-color:rgb(191,191,191)}";
	
	context = gtk_widget_get_style_context (GTK_WIDGET (ss));
  //  gtk_widget_set_size_request(GTK_WIDGET (ss), 750, 428);
    provider = gtk_css_provider_new ();
    gtk_css_provider_load_from_data (provider, css, -1, NULL);
	gtk_style_context_add_provider (context, 
                                    GTK_STYLE_PROVIDER (provider),
						            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    ss->showing_image = FALSE;
    
    ss->image1 = gtk_image_new();
    gtk_stack_add_named (GTK_STACK (ss), ss->image1, "image1"); 
    
    ss->image2 = gtk_image_new();
    gtk_stack_add_named (GTK_STACK (ss), ss->image2, "image2"); 

    ss->box_error = gtk_box_new (GTK_ORIENTATION_VERTICAL,6);
    error_image = gtk_image_new_from_icon_name ("dialog-error-symbolic",GTK_ICON_SIZE_MENU);
    gtk_box_pack_start(GTK_BOX(ss->box_error),
                       error_image ,FALSE,FALSE, 
                       10);
    ss->label_error = gtk_label_new (NULL);
    gtk_box_pack_start(GTK_BOX(ss->box_error),
                       ss->label_error ,FALSE,FALSE, 
                       10);
    gtk_stack_add_named (GTK_STACK (ss), ss->box_error, "error"); 
    
}
static void
soft_app_screenshot_destroy (GtkWidget *widget)
{
    SoftAppScreenshot *ss = SOFT_APP_SCREENSHOT (widget);

    if (ss->message != NULL) 
    {
        soup_session_cancel_message (ss->session,
                                     ss->message,
                                     SOUP_STATUS_CANCELLED);
        g_clear_object (&ss->message);
    }
    g_clear_object (&ss->session);
    g_clear_pointer (&ss->filename, g_free);
}

static gboolean
soft_app_screenshot_draw (GtkWidget *widget, cairo_t *cr)
{
    GtkStyleContext *context;

    context = gtk_widget_get_style_context (widget);
    gtk_render_background (context, cr,
                   0, 0,
                   gtk_widget_get_allocated_width (widget),
                   gtk_widget_get_allocated_height (widget));
    gtk_render_frame (context, cr,
              0, 0,
              gtk_widget_get_allocated_width (widget),
              gtk_widget_get_allocated_height (widget));

    return GTK_WIDGET_CLASS (soft_app_screenshot_parent_class)->draw (widget, cr);
}

static void
soft_app_screenshot_class_init (SoftAppScreenshotClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    widget_class->destroy = soft_app_screenshot_destroy;
    widget_class->draw = soft_app_screenshot_draw;
}

GtkWidget *
soft_app_screenshot_new (SoupSession *session)
{
    SoftAppScreenshot *ss;
    ss = g_object_new (SOFT_APP_TYPE_SCREENSHOT, NULL);
    ss->session = g_object_ref (session);
    return GTK_WIDGET (ss);
}
