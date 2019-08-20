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

#ifndef __APP_STORE_MESSAGE_H__
#define __APP_STORE_MESSAGE_H__

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


typedef struct SoftAppMessage
{
    AsApp        parent_instance;
    
    char        *icon_name;
    char        *soft_name;
	char        *soft_summary;
    float        soft_score;
	char        *screenshot_url;
	char        *description;
    char        *soft_version;
    char        *soft_license;
	char        *soft_size;
    char        *soft_homepage;
    char        *soft_arch;
    char        *soft_package;//package name
	char        *soft_pkgname;//install formt  
	char        *cache_file;
	char       **soft_files;
	char        *button_name; //button name install installed uninstall
    int          soft_index;
	gboolean     soft_state;
    gboolean     soft_mode;  //mode: TRUE  local soft 
                             //mode: FALSE store soft
}SoftAppMessage;

typedef struct SoftAppMessageClass
{
    AsAppClass parent_instance_class;
}SoftAppMessageClass;

GType             soft_app_message_get_type            (void) G_GNUC_CONST;

SoftAppMessage   *soft_app_message_new                 (void);


const gchar      *soft_app_message_get_icon            (SoftAppMessage *Message);

void              soft_app_message_set_icon            (SoftAppMessage *Message,
                                                        const gchar    *soft_icon);

const gchar      *soft_app_message_get_name            (SoftAppMessage *Message);

void              soft_app_message_set_name            (SoftAppMessage *Message,
                                                        const gchar    *soft_name);

float             soft_app_message_get_score           (SoftAppMessage *Message);

void              soft_app_message_set_score           (SoftAppMessage *Message,
                                                        float           soft_score);

const gchar      *soft_app_message_get_summary         (SoftAppMessage *Message);

void              soft_app_message_set_summary         (SoftAppMessage *Message,
                                                        const gchar    *soft_summary);

const gchar      *soft_app_message_get_screenshot      (SoftAppMessage *Message);

void              soft_app_message_set_screenshot      (SoftAppMessage *Message,
                                                        const gchar    *screenshot_url);

const gchar      *soft_app_message_get_description     (SoftAppMessage *Message);

void              soft_app_message_set_description     (SoftAppMessage *Message,
                                                        const gchar    *description);

const gchar      *soft_app_message_get_version         (SoftAppMessage *Message);

void              soft_app_message_set_version         (SoftAppMessage *Message,
                                                        const gchar    *version);
 
const gchar      *soft_app_message_get_license         (SoftAppMessage *Message);

void              soft_app_message_set_license         (SoftAppMessage *Message,
                                                        const gchar    *license);

const gchar      *soft_app_message_get_size            (SoftAppMessage *Message);

void              soft_app_message_set_size            (SoftAppMessage *Message,
                                                        const gchar    *soft_size);


const gchar      *soft_app_message_get_url             (SoftAppMessage *Message);

void              soft_app_message_set_url             (SoftAppMessage *Message,
                                                        const gchar    *url);

const gchar      *soft_app_message_get_arch            (SoftAppMessage *Message);

void              soft_app_message_set_arch            (SoftAppMessage *Message,
                                                        const gchar    *arch);

const gchar      *soft_app_message_get_package         (SoftAppMessage *Message);

void              soft_app_message_set_package         (SoftAppMessage *Message,
                                                        const gchar    *package);

const gchar      *soft_app_message_get_pkgname         (SoftAppMessage *Message);

void              soft_app_message_set_pkgname         (SoftAppMessage *Message,
                                                        const gchar    *pkgname);

const gchar      *soft_app_message_get_cache           (SoftAppMessage *Message);

void              soft_app_message_set_cache           (SoftAppMessage *Message,
                                                        const gchar    *cache);

char            **soft_app_message_get_files           (SoftAppMessage *Message);

void              soft_app_message_set_files           (SoftAppMessage *Message,
                                                        gchar         **files);
int               soft_app_message_get_index           (SoftAppMessage *Message);

void              soft_app_message_set_index           (SoftAppMessage *Message,
                                                        int             index);

gboolean          soft_app_message_get_state           (SoftAppMessage *Message);

void              soft_app_message_set_state           (SoftAppMessage *Message,
		                                                gboolean        state);

const gchar      *soft_app_message_get_button          (SoftAppMessage *Message);

void              soft_app_message_set_button          (SoftAppMessage *Message,
                                                        const gchar    *button);

gboolean          soft_app_message_get_mode            (SoftAppMessage *Message);

void              soft_app_message_set_mode            (SoftAppMessage *Message,
		                                                gboolean        mode);
G_END_DECLS

#endif
