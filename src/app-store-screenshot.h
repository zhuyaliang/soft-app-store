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

#ifndef __APP_STORE_SCREENSHOT_H__
#define __APP_STORE_SCREENSHOT_H__
#include <appstream-glib.h>
#include <libsoup/soup.h>
 
G_BEGIN_DECLS
#define SOFT_APP_TYPE_SCREENSHOT             (soft_app_screenshot_get_type ())
#define SOFT_APP_SCREENSHOT(object)          (G_TYPE_CHECK_INSTANCE_CAST ((object),\
                                              SOFT_APP_TYPE_SCREENSHOT,\
                                              SoftAppScreenshot))

typedef struct SoftAppScreenshot
{
    GtkStack     parent_instance;
    GtkWidget   *box_error;
    GtkWidget   *image1;
    GtkWidget   *image2;
    GtkWidget   *label_error;
    GSettings   *settings;
    SoupSession *session;
    SoupMessage *message;
    gchar       *filename;
    const gchar *current_image;
    guint        width;
    guint        height;
    guint        scale;
    gboolean     showing_image;

}SoftAppScreenshot;

typedef struct SoftAppScreenshotClass
{
    GtkStackClass parent_instance_class;
}SoftAppScreenshotClass;

GType             soft_app_screenshot_get_type      (void) G_GNUC_CONST;

GtkWidget        *soft_app_screenshot_new           (SoupSession       *session);
/*
AsScreenshot     *soft_app_screenshot_get_asscr     (SoftAppScreenshot *ss);

void              soft_app_screenshot_set_asscr     (SoftAppScreenshot *ss,
                                                     AsScreenshot      *screenshot);
*/
void              soft_app_screenshot_set_size      (SoftAppScreenshot *ss,
                                                     guint              width,
                                                     guint              height);

void              soft_app_screenshot_load_async    (SoftAppScreenshot *ss,
                                                     const char        *screenshot_url);

G_END_DECLS
#endif
