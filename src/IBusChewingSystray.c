#include "IBusChewingSystray.h"

/*=== Internal functions start ===*/

void ibus_chewing_systray_left_click_callback_interface(GtkStatusIcon *
                                                        icon, gpointer userData)
{
    IBusChewingSystrayIcon *systrayIcon = (IBusChewingSystrayIcon *) userData;

    systrayIcon->leftClickFunc(systrayIcon, systrayIcon->leftClickData);
}

void ibus_chewing_systray_right_click_callback_interface(GtkStatusIcon *
                                                         icon,
                                                         GdkEvent * event,
                                                         gpointer userData)
{
    IBusChewingSystrayIcon *systrayIcon = (IBusChewingSystrayIcon *) userData;

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
    IBUS_CHEWING_LOG(DEBUG, "* ibus_chewing_systray_icon_new()");
    va_list argList;
    const gchar *iconFile;

    IBusChewingSystrayIcon *self = g_new0(IBusChewingSystrayIcon, 1);

    self->icon = gtk_status_icon_new();
    self->iconFileArray = g_ptr_array_new();
    self->iconCacheArray = g_ptr_array_new();

    va_start(argList, rightClickData);
    for (gint i = 0; i < IBUS_CHEWING_SYSTRAY_ICON_COUNT_MAX; i++) {
        iconFile = va_arg(argList, const gchar *);

        if (iconFile == NULL) {
            break;
        }
        gtk_status_icon_set_from_file(self->icon, iconFile);
        GdkPixbuf *pbuf = gtk_status_icon_get_pixbuf(self->icon);
        GVariant *cacheGVar = g_icon_serialize(G_ICON(pbuf));

        g_ptr_array_add(self->iconFileArray, (gpointer) iconFile);
        g_ptr_array_add(self->iconCacheArray, (gpointer) cacheGVar);
    }
    va_end(argList);

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
    g_ptr_array_free(self->iconCacheArray, TRUE);
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
    GVariant *cacheGVar =
        (GVariant *) g_ptr_array_index(self->iconCacheArray, self->value);
    GIcon *cachedIcon = g_icon_deserialize(g_variant_ref_sink(cacheGVar));

    g_variant_unref(cacheGVar);
    gtk_status_icon_set_from_gicon(self->icon, cachedIcon);
    ibus_chewing_systray_icon_set_visible(self, TRUE);
}

/*=== Chi_Eng systray Icon ===*/
/**
 * ibus_chewing_systray_chi_eng_toggle_callback:
 */
void ibus_chewing_systray_chi_eng_toggle_callback(IBusChewingSystrayIcon *
                                                  self, gpointer userData)
{
    IBusChewingEngine *iEngine = (IBusChewingEngine *) userData;

    ibus_chewing_pre_edit_toggle_chi_eng_mode(iEngine->icPreEdit);
    ibus_chewing_systray_chi_eng_icon_refresh_value(iEngine);
    ibus_chewing_engine_refresh_property_list(iEngine);
}

/**
 * ibus_chewing_systray_full_half_toggle_callback:
 */
void ibus_chewing_systray_full_half_toggle_callback(IBusChewingSystrayIcon
                                                    * self, gpointer userData)
{
    IBusChewingEngine *iEngine = (IBusChewingEngine *) userData;

    ibus_chewing_pre_edit_toggle_full_half_mode(iEngine->icPreEdit);
    ibus_chewing_systray_chi_eng_icon_refresh_value(iEngine);
    ibus_chewing_engine_refresh_property_list(iEngine);
}

IBusChewingSystrayIcon
    * ibus_chewing_systray_chi_eng_icon_new(IBusChewingEngine * iEngine)
{
    IBusChewingSystrayIcon *iChiEngSystrayIcon =
        ibus_chewing_systray_icon_new(1,
                                      ibus_chewing_systray_chi_eng_toggle_callback,
                                      (gpointer)
                                      iEngine,
                                      ibus_chewing_systray_full_half_toggle_callback,
                                      iEngine,
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
                                     ("Left click to toggle Chi/Eng; Right click to toggle full/half shape"));
    return iChiEngSystrayIcon;
}

gboolean
ibus_chewing_systray_chi_eng_icon_create_or_hide(IBusChewingEngine * iEngine)
{
    if (ibus_chewing_pre_edit_get_property_boolean
        (iEngine->icPreEdit, "show-systray")) {
        if (iEngine->iChiEngSystrayIcon == NULL) {
            iEngine->iChiEngSystrayIcon =
                ibus_chewing_systray_chi_eng_icon_new(iEngine);
        }
        ibus_chewing_systray_icon_set_visible(iEngine->iChiEngSystrayIcon,
                                              TRUE);
        return TRUE;
    }
    if (iEngine->iChiEngSystrayIcon != NULL) {
        ibus_chewing_systray_icon_set_visible(iEngine->iChiEngSystrayIcon,
                                              FALSE);
    }
    return FALSE;
}

void ibus_chewing_systray_chi_eng_icon_refresh_value(IBusChewingEngine *
                                                     iEngine)
{
    if (ibus_chewing_pre_edit_get_chi_eng_mode(iEngine->icPreEdit)) {
        mkdg_set_flag(iEngine->iChiEngSystrayIcon->value,
                      IBUS_CHEWING_SYSTRAY_CHINESE_FLAG);
    } else {
        mkdg_clear_flag(iEngine->iChiEngSystrayIcon->value,
                        IBUS_CHEWING_SYSTRAY_CHINESE_FLAG);
    }

    if (ibus_chewing_pre_edit_get_full_half_mode(iEngine->icPreEdit)) {
        mkdg_set_flag(iEngine->iChiEngSystrayIcon->value,
                      IBUS_CHEWING_SYSTRAY_FULL_HALF_SHAPE_FLAG);
    } else {
        mkdg_clear_flag(iEngine->iChiEngSystrayIcon->value,
                        IBUS_CHEWING_SYSTRAY_FULL_HALF_SHAPE_FLAG);
    }
    ibus_chewing_systray_icon_update(iEngine->iChiEngSystrayIcon);
}
