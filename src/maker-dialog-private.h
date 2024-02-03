/*
 * Copyright © 2009  Red Hat, Inc. All rights reserved.
 * Copyright © 2009  Ding-Yi Chen <dchen at redhat.com>
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

#ifndef __MAKER_DIALOG_PRIVATE_H__
#define __MAKER_DIALOG_PRIVATE_H__

#include "maker-dialog.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct _MakerDialogPrivate {
	GHashTable * containerTable;
	GHashTable * widgetTable;
	GHashTable * notebookContentTable;
	GStringChunk * widgetIds;
	GPtrArray * pageLabels;
	MkdgProperties * properties;
};
gboolean 	maker_dialog_prepare	(MakerDialog * self);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
