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

#include "app-store-message.h"
#include "app-store-util.h"

G_DEFINE_TYPE (SoftAppMessage, soft_app_message, AS_TYPE_APP)

const gchar *
soft_app_message_get_icon (SoftAppMessage *Message)
{
    return Message->icon_name;
}

void
soft_app_message_set_icon (SoftAppMessage  *Message, 
                           const gchar     *icon)
{
    g_free (Message->icon_name);
    Message->icon_name = g_strdup (icon);
}  

const gchar *
soft_app_message_get_name (SoftAppMessage *Message)
{
    return Message->soft_name;
}

void
soft_app_message_set_name (SoftAppMessage  *Message, 
                           const gchar     *soft_name)
{
    g_free (Message->soft_name);
    Message->soft_name = g_strdup (soft_name);
} 

const gchar *
soft_app_message_get_summary (SoftAppMessage *Message)
{
    return Message->soft_summary;
}

void
soft_app_message_set_summary (SoftAppMessage  *Message, 
							  const gchar     *soft_summary)
{
    g_free (Message->soft_summary);
    Message->soft_summary = g_strdup (soft_summary);
} 

float
soft_app_message_get_score (SoftAppMessage *Message)
{
    return Message->soft_score;
}

void
soft_app_message_set_score (SoftAppMessage  *Message, 
                           float             soft_score)
{
    Message->soft_score = soft_score;
}  

const gchar *
soft_app_message_get_screenshot (SoftAppMessage *Message)
{
    return Message->screenshot_url;
}

void
soft_app_message_set_screenshot (SoftAppMessage  *Message, 
								 const gchar     *screenshot_url)
{
    g_free (Message->screenshot_url);
    Message->screenshot_url = g_strdup (screenshot_url);
} 

const gchar *
soft_app_message_get_description (SoftAppMessage *Message)
{
    return Message->description;
}

void
soft_app_message_set_description (SoftAppMessage  *Message, 
								  const gchar     *description)
{
    g_free (Message->description);
    Message->description = g_strdup (description);
} 

const gchar *
soft_app_message_get_version (SoftAppMessage *Message)
{
    return Message->soft_version;
}

void
soft_app_message_set_version (SoftAppMessage  *Message, 
                              const gchar     *version)
{
    g_free (Message->soft_version);
    Message->soft_version = g_strdup (version);
} 

const gchar *
soft_app_message_get_license (SoftAppMessage *Message)
{
    return Message->soft_license;
}

void
soft_app_message_set_license (SoftAppMessage  *Message, 
                              const gchar     *licenses)
{
    g_free (Message->soft_license);
    Message->soft_license = g_strdup (licenses);
}  

const gchar*
soft_app_message_get_size (SoftAppMessage *Message)
{
    return Message->soft_size;
}

void
soft_app_message_set_size (SoftAppMessage  *Message, 
                           const gchar     *soft_size)
{
    g_free (Message->soft_size);
    Message->soft_size = g_strdup (soft_size);
}  

const gchar *
soft_app_message_get_url (SoftAppMessage *Message)
{
    return Message->soft_homepage;
}

void
soft_app_message_set_url (SoftAppMessage  *Message, 
                          const gchar     *url)
{
    g_free (Message->soft_homepage);
    Message->soft_homepage = g_strdup (url);
} 

const gchar *
soft_app_message_get_arch (SoftAppMessage *Message)
{
    return Message->soft_arch;
}

void
soft_app_message_set_arch (SoftAppMessage  *Message, 
                           const gchar     *arch)
{
    g_free (Message->soft_arch);
    Message->soft_arch = g_strdup (arch);
}  
const gchar *
soft_app_message_get_package (SoftAppMessage *Message)
{
    return Message->soft_package;
}

void
soft_app_message_set_package (SoftAppMessage  *Message, 
                           const gchar     *package)
{
    g_free (Message->soft_package);
    Message->soft_package = g_strdup (package);
}  

const gchar *
soft_app_message_get_pkgname (SoftAppMessage *Message)
{
    return Message->soft_pkgname;
}

void
soft_app_message_set_pkgname (SoftAppMessage  *Message, 
                              const gchar     *pkgname)
{
    g_free (Message->soft_pkgname);
    Message->soft_pkgname = g_strdup(pkgname);
}


const gchar *
soft_app_message_get_cache (SoftAppMessage *Message)
{
    return Message->cache_file;
}

void
soft_app_message_set_cache (SoftAppMessage  *Message, 
                            const gchar      *cache)
{
    g_free (Message->cache_file);
    Message->cache_file = g_strdup (cache);
}  
gchar **
soft_app_message_get_files (SoftAppMessage *Message)
{
    return Message->soft_files;
}

void
soft_app_message_set_files (SoftAppMessage  *Message, 
                            gchar          **files)
{
    g_strfreev (Message->soft_files);
    Message->soft_files = g_strdupv (files);
}    

int
soft_app_message_get_index (SoftAppMessage *Message)
{
    return Message->soft_index;
}

void
soft_app_message_set_index (SoftAppMessage  *Message, 
                            int              index)
{
    Message->soft_index = index;
} 

gboolean
soft_app_message_get_state (SoftAppMessage *Message)
{
    return Message->soft_state;
}

void
soft_app_message_set_state (SoftAppMessage  *Message, 
                            gboolean         state)
{
    Message->soft_state = state;
} 

gboolean
soft_app_message_get_mode (SoftAppMessage *Message)
{
    return Message->soft_mode;
}

void
soft_app_message_set_mode (SoftAppMessage  *Message, 
                           gboolean         mode)
{
    Message->soft_mode = mode;
}

const gchar *
soft_app_message_get_button (SoftAppMessage *Message)
{
    return Message->button_name;
}

void
soft_app_message_set_button (SoftAppMessage  *Message, 
                            const gchar      *button_name)
{
    g_free (Message->button_name);
    Message->button_name = g_strdup (button_name);
}  
static void
soft_app_message_finalize (GObject *object)
{
    SoftAppMessage *Message = SOFT_APP_MESSAGE (object);

    g_free (Message->icon_name);
    g_free (Message->soft_name);
    g_free (Message->soft_summary);
    g_free (Message->screenshot_url);
    g_free (Message->description);
    g_free (Message->soft_version);
    g_free (Message->soft_license);
    g_free (Message->soft_size);
    g_free (Message->soft_homepage);
    g_free (Message->soft_arch);
    g_free (Message->soft_package);
    g_free (Message->soft_pkgname);
    g_free (Message->cache_file);
    g_free (Message->button_name);
    g_strfreev (Message->soft_files);
}

static void
soft_app_message_class_init (SoftAppMessageClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = soft_app_message_finalize;
}

static void
soft_app_message_init (SoftAppMessage *Message)
{

}

SoftAppMessage *
soft_app_message_new (void)
{
    SoftAppMessage *Message;
    Message = g_object_new (SOFT_APP_TYPE_MESSAGE, NULL);
    
    return SOFT_APP_MESSAGE (Message);
}
