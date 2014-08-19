#include <glib.h>
#include <ibus.h>
#include "maker-dialog.h"
#ifndef _IBUS_CHEWING_CONFIG_H_
#define _IBUS_CHEWING_CONFIG_H_

typedef struct {
    IBusConfig *config;
    MakerDialog *settingDialog;
} IBusChewingConfig;

/* GConf/dconf section except "/desktop/ibus" */

#define IBUS_CHEWING_CONFIG_SECTION "engine/Chewing"

IBusChewingConfig *IBusChewingConfig_new(IBusService * service);

void IBusChewingConfig_load(IBusChewingConfig * self);

void IBusChewingConfig_set_dialog(IBusChewingConfig * self,
				  MakerDialog * settingDialog);

gboolean IBusChewingConfig_get_value(IBusChewingConfig * self,
				     const gchar * key, GValue * gValue);

gboolean IBusChewingConfig_set_value(IBusChewingConfig * self,
				     const gchar * key, GValue * gValue);

PropertySpec *IBusChewingConfig_find_key(const gchar * key);

gboolean IBusChewingConfig_foreach_properties(gboolean stopOnError,
					      CallbackBoolFunc callback,
					      gpointer userData);

#endif				/* _IBUS_CHEWING_CONFIG_H_ */
