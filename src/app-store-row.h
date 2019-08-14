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

#ifndef __APP_STORE_ROW_H__
#define __APP_STORE_ROW_H__

#define I_KNOW_THE_PACKAGEKIT_GLIB2_API_IS_SUBJECT_TO_CHANGE
#include <packagekit-glib2/packagekit.h>
#include <gtk/gtk.h>
#include <appstream-glib.h>

G_BEGIN_DECLS
#define SOFT_APP_TYPE_MESSAGE            (soft_app_message_get_type ())

#define SOFT_APP_MESSAGE(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object),\
                                          SOFT_APP_TYPE_MESSAGE,\
                                          SoftAppMessage))

#define SOFT_APP_MESSAGE_CLASS(o)        (G_TYPE_INSTANCE_GET_CLASS ((o), \
			                              SOFT_APP_TYPE_MESSAGE, \
			                              SoftAppMessageClass))


#define SOFT_APP_TYPE_ROW                (soft_app_row_get_type ())
#define SOFT_APP_ROW(object)             (G_TYPE_CHECK_INSTANCE_CAST ((object),\
                                          SOFT_APP_TYPE_ROW,\
                                          SoftAppRow))


typedef struct SoftAppMessage
{
    AsApp        parent_instance;
    
    char        *icon_name;
    float        soft_score;
    char        *soft_arch;
    char        *soft_version;
    char        *soft_url;
    char        *soft_license;
    char        *soft_package;
	char        *soft_size;
	char        *soft_ids;
	char        *cache;
	char       **soft_files;
}SoftAppMessage;

typedef struct SoftAppMessageClass
{
    AsAppClass parent_instance_class;
	void        (*get_local_soft_detalis)   (SoftAppMessage *m,
			                                 PkClient       *client,
			                                 const char    **package_ids,
											 GCancellable   *cancellable,
											 PkProgressCallback progress_callback,
                                             gpointer        progress_user_data,
                                             GAsyncReadyCallback callback_ready,
                                             gpointer        user_data);
}SoftAppMessageClass;

typedef struct SoftAppRow
{
	GtkListBoxRow	parent_instance;

    SoftAppMessage *Message;
	GtkWidget	   *label_name;
	GtkWidget	   *image;
	GtkWidget      *stars1;
	GtkWidget      *stars2;
	GtkWidget      *stars3;
	GtkWidget      *stars4;
	GtkWidget      *stars5;
	GtkWidget      *label_describe;
	GtkWidget      *button;
	GtkWidget      *label_size;

}SoftAppRow;
typedef struct SoftAppRowClass
{
	GtkListBoxRowClass	 parent_instance_class;

}SoftAppRowClass;

GType             soft_app_message_get_type            (void) G_GNUC_CONST;
GType             soft_app_row_get_type                (void) G_GNUC_CONST;

SoftAppMessage   *soft_app_message_new                 (void);

GtkWidget        *soft_app_row_new                     (SoftAppMessage *Message);

const gchar      *soft_app_message_get_icon            (SoftAppMessage *Message);

void              soft_app_message_set_icon            (SoftAppMessage *Message,
                                                        const gchar    *soft_con);

float             soft_app_message_get_score           (SoftAppMessage *Message);

void              soft_app_message_set_score           (SoftAppMessage *Message,
                                                        float           soft_score);

const gchar      *soft_app_message_get_size            (SoftAppMessage *Message);

void              soft_app_message_set_size            (SoftAppMessage *Message,
                                                        const gchar    *soft_size);


const gchar      *soft_app_message_get_version         (SoftAppMessage *Message);

void              soft_app_message_set_version         (SoftAppMessage *Message,
                                                        const gchar    *version);
 
const gchar      *soft_app_message_get_license         (SoftAppMessage *Message);

void              soft_app_message_set_license         (SoftAppMessage *Message,
                                                        const gchar    *license);

const gchar      *soft_app_message_get_url             (SoftAppMessage *Message);

void              soft_app_message_set_url             (SoftAppMessage *Message,
                                                        const gchar    *url);

const gchar      *soft_app_message_get_arch            (SoftAppMessage *Message);

void              soft_app_message_set_arch            (SoftAppMessage *Message,
                                                        const gchar    *arch);

const gchar      *soft_app_message_get_package         (SoftAppMessage *Message);

void              soft_app_message_set_package         (SoftAppMessage *Message,
                                                        const gchar    *package);

const gchar      *soft_app_message_get_pkgid           (SoftAppMessage *Message);

void              soft_app_message_set_pkgid           (SoftAppMessage *Message,
                                                        const gchar    *ids);

const gchar      *soft_app_message_get_cache           (SoftAppMessage *Message);

void              soft_app_message_set_cache           (SoftAppMessage *Message,
                                                        const gchar    *cache);

char            **soft_app_message_get_files           (SoftAppMessage *Message);

void              soft_app_message_set_files           (SoftAppMessage *Message,
                                                        gchar         **files);

void              soft_app_local_soft_detalis          (SoftAppMessage *Message,
		                                                PkClient       *client,
			                                            const char    **package_ids,
								   	                    GCancellable   *cancellable,
                                                        GAsyncReadyCallback callback_ready,
                                                        gpointer        user_data);

void              soft_app_row_set_list                (SoftAppRow     *Row,
                                                        SoftAppMessage *Message);

G_END_DECLS

#endif
