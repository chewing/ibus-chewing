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
 * SECTION:IBusChewingLookupTable
 * @short_description: Lookup table for ibus-chewing
 * @title: IBusChewingLookupTable
 * @stability: Stable
 * @include: IbusChewingLookupTable.h
 *
 * IBusChewingLookup provides subroutines for manipulate
 * IBusLookupTable.
 */

#ifndef _IBUS_CHEWING_LOOKUP_TABLE_H_
#define _IBUS_CHEWING_LOOKUP_TABLE_H_
#include <ibus.h>
#include <chewing.h>
#include "IBusChewingProperties.h"

IBusLookupTable *ibus_chewing_lookup_table_new(IBusChewingProperties *
					       iProperties,
					       ChewingContext * context);

void ibus_chewing_lookup_table_resize(IBusLookupTable * iTable,
				      IBusChewingProperties * iProperties,
				      ChewingContext * context);

guint ibus_chewing_lookup_table_update(IBusLookupTable * iTable,
	IBusChewingProperties * iProperties,
	ChewingContext * context);

#endif				/* _IBUS_CHEWING_LOOKUP_TABLE_H_ */
