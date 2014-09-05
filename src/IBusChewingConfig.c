#define GETTEXT_PACKAGE "gtk20"
#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include "MakerDialogUtil.h"
#include "ibus-chewing-util.h"
#include "IBusChewingConfig.h"
#include "IBusConfigBackend.h"

PropertySpec propSpecs[] = {
    {G_TYPE_STRING, "KBType", "Keyboard", N_("Keyboard Type"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "default", kbType_ids, NULL, 0, 0,
     KBType_apply_callback,
     MAKER_DIALOG_PROPERTY_FLAG_INEDITABLE |
     MAKER_DIALOG_PROPERTY_FLAG_HAS_TRANSLATION,
     N_("Select Zhuyin keyboard layout."),
     }
    ,
    {G_TYPE_STRING, "selKeys", "Keyboard", N_("Selection keys"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "1234567890", selKeys_array, NULL, 0,
     0,
     selKeys_apply_callback, 0,
     N_
     ("Keys used to select candidate. For example \"asdfghjkl;\", press 'a' to select the 1st candidate, 's' for 2nd, and so on."),
     }
    ,
    {G_TYPE_INT, "hsuSelKeyType", "Keyboard", N_("Hsu's selection key"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "1", NULL, NULL, 1, 2,
     hsuSelKeyType_apply_callback, 0,
     N_
     ("Hsu's keyboard selection keys, 1 for asdfjkl789, 2 for asdfzxcv89 ."),
     }
    ,

    {G_TYPE_BOOLEAN, "autoShiftCur", "Editing", N_("Auto move cursor"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "1", NULL, NULL, 0, 1,
     autoShiftCur_apply_callback, 0,
     N_("Automatically move cursor to next character."),
     }
    ,
    {G_TYPE_BOOLEAN, "addPhraseDirection", "Editing",
     N_("Add phrases in front"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "1", NULL, NULL, 0, 1,
     addPhraseDirection_apply_callback, 0,
     N_("Add phrases in the front."),
     }
    ,
    {G_TYPE_BOOLEAN, "easySymbolInput", "Editing", N_("Easy symbol input"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "1", NULL, NULL, 0, 1,
     easySymbolInput_apply_callback, 0,
     N_("Easy symbol input."),
     }
    ,
    {G_TYPE_BOOLEAN, "escCleanAllBuf", "Editing",
     N_("Esc clean all buffer"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "0", NULL, NULL, 0, 1,
     escCleanAllBuf_apply_callback, 0,
     N_("Escape key cleans the text in pre-edit-buffer."),
     }
    ,
    {G_TYPE_INT, "maxChiSymbolLen", "Editing",
     N_("Maximum Chinese characters"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "20", NULL, NULL, 8, 50,
     maxChiSymbolLen_apply_callback, 0,
     N_
     ("Maximum Chinese characters in pre-edit buffer, including inputing Zhuyin symbols"),
     }
    ,
    {G_TYPE_BOOLEAN, "forceLowercaseEnglish", "Editing",
     N_("Force lowercase in En mode"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "0", NULL, NULL, 0, 1,
     forceLowercaseEnglish_apply_callback, 0,
     N_("Ignore CapsLock status and input lowercase by default.\n\
		It is handy if you wish to enter lowercase by default. Uppercase can still be inputted with Shift."),
     }
    ,

    /* Sync between CapsLock and IM */
    {G_TYPE_STRING, "syncCapsLock", "Editing",
     N_("Sync between CapsLock and IM"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "disable", syncCapsLock_strs, "Sync",
     0, 1,
     syncCapsLock_apply_callback,
     MAKER_DIALOG_PROPERTY_FLAG_INEDITABLE |
     MAKER_DIALOG_PROPERTY_FLAG_HAS_TRANSLATION |
     MAKER_DIALOG_PROPERTY_FLAG_TRANSLATION_WITH_CONTEXT,
     N_("Occasionally, the CapsLock status does not match the IM, \
		this option determines how these status be synchronized. Valid values:\n\
		\"disable\": Do nothing.\n\
		\"keyboard\": IM status follows keyboard status.\n\
		\"IM\": Keyboard status follows IM status."),
     }
    ,

    {G_TYPE_BOOLEAN, "numpadAlwaysNumber", "Editing",
     N_("Number pad always input number"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "1", NULL, NULL, 0, 1,
     numpadAlwaysNumber_apply_callback, 0,
     N_("Always input numbers when number keys from key pad is inputted."),
     }
    ,
    {G_TYPE_BOOLEAN, "plainZhuyin", "Selecting", N_("Plain Zhuyin mode"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "0", NULL, NULL, 0, 1,
     plainZhuyin_apply_callback, 0,
     N_
     ("In plain Zhuyin mode, automatic candidate selection and related options are disabled or ignored."),
     }
    ,
    {G_TYPE_INT, "candPerPage", "Selecting", N_("Candidate per page"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "10", NULL, NULL, 8, 10,
     candPerPage_apply_callback, 0,
     N_("Number of candidate per page."),
     }
    ,
    {G_TYPE_BOOLEAN, "phraseChoiceRearward", "Selecting",
     N_("Choose phrases from backward"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "1", NULL, NULL, 0, 1,
     phraseChoiceRearward_apply_callback, 0,
     N_("Choose phrases from the back, without moving cursor."),
     }
    ,
    {G_TYPE_BOOLEAN, "spaceAsSelection", "Selecting",
     IBUS_CHEWING_CONFIG_SUBSECTION, N_("Space to select"),
     "0", NULL, NULL, 0, 1,
     spaceAsSelection_apply_callback, 0,
     "Press Space to select the candidate.",
     }
    ,

    {G_TYPE_INVALID, "", "", "",
     IBUS_CHEWING_CONFIG_SUBSECTION, "", NULL, NULL, 0, 0,
     NULL,
     0,
     NULL,
     }
    ,
};

/*============================================
 * Class methods
 */

IBusChewingConfig *ibus_chewing_config_new(IBusService * service,
					   MkdgBackend * backend,
					   gpointer parent,
					   gpointer auxData)
{
    IBusConfig *config = NULL;
#if IBUS_CHECK_VERSION(1, 4, 0)
    GError *error = NULL;
    GDBusConnection *connection = ibus_service_get_connection(service);
    config = g_object_ref_sink(ibus_config_new(connection, NULL, &error));
    g_assert(error == NULL);
#else
    GList *connections_list = ibus_service_get_connections(service);
    g_assert(connections_list);
    g_assert(connections_list->data);
    IBusConnection *iConnection =
	(IBusConnection *) connections_list->data;
    config = g_object_ref_sink(ibus_config_new(iConnection));
#endif
    IBusChewingConfig *self = g_new0(IBusChewingConfig, 1);
    self->config = config;
    self->backend = backend;
    self->properties =
	mkdg_properties_from_spec_array(propSpecs, backend, parent,
					auxData);
    return self;
}

GValue *ibus_chewing_config_load(IBusChewingConfig * self,
				 const gchar * key, gpointer userData)
{
    if (self == NULL) {
	IBUS_CHEWING_LOG(ERROR,
			 "ibus_chewing_config_load(-,%s,-): self should not be NULL",
			 key);
    }

    iConfig->properties if (backend == NULL) {
    }
    if (backend->config == NULL) {
	IBUS_CHEWING_LOG(ERROR, "config backend is not available");
    }
#define BUFFER_SIZE_LOCAL 200
    int i;
    for (i = 0; propSpecs[i].valueType != G_TYPE_INVALID; i++) {
	GValue gValue = { 0 };
	gchar *key = propSpecs[i].key;
	mkdg_log(DEBUG, "ibus_chewing_config_new(), key=%s", key);
	if (!ibus_chewing_config_get_value(self, key, &gValue)) {
	    gchar *defaultValue = propSpecs[i].defaultValue;
	    gchar logString[BUFFER_SIZE_LOCAL];
	    g_snprintf(logString, BUFFER_SIZE_LOCAL,
		       "%s /desktop/ibus/" IBUS_CHEWING_CONFIG_SECTION
		       "/%s.", _("Warning: cannot load configure key"),
		       key);
	    guint64 uintValue;
	    gint64 intValue;
	    if (defaultValue) {
		switch (propSpecs[i].valueType) {
		case G_TYPE_BOOLEAN:
		    if (strcmp(propSpecs[i].defaultValue, "1") == 0) {
			g_value_set_boolean(&gValue, TRUE);
		    } else {
			g_value_set_boolean(&gValue, FALSE);
		    }
		    break;
		case G_TYPE_UINT:
		    uintValue = g_ascii_strtoull(defaultValue, NULL, 10);
		    g_value_set_uint(&gValue, uintValue);
		    break;
		case G_TYPE_INT:
		    intValue = g_ascii_strtoull(defaultValue, NULL, 10);
		    g_value_set_int(&gValue, intValue);
		    break;
		case G_TYPE_STRING:
		    g_value_set_string(&gValue, defaultValue);
		    break;
		default:
		    mkdg_log(WARN,
			     "ibus_chewing_config_new(): Unsupported value type for key %s, skip",
			     key);
		    break;
		}
		if (G_IS_VALUE(&gValue)) {
		    ibus_chewing_config_set_value(self, key, &gValue);
		    g_strlcat(logString, _("Use default value:"),
			      BUFFER_SIZE_LOCAL);
		    g_strlcat(logString, defaultValue, BUFFER_SIZE_LOCAL);
		}
	    } else {
		g_strlcat(logString, _("No default value, skipped."),
			  BUFFER_SIZE_LOCAL);
	    }
	    mkdg_log(INFO, logString);
	}
	if (G_IS_VALUE(&gValue)) {
	    g_value_unset(&gValue);
	}
    }
#undef BUFFER_SIZE_LOCAL
}

GValue *ibus_chewing_config_load_value(IBusChewingConfig * self,
				       const gchar * key)
{
    if (self == NULL || self->backend == NULL) {
	return FALSE;
    }

}

/* Load: backend -> MkdgProperties or Default */
/* Save: MkdgProperties -> backend */
/* Get: MkdgProperties -> UI */
/* Set: UI -> MkdgProperties */
/* Apply: MkdgProperties -> set callback */

gboolean ibus_chewing_config_load_value(IBusChewingConfig * self,
					const gchar * key, GValue * value)
{
    if (self == NULL || self->backend == NULL) {
	return FALSE;
    }
    return ibus_chewing_config_load_ibus_config(self,
						IBUS_CHEWING_CONFIG_SECTION,
						key, value);
}

gboolean ibus_chewing_config_load_ibus_config(IBusChewingConfig * self,
					      const gchar * section,
					      const gchar * key,
					      GValue * value)
{
    GValue *ret =
	mkdg_backend_load(self->backend, value, section, key, NULL);
    if (ret == NULL) {
	return FALSE;
    }
    return TRUE;
}

gboolean ibus_chewing_config_save_value(IBusChewingConfig * self,
					const gchar * key, GValue * gValue)
{
    return mkdg_backend_save(self->backend, value,
			     IBUS_CHEWING_CONFIG_SECTION, key, NULL);
}


gboolean ibus_chewing_config_get_value(IBusChewingConfig * self,
				       const gchar * key, GValue * value)
{
    return
	return ibus_chewing_config_load_ibus_config(self,
						    IBUS_CHEWING_CONFIG_SECTION,
						    key, value);
}


gboolean ibus_chewing_config_save_value(IBusChewingConfig * self,
					const gchar * key, GValue * gValue)
{
    return mkdg_backend_save(self->backend, value, section, key, NULL);
    gboolean result = FALSE;
#if IBUS_CHECK_VERSION(1, 4, 0)
    GVariant *gVar = g_variant_ref_sink(g_value_to_g_variant(gValue));
    if (gVar != NULL) {
	result = ibus_config_set_value(self->config,
				       IBUS_CHEWING_CONFIG_SECTION, key,
				       gVar);
    }
#else
    result =
	ibus_config_set_value(self->config, IBUS_CHEWING_CONFIG_SECTION,
			      gValue);
#endif

    if (result == FALSE) {
	mkdg_log(WARN,
		 "ibus_chewing_config_set_value(-, %s, -) %s %s",
		 key, _("Failed to set variable"), key);
	return FALSE;
    }

    if (!self->config) {
	mkdg_log(WARN,
		 "ibus_chewing_config_set_value(-, %s, -) %s",
		 key, _("Failed to connect to IBusService"));
	return FALSE;
    }

    PropertyContext ctx;
    ctx.spec = ibus_chewing_config_find_key(key);
    ctx.spec->setFunc(&ctx, gValue);
    return TRUE;
}

PropertySpec *ibus_chewing_config_find_key(const gchar * key)
{
    int i;
    for (i = 0; propSpecs[i].valueType != G_TYPE_INVALID; i++) {
	if (STRING_EQUALS(propSpecs[i].key, key)) {
	    return &propSpecs[i];
	}
    }
    return NULL;
}

MkdgProperties
    * ibus_chewing_config_load_all(MkdgBackend * backend,
				   gpointer userData)
{
    gsize arraySize = 0;
    gsize i;

    for (i = 0; specs[i].valueType != G_TYPE_INVALID; i++) {
	arraySize++;
    }
    MkdgProperties *result = g_ptr_array_sized_new(arraySize);
    for (i = 0; i < arraySize; i++) {
	PropertyContext *ctx =
	    PropertyContext_new(&specs[i], NULL, backend,
				userData);
	if (backend) {
	    GValue *ret = backend->loadFunction(backend, &(ctx->value),
						IBUS_CHEWING_CONFIG_SECTION,
						ctx->spec->key, userData);
	    if (ret == NULL) {
		mkdg_log(WARN,
			 "ibus_chewing_config_to_property_context_array(): failed to load key %s",
			 ctx->spec->key);
	    }
	}
	g_ptr_array_add(result, (gpointer) ctx);
    }
    return result;
}

gboolean ibus_chewing_config_foreach_properties(gboolean stopOnError,
						CallbackBoolFunc callback,
						gpointer ctxData,
						gpointer userData)
{
    int i;
    gboolean result = TRUE;
    for (i = 0; propSpecs[i].valueType != G_TYPE_INVALID; i++) {
	PropertyContext ctx;
	ctx.spec = &propSpecs[i];
	ctx.userData = ctxData;
	gboolean success = callback(&ctx, userData);

	if (!success) {
	    if (stopOnError) {
		return FALSE;
	    }
	    result = FALSE;
	}
    }
    return result;
}
