#include "IBusChewingSystray.h"

/*=== Internal functions start ===*/

void ibus_chewing_systray_left_click_callback_interface(GtkStatusIcon *
							icon,
							gpointer userData)
{
    IBusChewingSystrayIcon *systrayIcon =
	(IBusChewingSystrayIcon *) userData;
    systrayIcon->leftClickFunc(systrayIcon, systrayIcon->leftClickData);
}

void ibus_chewing_systray_right_click_callback_interface(GtkStatusIcon *
							 icon,
							 GdkEvent * event,
							 gpointer userData)
{
    IBusChewingSystrayIcon *systrayIcon =
	(IBusChewingSystrayIcon *) userData;
    if (event->type == GDK_BUTTON_RELEASE) {
	if (((GdkEventButton *) event)->button == 3) {
	    /* Right Click release */
	    systrayIcon->rightClickFunc(systrayIcon,
					systrayIcon->rightClickData);
	}
    }
}

/*=== Member functions start ===*/
IBusChewingSystrayIcon
    * ibus_chewing_systray_icon_new(guint defaultValue,
				    IBusChewingSystrayClickFunc
				    leftClickFunc,
				    gpointer leftClickData,
				    IBusChewingSystrayClickFunc
				    rightClickFunc,
				    gpointer rightClickData, ...)
{
    va_list argList;
    const gchar *iconFile;
    GPtrArray *fileArray = g_ptr_array_new();
    va_start(argList, rightClickData);
    for (gint i = 0; i < IBUS_CHEWING_SYSTRAY_ICON_COUNT_MAX; i++) {
	iconFile = va_arg(argList, const gchar *);
	if (iconFile == NULL) {
	    break;
	}
	g_ptr_array_add(fileArray, (gpointer) iconFile);

    }
    va_end(argList);

    IBusChewingSystrayIcon *self = g_new0(IBusChewingSystrayIcon, 1);
    self->iconFileArray = fileArray;
    self->icon = gtk_status_icon_new();
    self->leftClickFunc = leftClickFunc;
    self->leftClickData = leftClickData;
    if (leftClickFunc != NULL) {
	g_signal_connect(G_OBJECT(self->icon), "activate",
			 G_CALLBACK
			 (ibus_chewing_systray_left_click_callback_interface),
			 self);
    }
    self->rightClickFunc = rightClickFunc;
    self->rightClickData = rightClickData;
    if (rightClickFunc != NULL) {
	g_signal_connect(G_OBJECT(self->icon), "button-release-event",
			 G_CALLBACK
			 (ibus_chewing_systray_right_click_callback_interface),
			 self);
    }
    ibus_chewing_systray_icon_set_value(self, defaultValue);
    gtk_status_icon_set_visible(self->icon, TRUE);
    return self;
}

void ibus_chewing_systray_icon_free(IBusChewingSystrayIcon * self)
{
    gtk_status_icon_set_visible(self->icon, FALSE);
    g_object_unref(self->icon);
    g_ptr_array_free(self->iconFileArray, TRUE);
    g_free(self);
}

void ibus_chewing_systray_icon_set_value(IBusChewingSystrayIcon *
					 self, guint value)
{
    self->value = value;
}

void ibus_chewing_systray_icon_set_visible(IBusChewingSystrayIcon *
					   self, gboolean visible)
{
    gtk_status_icon_set_visible(self->icon, visible);
}


void ibus_chewing_systray_icon_update(IBusChewingSystrayIcon * self)
{
    gtk_status_icon_set_from_file(self->icon,
				  ibus_chewing_systray_icon_get_icon_file
				  (self, self->value));
    ibus_chewing_systray_icon_set_visible(self, TRUE);
}


/*=== Chi_Eng systray Icon ===*/
void ibus_chewing_systray_chi_eng_refresh_value(IBusChewingSystrayIcon *
						self,
						IBusChewingPreEdit *
						icPreEdit)
{
    if (ibus_chewing_pre_edit_get_chi_eng_mode(icPreEdit)) {
	mkdg_set_flag(self->value, IBUS_CHEWING_SYSTRAY_CHINESE_FLAG);
    } else {
	mkdg_clear_flag(self->value, IBUS_CHEWING_SYSTRAY_CHINESE_FLAG);
    }

    if (ibus_chewing_pre_edit_get_full_half(icPreEdit)) {
	mkdg_set_flag(self->value,
		      IBUS_CHEWING_SYSTRAY_FULL_HALF_SHAPE_FLAG);
    } else {
	mkdg_clear_flag(self->value,
			IBUS_CHEWING_SYSTRAY_FULL_HALF_SHAPE_FLAG);
    }
    ibus_chewing_systray_icon_update(self);
}

/**
 * ibus_chewing_systray_chi_eng_toggle_callback:
 */
void ibus_chewing_systray_chi_eng_toggle_callback(IBusChewingSystrayIcon *
						  self,
						  gpointer icPreEditPtr)
{
    IBusChewingPreEdit *icPreEdit = (IBusChewingPreEdit *) icPreEditPtr;
    ibus_chewing_pre_edit_toggle_chi_eng_mode(icPreEdit);
    ibus_chewing_systray_chi_eng_refresh_value(self, icPreEdit);
}

/**
 * ibus_chewing_systray_full_half_toggle_callback:
 */
void ibus_chewing_systray_full_half_toggle_callback(IBusChewingSystrayIcon
						    * self,
						    gpointer icPreEditPtr)
{
    IBusChewingPreEdit *icPreEdit = (IBusChewingPreEdit *) icPreEditPtr;
    ibus_chewing_pre_edit_toggle_full_half(icPreEdit);
    ibus_chewing_systray_chi_eng_refresh_value(self, icPreEdit);
}

IBusChewingSystrayIcon
    * ibus_chewing_systray_chi_eng_icon_new(IBusChewingPreEdit * icPreEdit)
{
    IBusChewingSystrayIcon *iChiEngSystrayIcon =
	ibus_chewing_systray_icon_new(1,
				      ibus_chewing_systray_chi_eng_toggle_callback,
				      (gpointer)
				      icPreEdit,
				      ibus_chewing_systray_full_half_toggle_callback,
				      icPreEdit,
				      QUOTE_ME(PRJ_ICON_DIR)
				      "/ibus-chewing-eng-half.svg",
				      QUOTE_ME(PRJ_ICON_DIR)
				      "/ibus-chewing-chi-half.svg",
				      QUOTE_ME(PRJ_ICON_DIR)
				      "/ibus-chewing-eng-full.svg",
				      QUOTE_ME(PRJ_ICON_DIR)
				      "/ibus-chewing-chi-full.svg",
				      NULL);
    gtk_status_icon_set_tooltip_text(iChiEngSystrayIcon->icon,
				     _
				     ("Left click to toggle Chi / Eng; Right click to toggle half / full shape"));
    return iChiEngSystrayIcon;
}
