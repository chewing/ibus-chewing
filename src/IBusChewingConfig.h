#ifndef _IBUS_CHEWING_CONFIG_H_
#define _IBUS_CHEWING_CONFIG_H_
#include <glib.h>
#include <ibus.h>
#include "MakerDialogProperty.h"
#include "MakerDialogBackend.h"

typedef struct {
    MkdgBackend *backend;
    MkdgProperties *properties;
} IBusChewingConfig;

/* GConf/dconf section except "/desktop/ibus" */

#define IBUS_CHEWING_CONFIG_SUBSECTION "engine/Chewing"

IBusChewingConfig *ibus_chewing_config_new(MkdgBackend * backend,
	gpointer parent,
	gpointer auxData);

GValue *ibus_chewing_config_load_ibus_config(IBusChewingConfig * self,
					     GValue * value,
					     const gchar * section,
					     const gchar * key,
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

extern const gchar *kbType_ids[];

extern const gchar *selKeys_array[];

extern const gchar *syncCapsLock_strs[];

extern const gchar *outputCharsets[];

#endif				/* _IBUS_CHEWING_CONFIG_H_ */
