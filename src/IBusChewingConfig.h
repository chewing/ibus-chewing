#ifndef _IBUS_CHEWING_CONFIG_H_
#define _IBUS_CHEWING_CONFIG_H_
#include <glib.h>
#include <ibus.h>
#include "MakerDialogProperty.h"
#include "MakerDialogBackend.h"

typedef struct {
    IBusConfig *iConfig;
    MkdgBackend *backend;
    MkdgProperties *properties;
} IBusChewingConfig;

/* GConf/dconf section except "/desktop/ibus" */

#define IBUS_CHEWING_CONFIG_SUBSECTION "engine/Chewing"

IBusChewingConfig *ibus_chewing_config_new(IBusService * service,
	MkdgBackend * backend,
	gpointer parent,
	gpointer auxData);

void IBusChewingConfig_load(IBusChewingConfig * self);

gboolean IBusChewingConfig_get_value(IBusChewingConfig * self,
				     const gchar * key, GValue * gValue);


gboolean IBusChewingConfig_get_ibus_value(IBusChewingConfig * self,
					  const gchar * section,
					  const gchar * key,
					  GValue * gValue);

gboolean IBusChewingConfig_set_value(IBusChewingConfig * self,
				     const gchar * key, GValue * gValue);

PropertySpec *IBusChewingConfig_find_key(const gchar * key);

MkdgProperties
    * IBusChewingConfig_get_MkdgProperties(IBusChewingConfig * self);

gboolean IBusChewingConfig_foreach_properties(gboolean stopOnError,
					      CallbackBoolFunc callback,
					      gpointer ctxData,
					      gpointer userData);

/*============================================
 * Callback functions
 */
gboolean KBType_apply_callback(PropertyContext * ctx, GValue * value);

gboolean selKeys_apply_callback(PropertyContext * ctx, GValue * value);

gboolean hsuSelKeyType_apply_callback(PropertyContext * ctx,
				      GValue * value);

gboolean autoShiftCur_apply_callback(PropertyContext * ctx,
				     GValue * value);

gboolean addPhraseDirection_apply_callback(PropertyContext * ctx,
					   GValue * value);

gboolean easySymbolInput_apply_callback(PropertyContext * ctx,
					GValue * value);

gboolean escCleanAllBuf_apply_callback(PropertyContext * ctx,
				       GValue * value);

gboolean maxChiSymbolLen_apply_callback(PropertyContext * ctx,
					GValue * value);

gboolean forceLowercaseEnglish_apply_callback(PropertyContext * ctx,
					      GValue * value);

gboolean syncCapsLock_apply_callback(PropertyContext * ctx,
				     GValue * value);

gboolean numpadAlwaysNumber_apply_callback(PropertyContext * ctx,
					   GValue * value);

gboolean candPerPage_apply_callback(PropertyContext * ctx, GValue * value);

gboolean phraseChoiceRearward_apply_callback(PropertyContext * ctx,
					     GValue * value);

gboolean spaceAsSelection_apply_callback(PropertyContext * ctx,
					 GValue * value);

gboolean plainZhuyin_apply_callback(PropertyContext * ctx, GValue * value);

#endif				/* _IBUS_CHEWING_CONFIG_H_ */
