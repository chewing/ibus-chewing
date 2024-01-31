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

#include <glib.h>
#include <glib-object.h>


#ifndef MAKER_DIALOG_H_
#define MAKER_DIALOG_H_
#include <stdlib.h>
#include <strings.h>
#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <locale.h>
#include "MakerDialogProperty.h"
#include "MakerDialogWidget.h"



#define MKDG_PAGE_NAME_DEFAULT "..."
#ifndef WIDGET_ID_PREFIX
#define WIDGET_ID_PREFIX "+"
#endif

#ifndef MAKER_DIALOG_ID_LENGTH
#define MAKER_DIALOG_ID_LENGTH 200
#endif

#define MKDG_CONTAINER_TABLE_LOOKUP(table,key) (MkdgWidgetContainer *) g_hash_table_lookup(table, (gconstpointer) key)



#endif    /* MAKER_DIALOG_H_ */

#ifndef __MAKER_DIALOG_H__
#define __MAKER_DIALOG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
 * Type checking and casting macros
 */
#define MAKER_TYPE_DIALOG	(maker_dialog_get_type())
#define MAKER_DIALOG(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), maker_dialog_get_type(), MakerDialog)
#define MAKER_DIALOG_CONST(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), maker_dialog_get_type(), MakerDialog const)
#define MAKER_DIALOG_CLASS(klass)	G_TYPE_CHECK_CLASS_CAST((klass), maker_dialog_get_type(), MakerDialogClass)
#define MAKER_IS_DIALOG(obj)	G_TYPE_CHECK_INSTANCE_TYPE((obj), maker_dialog_get_type ())

#define MAKER_DIALOG_GET_CLASS(obj)	G_TYPE_INSTANCE_GET_CLASS((obj), maker_dialog_get_type(), MakerDialogClass)

/* Private structure type */
typedef struct _MakerDialogPrivate MakerDialogPrivate;

/*
 * Main object structure
 */
#ifndef __TYPEDEF_MAKER_DIALOG__
#define __TYPEDEF_MAKER_DIALOG__
typedef struct _MakerDialog MakerDialog;
#endif
struct _MakerDialog {
	GtkDialog __parent__;
	/*< private >*/
	MkdgWgt * notebookWgt; /* protected */
	MkdgWidgetFlag wFlags; /* protected */
	MakerDialogPrivate *_priv;
};

/*
 * Class definition
 */
typedef struct _MakerDialogClass MakerDialogClass;
struct _MakerDialogClass {
	GtkDialogClass __parent__;
};


/*
 * Public methods
 */
GType	maker_dialog_get_type	(void) G_GNUC_CONST;
MakerDialog * 	maker_dialog_new	(void);
MakerDialog * 	maker_dialog_new_full	(MkdgProperties * properties,
					const gchar * title,
					MkdgWidgetFlag wFlags,
					MkdgButtonFlag bFlags);
gboolean 	maker_dialog_add_property	(MakerDialog * self,
					PropertyContext * ctx);
gboolean 	maker_dialog_add_all_properties	(MakerDialog * self);
void 	maker_dialog_assign_widget_value	(MakerDialog * self,
					PropertyContext * ctx,
					gpointer userData);
void 	maker_dialog_assign_all_widgets_values	(MakerDialog * self,
					gpointer userData);
void 	maker_dialog_save_all_widgets_values	(MakerDialog * self,
					gpointer userData);
GValue * 	maker_dialog_get_widget_value	(MakerDialog * self,
					const gchar * key,
					GValue * value);
void 	maker_dialog_set_widget_value	(MakerDialog * self,
					const gchar * key,
					GValue * value);
void 	maker_dialog_show	(MakerDialog * self);
MkdgWidget * 	maker_dialog_get_widget_by_key	(MakerDialog * self,
					const gchar * key);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
