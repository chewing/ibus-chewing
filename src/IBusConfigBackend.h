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
 * SECTION:IBusConfigBackend
 * @short_description: An IBus-Config Backend
 * @title: IBusConfig Backend
 * @stability: Stable
 * @include: IBusConfigBackend.h
 *
 * A MakerDialog backend provides an interface to configuration service
 * like GConf2 or dconf
 */

#ifndef _IBUS_CONFIG_BACKEND_H_
#define _IBUS_CONFIG_BACKEND_H_
#include "MakerDialogBackend.h"

MkdgBackend *ibus_config_backend_new(IBusService * service,
	gpointer auxData);

#endif /* _IBUS_CONFIG_BACKEND_H_ */
