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

#ifndef __APP_STORE_H__
#define __APP_STORE_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtk/gtk.h>
#include <libintl.h>   
#include <locale.h>   
#include <app-store-pkgkit.h>

#define _(STRING)  gettext(STRING)   

typedef struct 
{
    GtkWidget        *MainWindow;
    GtkWidget        *NoteBook;
	GtkWidget        *StoreBox;
    GtkWidget        *StoreStack;
    GtkWidget        *LocalBox;
    GtkWidget        *UpdateBox;
    PackageApp       *pkg;

}SoftAppStore;

#endif
