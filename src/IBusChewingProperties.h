#ifndef _IBUS_CHEWING_PROPERTIES_H_
#define _IBUS_CHEWING_PROPERTIES_H_
#include <glib.h>
#include <ibus.h>
#include "MakerDialogProperty.h"
#include "MakerDialogBackend.h"

typedef struct {
    MkdgProperties *properties;
    GHashTable *confObjTable;
} IBusChewingProperties;

/* GConf/dconf section under PROJECT_SCHEMA_BASE */
#define IBUS_CHEWING_PROPERTIES_SUBSECTION QUOTE_ME(PROJECT_SCHEMA_SECTION)

IBusChewingProperties *ibus_chewing_properties_new(MkdgBackend * backend,
						   gpointer parent,
						   gpointer auxData);

GValue *ibus_chewing_properties_read_general(IBusChewingProperties * self,
					     GValue * value,
					     const gchar * section,
					     const gchar * key,
					     gpointer userData);

gboolean ibus_chewing_properties_read_boolean_general(IBusChewingProperties
						      * self,
						      const gchar *
						      section,
						      const gchar * key,
						      gpointer userData);

gint ibus_chewing_properties_read_int_general(IBusChewingProperties * self,
					      const gchar * section,
					      const gchar * key,
					      gpointer userData);

/*============================================
 * Callback functions
 */
gboolean KBType_apply_callback(PropertyContext * ctx, gpointer userData);

gboolean selKeys_apply_callback(PropertyContext * ctx, gpointer userData);

gboolean hsuSelKeyType_apply_callback(PropertyContext * ctx,
				      gpointer userData);

gboolean showSystray_apply_callback(PropertyContext * ctx,
				    gpointer userData);

gboolean autoShiftCur_apply_callback(PropertyContext * ctx,
				     gpointer userData);

gboolean addPhraseDirection_apply_callback(PropertyContext * ctx,
					   gpointer userData);

gboolean cleanBufferFocusOut_apply_callback(PropertyContext * ctx,
					    gpointer userData);

gboolean easySymbolInput_apply_callback(PropertyContext * ctx,
					gpointer userData);

gboolean escCleanAllBuf_apply_callback(PropertyContext * ctx,
				       gpointer userData);

gboolean maxChiSymbolLen_apply_callback(PropertyContext * ctx,
					gpointer userData);

gboolean forceLowercaseEnglish_apply_callback(PropertyContext * ctx,
					      gpointer userData);

gboolean syncCapsLock_apply_callback(PropertyContext * ctx,
				     gpointer userData);

gboolean numpadAlwaysNumber_apply_callback(PropertyContext * ctx,
					   gpointer userData);

gboolean candPerPage_apply_callback(PropertyContext * ctx,
				    gpointer userData);

gboolean phraseChoiceRearward_apply_callback(PropertyContext * ctx,
					     gpointer userData);

gboolean spaceAsSelection_apply_callback(PropertyContext * ctx,
					 gpointer userData);

gboolean plainZhuyin_apply_callback(PropertyContext * ctx,
				    gpointer userData);

extern MkdgPropertySpec propSpecs[];

extern const gchar *kbType_ids[];

extern const gchar *selKeys_array[];

extern const gchar *syncCapsLock_strs[];

extern const gchar *outputCharsets[];

#endif				/* _IBUS_CHEWING_PROPERTIES_H_ */
