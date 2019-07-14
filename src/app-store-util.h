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

#ifndef __APP_STORE_UTIL_H__
#define __APP_STORE_UTIL_H__

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "app-store.h"

#define TYPEMSG    "<span foreground='red'font_desc='13'>%s </span>"

#define ERROR      1
#define WARING     2
#define INFOR      3
#define QUESTION   4

void         SoftAppStoreLog       (const char  *level,
                                    const char  *message,
                                    ...);

void         CloseLogFile          (void);

gboolean     WhetherUseHeadbar     (void);


int          MessageReport         (const char  *Title,
                                    const char  *Msg,
                                    int          nType);

void         SetLableFontType      (GtkWidget   *Lable ,
                                    const char  *Color,
                                    int          FontSzie,
                                    const char  *Word);

GtkWidget   *SetButtonIcon         (const char  *button_text,
		                            const char  *icon_name);

#endif
