/*
 * Copyright © 2014  Red Hat, Inc. All rights reserved.
 * Copyright © 2014  Ding-Yi Chen <dchen@redhat.com>
 *
 * This file is part of the ibus-chewing Project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * SECTION:MakerDialogBackend
 * @short_description: An interface to configuration service
 * @title: MakerDialog Backend
 * @stability: Stable
 * @include: MakerDailogBackend.h
 *
 * A MakerDialog backend provides an interface to configuration service like GConf2 or dconf
 */
#ifndef _MAKER_DIALOG_BACKEND_H_
#define _MAKER_DIALOG_BACKEND_H_
#include <glib.h>
#include <glib-object.h>

typedef struct MkdgBackend_ MkdgBackend;

typedef GValue *(*BackendReadFunc) (MkdgBackend * backend, GValue * value,
				    const gchar * section,
				    const gchar * key, gpointer userData);

typedef gboolean(*BackendWriteFunc) (MkdgBackend * backend, GValue * value,
				     const gchar * section,
				     const gchar * key, gpointer userData);


/**
 * MkdgBackend:
 * @config: A configuration service
 * @baseDir: All keys and sub-sections of this project should be put under this dir (without trailing '/')
 * @readFunc: Callback function for load
 * @writeFunc: Callback function for save
 * @auxData:  Auxiliary data that might be useful.
 *
 * A MkdgBackend object.
 */
struct MkdgBackend_ {
    gpointer config;
    const gchar *baseDir;
    BackendReadFunc readFunc;
    BackendWriteFunc writeFunc;
    gpointer auxData;
};

MkdgBackend *mkdg_backend_new(gpointer config, const gchar *baseDir, gpointer auxData);


GValue *mkdg_backend_read(MkdgBackend * backend, GValue * value,
			  const gchar * section, const gchar * key,
			  gpointer userData);

gboolean mkdg_backend_write(MkdgBackend * backend, GValue * value,
			    const gchar * section, const gchar * key,
			    gpointer userData);

#endif				/* _MAKER_DIALOG_BACKEND_H_ */
