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
 * SECTION:GSettingsBackend
 * @short_description: GSettings backend
 * @title: GSettings Backend
 * @stability: Stable
 * @include: GSettingsBackend.h
 *
 * GSettings as MakerDialog backend.
 */

#ifndef _MKDG_GSETTINGS_BACKEND_H_
#define _MKDG_GSETTINGS_BACKEND_H_
#include <stdio.h>
#include <glib.h>
#include <gio/gio.h>
#include "MakerDialogPropertySpec.h"
#include "MakerDialogBackend.h"
#define GSETTINGS_BACKEND_ID "gsettings"

GValue *mkdg_g_settings_read_value(GSettings * settings,
				   GValue * value, const gchar * key);


/**
 * mkdg_g_settings_backend_new:
 * @schemaId: GSettings Schema ID.
 * @basePath: GSettings path with trailing '/'
 * @auxData: Auxiliary data.
 *
 * @returns: An GSettings backend for this project.
 *
 */

MkdgBackend *mkdg_g_settings_backend_new(const gchar * schemaId,
					 const gchar * basePath,
					 gpointer auxData);

gboolean mkdg_g_settings_write_schema_from_spec_array(const gchar *
						      schemaId,
						      const gchar *
						      basePath,
						      FILE * file,
						      MkdgPropertySpec
						      specs[],
						      const gchar *
						      gettextDomain);

#endif				/* _MKDG_GSETTINGS_BACKEND_H_ */
