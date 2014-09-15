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
 * SECTION:GConf2Backend
 * @short_description: An IBus-Config Backend
 * @title: GConf2 Backend
 * @stability: Stable
 * @include: GConf2Backend.h
 *
 * A MakerDialog backend provides an interface to configuration service
 * like GConf2 or dconf
 */

#ifndef _G_CONF2_BACKEND_H_
#define _G_CONF2_BACKEND_H_
#include "MakerDialogBackend.h"

/**
 * gconf2_backend_new:
 * @baseDir: All keys and sub-sections of this project should be put under this dir (without trailing '/')
 * @auxData: Auxiliary data.
 *
 * @returns: And GCon2 backend for this project.
 */
MkdgBackend *gconf2_backend_new(const gchar * baseDir, gpointer auxData);

#endif				/* _G_CONF2_BACKEND_H_ */
