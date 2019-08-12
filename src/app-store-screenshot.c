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


static char *soft_app_get_cache_filename (const gchar *kind,
                                          const gchar *resource)
{
    g_autofree gchar *basename = NULL;
    g_autofree gchar *cachedir = NULL;
    g_autoptr(GFile) cachedir_file = NULL;
    g_autoptr(GPtrArray) candidates = g_ptr_array_new_with_free_func (g_free);
    g_autofree gchar *basename_tmp = g_path_get_basename (resource);
    g_autofree gchar *hash = g_compute_checksum_for_string (G_CHECKSUM_SHA1,
                                                            resource, 
                                                            -1);
    
    basename = g_strdup_printf ("%s-%s", hash, basename_tmp);
    cachedir = g_build_filename (g_get_user_cache_dir (),
                                "soft-app-store",
                                 kind,
                                 NULL);
    cachedir_file = g_file_new_for_path (cachedir);
    if (!g_file_query_exists (cachedir_file, NULL) &&
        !g_file_make_directory_with_parents (cachedir_file, NULL, NULL))
        return NULL;
    return g_build_filename (cachedir, basename, NULL);
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

void soft_app_screenshot_load_async (SoftAppScreenshot *ss,
				                     const char        *url)
{
	g_autofree gchar *basename = NULL;
	g_autofree gchar *cache_kind = NULL;
	g_autofree gchar *cachefn_thumb = NULL;
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
	ss->filename = soft_app_get_cache_filename (cache_kind,
						                        basename);
    g_print("ss->filename = %s\r\n",ss->filename);
	if (ss->filename == NULL) 
    {
		soft_app_screenshot_set_error (ss, _("Could not create cache"));
		return;
	}
	/* does local file already exist and has recently been downloaded */
	if (g_file_test (ss->filename, G_FILE_TEST_EXISTS)) 
    {
		soft_app_screenshot_show_image (ss);
		return;
	}

	/* re-request the cache filename, which might be different as it needs
	 * to be writable this time */
/*	g_free (ssimg->filename);
	ssimg->filename = gs_utils_get_cache_filename (cache_kind,
						       basename,
						       GS_UTILS_CACHE_FLAG_WRITEABLE,
						       NULL);
*/
	/* download file */
/*	g_print ("downloading %s to %s\r\n", url, ssimg->filename);
	base_uri = soup_uri_new (url);
	if (base_uri == NULL || !SOUP_URI_VALID_FOR_HTTP (base_uri)) {
*/
        /* TRANSLATORS: this is when we try to download a screenshot
		 * that was not a valid URL */
/*		gs_screenshot_image_set_error (ssimg, _("Screenshot not valid"));
		return;
	}
*/
	/* cancel any previous messages */
/*	if (ssimg->message != NULL) {
		soup_session_cancel_message (ssimg->session,
		                             ssimg->message,
		                             SOUP_STATUS_CANCELLED);
		g_clear_object (&ssimg->message);
	}

	ssimg->message = soup_message_new_from_uri (SOUP_METHOD_GET, base_uri);
	if (ssimg->message == NULL) {
*/
        /* TRANSLATORS: this is when networking is not available */
/*		gs_screenshot_image_set_error (ssimg, _("Screenshot not available"));
		return;
	}
*/
	/* not all servers support If-Modified-Since, but worst case we just
	 * re-download the entire file again every 30 days */
/*	if (g_file_test (ssimg->filename, G_FILE_TEST_EXISTS)) {
		g_autoptr(GFile) file = g_file_new_for_path (ssimg->filename);
		gs_screenshot_soup_msg_set_modified_request (ssimg->message, file);
	}
*/
	/* send async */
/*	soup_session_queue_message (ssimg->session,
				    g_object_ref (ssimg->message) ,
				    gs_screenshot_image_complete_cb,
				    g_object_ref (ssimg));
*/
}
static void
soft_app_screenshot_init (SoftAppScreenshot *ss)
{
    GtkWidget *error_image;
	GtkStyleContext *context;
	g_autoptr(GtkCssProvider) provider = NULL;
    char *css = "stack {background-color:rgb(191,191,191)}";
	
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
    g_clear_object (&ss->screenshot);
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
