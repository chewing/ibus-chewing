#ifndef _IBUS_CHEWING_CONFIG_H_
#define _IBUS_CHEWING_CONFIG_H_
#include <glib.h>
#include <ibus.h>
#include "MakerDialogProperty.h"

typedef struct {
    IBusConfig *config;
} IBusChewingConfig;

/* GConf/dconf section except "/desktop/ibus" */

#define IBUS_CHEWING_CONFIG_SECTION "engine/Chewing"

IBusChewingConfig *IBusChewingConfig_new(IBusService * service);

void IBusChewingConfig_load(IBusChewingConfig * self);

gboolean IBusChewingConfig_get_value(IBusChewingConfig * self,
				     const gchar * key, GValue * gValue);

gboolean IBusChewingConfig_set_value(IBusChewingConfig * self,
				     const gchar * key, GValue * gValue);

PropertySpec *IBusChewingConfig_find_key(const gchar * key);

gboolean IBusChewingConfig_foreach_properties(gboolean stopOnError,
					      CallbackBoolFunc callback,
					      gpointer ctxData,
					      gpointer userData);

#endif				/* _IBUS_CHEWING_CONFIG_H_ */
