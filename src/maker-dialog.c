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

#define GOB_VERSION_MAJOR 2
#define GOB_VERSION_MINOR 0
#define GOB_VERSION_PATCHLEVEL 20

#define selfp (self->_priv)

#include <string.h> /* memset() */

#include "maker-dialog.h"

#include "maker-dialog-private.h"

#ifdef G_LIKELY
#define ___GOB_LIKELY(expr) G_LIKELY(expr)
#define ___GOB_UNLIKELY(expr) G_UNLIKELY(expr)
#else /* ! G_LIKELY */
#define ___GOB_LIKELY(expr) (expr)
#define ___GOB_UNLIKELY(expr) (expr)
#endif /* G_LIKELY */


#define GETTEXT_PACKAGE "gtk30"
#include <glib/gi18n.h>

/* self casting macros */
#define SELF(x) MAKER_DIALOG(x)
#define SELF_CONST(x) MAKER_DIALOG_CONST(x)
#define IS_SELF(x) MAKER_IS_DIALOG(x)
#define TYPE_SELF MAKER_TYPE_DIALOG
#define SELF_CLASS(x) MAKER_DIALOG_CLASS(x)

#define SELF_GET_CLASS(x) MAKER_DIALOG_GET_CLASS(x)

/* self typedefs */
typedef MakerDialog Self;
typedef MakerDialogClass SelfClass;

/* here are local prototypes */
static void maker_dialog_class_init (MakerDialogClass * c) G_GNUC_UNUSED;
static void maker_dialog_init (MakerDialog * self) G_GNUC_UNUSED;

/* pointer to the class of our parent */
static GtkDialogClass *parent_class = NULL;

/* Short form macros */
#define self_new maker_dialog_new
#define self_new_full maker_dialog_new_full
#define self_add_property maker_dialog_add_property
#define self_add_all_properties maker_dialog_add_all_properties
#define self_prepare maker_dialog_prepare
#define self_assign_widget_value maker_dialog_assign_widget_value
#define self_assign_all_widgets_values maker_dialog_assign_all_widgets_values
#define self_save_all_widgets_values maker_dialog_save_all_widgets_values
#define self_get_widget_value maker_dialog_get_widget_value
#define self_set_widget_value maker_dialog_set_widget_value
#define self_show maker_dialog_show
#define self_get_widget_by_key maker_dialog_get_widget_by_key
GType
maker_dialog_get_type (void)
{
	static GType type = 0;

	if ___GOB_UNLIKELY(type == 0) {
		static const GTypeInfo info = {
			sizeof (MakerDialogClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) maker_dialog_class_init,
			(GClassFinalizeFunc) NULL,
			NULL /* class_data */,
			sizeof (MakerDialog),
			0 /* n_preallocs */,
			(GInstanceInitFunc) maker_dialog_init,
			NULL
		};

		type = g_type_register_static (GTK_TYPE_DIALOG, "MakerDialog", &info, (GTypeFlags)0);
	}

	return type;
}

/* a macro for creating a new object of our type */
#define GET_NEW ((MakerDialog *)g_object_new(maker_dialog_get_type(), NULL))

/* a function for creating a new object of our type */
#include <stdarg.h>
static MakerDialog * GET_NEW_VARG (const char *first, ...) G_GNUC_UNUSED;
static MakerDialog *
GET_NEW_VARG (const char *first, ...)
{
	MakerDialog *ret;
	va_list ap;
	va_start (ap, first);
	ret = (MakerDialog *)g_object_new_valist (maker_dialog_get_type (), first, ap);
	va_end (ap);
	return ret;
}


static void
___finalize(GObject *obj_self)
{
#define __GOB_FUNCTION__ "Maker:Dialog::finalize"
	MakerDialog *self G_GNUC_UNUSED = MAKER_DIALOG (obj_self);
	gpointer priv G_GNUC_UNUSED = self->_priv;
	if(self->notebookWgt) { g_object_unref ((gpointer) self->notebookWgt); self->notebookWgt = NULL; }
	if(self->_priv->containerTable) { g_hash_table_destroy ((gpointer) self->_priv->containerTable); self->_priv->containerTable = NULL; }
	if(self->_priv->widgetTable) { g_hash_table_destroy ((gpointer) self->_priv->widgetTable); self->_priv->widgetTable = NULL; }
	if(self->_priv->notebookContentTable) { g_hash_table_destroy ((gpointer) self->_priv->notebookContentTable); self->_priv->notebookContentTable = NULL; }
	if(self->_priv->widgetIds) { g_string_chunk_free ((gpointer) self->_priv->widgetIds); self->_priv->widgetIds = NULL; }
#define pageLabels (self->_priv->pageLabels)
#define VAR pageLabels
	{
	
            if (VAR) {
                g_ptr_array_free(VAR, TRUE);
            }
        }
	memset(&(pageLabels), 0, sizeof(pageLabels));
#undef VAR
#undef pageLabels
#define properties (self->_priv->properties)
#define VAR properties
	{
	
            if (VAR) {
                mkdg_properties_free(VAR);
            }
        }
	memset(&(properties), 0, sizeof(properties));
#undef VAR
#undef properties
	if(G_OBJECT_CLASS(parent_class)->finalize) \
		(* G_OBJECT_CLASS(parent_class)->finalize)(obj_self);
}
#undef __GOB_FUNCTION__

static void 
maker_dialog_class_init (MakerDialogClass * c G_GNUC_UNUSED)
{
#define __GOB_FUNCTION__ "Maker:Dialog::class_init"
	GObjectClass *g_object_class G_GNUC_UNUSED = (GObjectClass*) c;

	g_type_class_add_private(c,sizeof(MakerDialogPrivate));

	parent_class = g_type_class_ref (GTK_TYPE_DIALOG);

	g_object_class->finalize = ___finalize;
}
#undef __GOB_FUNCTION__
static void 
maker_dialog_init (MakerDialog * self G_GNUC_UNUSED)
{
#define __GOB_FUNCTION__ "Maker:Dialog::init"
	self->_priv = G_TYPE_INSTANCE_GET_PRIVATE(self,MAKER_TYPE_DIALOG,MakerDialogPrivate);
	self->notebookWgt = NULL;
	self->_priv->containerTable =  g_hash_table_new_full(g_str_hash, g_str_equal, NULL, NULL) ;
	self->_priv->widgetTable =  g_hash_table_new_full(g_str_hash, g_str_equal, NULL, NULL) ;
	self->_priv->notebookContentTable =  g_hash_table_new_full(g_str_hash, g_str_equal, NULL, NULL) ;
	self->_priv->widgetIds =  g_string_chunk_new(MAKER_DIALOG_ID_LENGTH) ;
	self->_priv->pageLabels = NULL;
	self->_priv->properties = NULL;
 {

        /* initialize the object here */
    
 }
}
#undef __GOB_FUNCTION__



MakerDialog * 
maker_dialog_new (void)
{
#define __GOB_FUNCTION__ "Maker:Dialog::new"
{
	
        Self * self = GET_NEW; 
        return self;
    }}
#undef __GOB_FUNCTION__

MakerDialog * 
maker_dialog_new_full (MkdgProperties * properties, const gchar * title, MkdgWidgetFlag wFlags, MkdgButtonFlag bFlags)
{
#define __GOB_FUNCTION__ "Maker:Dialog::new_full"
{
	
        Self * self = GET_NEW; 
        GtkDialog * dialog = GTK_DIALOG(self);
        gtk_window_set_title(GTK_WINDOW(dialog), title);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);
        // gtk_window_set_type_hint(GTK_WINDOW(dialog), GDK_WINDOW_TYPE_HINT_DIALOG);
        self->wFlags = wFlags;
        self->_priv->properties = properties;

        gint i; 
        /* Create Buttons */
        for (i = MKDG_BUTTON_INDEX_INVALID -1; i >= 0; i--) {
            if ( bFlags & (1 << i)) {
                const gchar *buttonText = mkdg_button_get_text(i);
                mkdg_log(DEBUG, "new_full(-,%s,%X,%X) i=%d, buttonText=%s",
                    title, wFlags, bFlags, i, buttonText);
                MkdgButtonResponse response = mkdg_button_get_response(i);
                gtk_dialog_add_button(dialog, buttonText, response);
            }
        }

        /* Create Notebook */
        self->notebookWgt = gtk_notebook_new();
        // gtk_container_set_border_width(GTK_CONTAINER(self->notebookWgt), 5);
        // mkdg_wgt_show(self->notebookWgt);
        GtkWidget *dialogVboxWgt = gtk_dialog_get_content_area(GTK_DIALOG(self));
        // mkdg_wgt_show(dialogVboxWgt);
        gtk_box_prepend(GTK_BOX(dialogVboxWgt), self->notebookWgt);

        self_prepare(self);
        return self;
    }}
#undef __GOB_FUNCTION__

/**
 * maker_dialog_add_property:
 * @self: A MakerDialog
 * @ctx: A PropertyContext which contain the value to be set.
 * @returns: TRUE if the property adding succeed; FALSE otherwise.
 *
 * Add a property to the dialog and corresponding Gtk widget will also be
 * set.
 **/
gboolean 
maker_dialog_add_property (MakerDialog * self, PropertyContext * ctx)
{
#define __GOB_FUNCTION__ "Maker:Dialog::add_property"
	g_return_val_if_fail (self != NULL, (gboolean )0);
	g_return_val_if_fail (MAKER_IS_DIALOG (self), (gboolean )0);
{
	

        if (ctx == NULL) {
            return FALSE;
        }
        mkdg_log(INFO, "add_property(%s) %s", ctx->spec->key,
            property_context_to_string(ctx));

        /* Create the widget */
        MkdgWidget * mWidget = mkdg_widget_new(ctx, self->wFlags);
        if (mWidget == NULL)
            return FALSE;

        /* Find the container */
        const gchar *pageName = (STRING_IS_EMPTY(ctx->spec->pageName)) ? 
            MKDG_PAGE_NAME_DEFAULT : ctx->spec->pageName;
        MkdgWidgetContainer *container =
            MKDG_CONTAINER_TABLE_LOOKUP(self->_priv->containerTable, pageName);

        /* Do we need a new container/page? */
        if (container == NULL) {
            MkdgWgt * vboxWgt = gtk_box_new(GTK_ORIENTATION_VERTICAL, MKDG_VBOX_SPACING_DEFAULT);
            // gtk_container_set_border_width(GTK_CONTAINER (vboxWgt), 10);
            // mkdg_wgt_show(vboxWgt);
            container = mkdg_widget_container_new(vboxWgt);
            g_hash_table_insert(self->_priv->containerTable, (gpointer) pageName,
                                (gpointer) container);

            /* new notebook page */
            MkdgWgt * pageLabelWgt = gtk_label_new(_(pageName));
            // mkdg_wgt_show(pageLabelWgt);
            gtk_notebook_append_page(GTK_NOTEBOOK(self->notebookWgt), vboxWgt,
                                     pageLabelWgt);
        }

        /* Add the widget to container */
        mkdg_widget_container_add_widget(container, mWidget);
        g_hash_table_insert(self->_priv->widgetTable, (gpointer) ctx->spec->key,
                            (gpointer) mWidget);
        return TRUE;
    }}
#undef __GOB_FUNCTION__

/**
 * maker_dialog_add_all_properties:
 * @self: A MakerDialog
 * @returns: TRUE if all properties added successfully; FALSE otherwise.
 *
 * Add all properties to the dialog and corresponding Gtk widget will also be
 * set.
 *
 **/
gboolean 
maker_dialog_add_all_properties (MakerDialog * self)
{
#define __GOB_FUNCTION__ "Maker:Dialog::add_all_properties"
	g_return_val_if_fail (self != NULL, (gboolean )0);
	g_return_val_if_fail (MAKER_IS_DIALOG (self), (gboolean )0);
{
	
        gint i;
        gboolean result = TRUE;
        for(i = 0; i < mkdg_properties_size(self->_priv->properties); i++) {
            PropertyContext * ctx = mkdg_properties_index(self->_priv->properties, i);
            GValue * ret = property_context_load(ctx, NULL);
            if (ret == NULL)
                result = FALSE;
            self_add_property(self, ctx);
        }
        return result;
    }}
#undef __GOB_FUNCTION__

/**
 * maker_dialog_prepare:
 * @returns: TRUE if success; FALSE otherwise.
 *
 * Prepare the MakerDialog after new. This includes add_all_properties.
 **/
gboolean 
maker_dialog_prepare (MakerDialog * self)
{
#define __GOB_FUNCTION__ "Maker:Dialog::prepare"
	g_return_val_if_fail (self != NULL, (gboolean )0);
	g_return_val_if_fail (MAKER_IS_DIALOG (self), (gboolean )0);
{
	
        gboolean success;
        success = self_add_all_properties(self);
            if (!success) {
                return FALSE;
            }

        GList * containerList = g_hash_table_get_values(self->_priv->containerTable);
        GList * node;
        for(node = g_list_first(containerList); node != NULL; node = g_list_next(node)) {
            MkdgWidgetContainer *container = (MkdgWidgetContainer *) node->data;
            mkdg_widget_container_align_all_children(container, 1.0f, 0.5f, 0.0f, 0.5f);
        }
        g_list_free(containerList);
        return TRUE;
    }}
#undef __GOB_FUNCTION__

/**
 * maker_dialog_assign_widget_value:
 * @self: A MakerDialog.
 * @ctx: The PropertyContext
 * @userData: userData that pass through applyFunc.
 *
 * Assign the value shown in widget to the property.
 * That is firstly save the value, then apply the value.
 *
 * @see_also: set_widget_value()
 **/
void 
maker_dialog_assign_widget_value (MakerDialog * self, PropertyContext * ctx, gpointer userData)
{
#define __GOB_FUNCTION__ "Maker:Dialog::assign_widget_value"
	g_return_if_fail (self != NULL);
	g_return_if_fail (MAKER_IS_DIALOG (self));
{
	
        mkdg_log(INFO, "assign_widget_value(%s)", ctx->spec->key);
        GValue gValue = { 0 };
        mkdg_g_value_reset(&gValue, ctx->spec->valueType, TRUE);
        self_get_widget_value(self, ctx->spec->key, &gValue);
        property_context_save(ctx, &gValue, userData);
        property_context_apply(ctx, userData);
        g_value_unset(&gValue);
    }}
#undef __GOB_FUNCTION__

/**
 * maker_dialog_assign_all_widgets_values:
 * @self: A MakerDialog.
 * @userData: userData that pass through applyFunc.
 *
 * Invoke assign_widget_value for all widgets.
 *
 * @see_also: assign_widget_value(), set_widget_value()
 **/
void 
maker_dialog_assign_all_widgets_values (MakerDialog * self, gpointer userData)
{
#define __GOB_FUNCTION__ "Maker:Dialog::assign_all_widgets_values"
	g_return_if_fail (self != NULL);
	g_return_if_fail (MAKER_IS_DIALOG (self));
{
	
        gint i;
        for (i = 0; i < mkdg_properties_size(self->_priv->properties); i++) {
            PropertyContext *ctx = mkdg_properties_index(self->_priv->properties, i);
            self_assign_widget_value(self, ctx, userData);
        }
    }}
#undef __GOB_FUNCTION__

/**
 * maker_dialog_save_all_widgets_values:
 * @self: A MakerDialog.
 * @userData: userData that pass through property_context_save.
 *
 * Save all widget values to configuration backend.
 *
 **/
void 
maker_dialog_save_all_widgets_values (MakerDialog * self, gpointer userData)
{
#define __GOB_FUNCTION__ "Maker:Dialog::save_all_widgets_values"
	g_return_if_fail (self != NULL);
	g_return_if_fail (MAKER_IS_DIALOG (self));
{
	
        gint i;
        for (i = 0; i < mkdg_properties_size(self->_priv->properties); i++) {
            PropertyContext *ctx = mkdg_properties_index(self->_priv->properties,i);
            GValue gValue= { 0 };
            mkdg_g_value_reset(&gValue, ctx->spec->valueType, TRUE);
            self_get_widget_value(self, ctx->spec->key, &gValue);
            property_context_save(ctx, &gValue, userData);
            g_value_reset(&gValue);
        }
    }}
#undef __GOB_FUNCTION__

/**
 * maker_dialog_get_widget_value:
 * @self: A MakerDialog.
 * @key: The key of a property.
 * @value: GValue that store output.
 * @returns: The address of value if success, NULL if failed.
 *
 * Get value as display in widget.
 **/
GValue * 
maker_dialog_get_widget_value (MakerDialog * self, const gchar * key, GValue * value)
{
#define __GOB_FUNCTION__ "Maker:Dialog::get_widget_value"
	g_return_val_if_fail (self != NULL, (GValue * )0);
	g_return_val_if_fail (MAKER_IS_DIALOG (self), (GValue * )0);
{
	
        mkdg_log(INFO, "get_widget_value(%s)", key);
        MkdgWidget * mWidget = self_get_widget_by_key(self, key);
        if (mWidget == NULL) {
            /* Not fount */
            return NULL;
        }
        return mkdg_widget_get_widget_value(mWidget, value);
    }}
#undef __GOB_FUNCTION__

/**
 * maker_dialog_set_widget_value:
 * @self: A MakerDialog.
 * @key: The key of a property.
 * @value: Value to be shown in widget.
 *
 * Set value for widget display.
 * However, value in PropertyContext is not changed.
 * Use assign_widget_value() to do it.
 * @see_also: assign_widget_value()
 **/
void 
maker_dialog_set_widget_value (MakerDialog * self, const gchar * key, GValue * value)
{
#define __GOB_FUNCTION__ "Maker:Dialog::set_widget_value"
	g_return_if_fail (self != NULL);
	g_return_if_fail (MAKER_IS_DIALOG (self));
{
	
        mkdg_log(INFO, "set_widget_value(%s, %s)", key, mkdg_g_value_to_string(value));
        MkdgWidget * mWidget = self_get_widget_by_key(self, key);
        mkdg_widget_set_widget_value(mWidget, value);
    }}
#undef __GOB_FUNCTION__

// /**
//  * maker_dialog_show:
//  * @self: A MakerDialog.
//  *
//  **/
// void 
// maker_dialog_show (MakerDialog * self)
// {
// #define __GOB_FUNCTION__ "Maker:Dialog::show"
// 	g_return_if_fail (self != NULL);
// 	g_return_if_fail (MAKER_IS_DIALOG (self));
// {
	
//         mkdg_wgt_show_all(GTK_WIDGET(self));
//     }}
// #undef __GOB_FUNCTION__

MkdgWidget * 
maker_dialog_get_widget_by_key (MakerDialog * self, const gchar * key)
{
#define __GOB_FUNCTION__ "Maker:Dialog::get_widget_by_key"
	g_return_val_if_fail (self != NULL, (MkdgWidget * )0);
	g_return_val_if_fail (MAKER_IS_DIALOG (self), (MkdgWidget * )0);
{
	
        return (MkdgWidget *) g_hash_table_lookup(self->_priv->widgetTable, key);
    }}
#undef __GOB_FUNCTION__
