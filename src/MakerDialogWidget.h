#ifndef _MAKER_DIALOG_WIDGET_H_
#    define _MAKER_DIALOG_WIDGET_H_
#    include <gtk/gtk.h>
#    include "MakerDialogProperty.h"

#    ifndef MKDG_HBOX_SPACING_DEFAULT
#        define MKDG_HBOX_SPACING_DEFAULT 2
#    endif

#    ifndef MKDG_VBOX_SPACING_DEFAULT
#        define MKDG_VBOX_SPACING_DEFAULT 2
#    endif

typedef enum {
    MKDG_WIDGET_FLAG_SET_IMMEDIATELY = 1,
    MKDG_WIDGET_FLAG_WRITE_IMMEDIATELY = 1 << 1,
    MKDG_WIDGET_FLAG_APPLY_IMMEDIATELY = 1 << 2
} MkdgWidgetFlag;

typedef enum {
    MKDG_BUTTON_INDEX_CANCEL,
    MKDG_BUTTON_INDEX_SAVE,
    MKDG_BUTTON_INDEX_APPLY,
    MKDG_BUTTON_INDEX_CLOSE,
    MKDG_BUTTON_INDEX_OK,
    MKDG_BUTTON_INDEX_INVALID,
} MkdgButtonIndex;

/**
 * MkdgButtonFlag:
 * @MKDG_BUTTON_FLAG_CANCEL: Close dialog.
 * @MKDG_BUTTON_FLAG_SAVE: Save.
 * @MKDG_BUTTON_FLAG_APPLY: Apply.
 * @MKDG_BUTTON_FLAG_OK: Save, Apply and Close dialog.
 *
 * Buttons of this dialog.
 */

typedef enum {
    MKDG_BUTTON_FLAG_CANCEL = 1 << MKDG_BUTTON_INDEX_CANCEL,
    MKDG_BUTTON_FLAG_SAVE = 1 << MKDG_BUTTON_INDEX_SAVE,
    MKDG_BUTTON_FLAG_APPLY = 1 << MKDG_BUTTON_INDEX_APPLY,
    MKDG_BUTTON_FLAG_CLOSE = 1 << MKDG_BUTTON_INDEX_CLOSE,
    MKDG_BUTTON_FLAG_OK = 1 << MKDG_BUTTON_INDEX_OK,
    MKDG_BUTTON_FLAG_INVALID = 1 << MKDG_BUTTON_INDEX_INVALID,
} MkdgButtonFlag;

typedef enum {
    MKDG_BUTTON_RESPONSE_CANCEL = GTK_RESPONSE_CANCEL,
    MKDG_BUTTON_RESPONSE_SAVE = MKDG_BUTTON_INDEX_SAVE,
    MKDG_BUTTON_RESPONSE_APPLY = GTK_RESPONSE_APPLY,
    MKDG_BUTTON_RESPONSE_CLOSE = GTK_RESPONSE_CLOSE,
    MKDG_BUTTON_RESPONSE_OK = GTK_RESPONSE_OK,
    MKDG_BUTTON_RESPONSE_INVALID = MKDG_BUTTON_INDEX_INVALID
} MkdgButtonResponse;

MkdgButtonResponse mkdg_button_get_response(MkdgButtonIndex index);
const gchar *mkdg_button_get_text(MkdgButtonIndex index);

/**
 * MkdgWdt:
 * 
 * The actual widget from the ToolKit.
 */
typedef GtkWidget MkdgWgt;

/**
 * MkdgWdtLabel:
 * 
 * The actual label widget from the ToolKit.
 */
typedef GtkLabel MkdgWgtLabel;

/**
 * MKDG_WGT:
 * @obj: Object to be casted.
 * 
 * Cast the object as MkdgWgt.
 */
#    define MKDG_WGT(obj) GTK_WIDGET(obj)

gint mkdg_wgt_get_width(MkdgWgt * wgt);

void mkdg_wgt_set_width(MkdgWgt * wgt, gint width);

void mkdg_wgt_set_alignment(MkdgWgt * wgt, gfloat xAlign, gfloat yAlign);

void mkdg_wgt_show(MkdgWgt * wgt);

void mkdg_wgt_show_all(MkdgWgt * wgt);

void mkdg_wgt_destroy(MkdgWgt * wgt);

/*=====================================
 * MkdgWidgetContainer subroutines
 */
typedef struct {
    MkdgWgt *wgt;
    GPtrArray *children;
    gint childrenLabelWidth;
} MkdgWidgetContainer;

typedef struct {
    MkdgWgtLabel *label;
    MkdgWgt *wgt;
    PropertyContext *ctx;
    MkdgWidgetFlag flags;
    MkdgWidgetContainer *container;
} MkdgWidget;

MkdgWidgetContainer *mkdg_widget_container_new(MkdgWgt * wgt);


void mkdg_widget_container_add_widget(MkdgWidgetContainer * container,
                                      MkdgWidget * mWidget);

void mkdg_widget_container_align_all_children(MkdgWidgetContainer *
                                              container,
                                              gfloat labelXAlign,
                                              gfloat labelYAlign,
                                              gfloat wgtXAlign,
                                              gfloat wgtYAlign);

gint mkdg_widget_container_count_children(MkdgWidgetContainer * container);

void mkdg_widget_cointainer_free(MkdgWidgetContainer * container);


MkdgWidget *mkdg_widget_new(PropertyContext * ctx, MkdgWidgetFlag widgetFlags);


/**
 * mkdg_widget_get_widget_value:
 * @mWidget: Widget to be get.
 * @value:  Result value stores here.
 * @returns: NULL if failed, otherwise return the address of value.
 *
 * Get value from display widget.
 * This subroutine will not touch the property context. Use property_context_get() for that.
 * @see_also: property_context_get(), mkdg_widget_set_widget_value()
 * @returns: TRUE if success, FALSE if either mWidget or it's members are NULL
 */
GValue *mkdg_widget_get_widget_value(MkdgWidget * mWidget, GValue * value);

/**
 * mkdg_widget_set_widget_value:
 * @mWidget: Widget to be set.
 * @value:  Set Widget with this value.
 *
 * Set widget value for display.
 * This subroutine will not touch the property context. Use property_context_set() for that.
 * @see_also: property_context_set(), mkdg_widget_get_widget_value()
 * @returns: TRUE if success, FALSE if either mWidget or it's members are NULL
 */
gboolean mkdg_widget_set_widget_value(MkdgWidget * mWidget, GValue * value);

#endif                          /* _MAKER_DIALOG_WIDGET_H_ */
