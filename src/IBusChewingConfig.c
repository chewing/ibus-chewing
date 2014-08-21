#define GETTEXT_PACKAGE "gtk20"
#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include "ibus-chewing-util.h"
#include "IBusChewingConfig.h"


const gchar *kbType_ids[] = {
    N_("default"),
    N_("hsu"),
    N_("ibm"),
    N_("gin_yieh"),
    N_("eten"),
    N_("eten26"),
    N_("dvorak"),
    N_("dvorak_hsu"),
    N_("dachen_26"),
    N_("hanyu"),
    NULL
};

#define SELKEYS_ARRAY_SIZE 8
const gchar *selKeys_array[SELKEYS_ARRAY_SIZE + 1] = {
    "1234567890",
    "asdfghjkl;",
    "asdfzxcv89",
    "asdfjkl789",
    "aoeu;qjkix",		/* Dvorak */
    "aoeuhtnsid",		/* Dvorak */
    "aoeuidhtns",		/* Dvorak */
    "1234qweras",
    NULL
};

const gchar *syncCapsLock_strs[] = {
    NC_("Sync", "disable"),
    NC_("Sync", "keyboard"),
    NC_("Sync", "input method"),
    NULL
};

const gchar *outputCharsets[] = {
    N_("Auto"),
    N_("Big5"),
    N_("UTF8"),
    NULL
};

const gchar *inputStyles[] = {
    N_("in application window"),
    N_("in candidate window"),
    NULL
};


#ifdef IBUS_CHEWING_MAIN
static ChewingKbType kbType_id_get_index(const gchar * kbType_id)
{
    ChewingKbType i = 0;
    for (i = 0; kbType_ids[i] != NULL; i++) {
	if (strcmp(kbType_id, kbType_ids[i]) == 0) {
	    return i;
	}
    }
    return CHEWING_KBTYPE_INVALID;
}
#endif

/*============================================
 * Callback functions
 */
static void KBType_set_callback(PropertyContext * ctx, GValue * value)
{
#ifdef IBUS_CHEWING_MAIN
    ChewingKbType kbType = kbType_id_get_index(g_value_get_string(value));
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->userData;
    chewing_set_KBType(engine->context, kbType);
#endif
}

static void selKeys_set_callback(PropertyContext * ctx, GValue * value)
{
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->userData;
    ibus_chewing_engine_set_selKeys_string(engine,
					   g_value_get_string(value));
    if (!engine->table) {
	engine->table =
	    g_object_ref_sink(ibus_lookup_table_new
			      (strlen(g_value_get_string(value)), 0, FALSE,
			       TRUE));
    }
    ibus_chewing_engine_set_lookup_table_label(engine,
					       g_value_get_string(value));
#endif
}

static void hsuSelKeyType_set_callback(PropertyContext * ctx,
				       GValue * value)
{
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->userData;
    chewing_set_hsuSelKeyType(engine->context, g_value_get_int(value));
#endif
}

static void autoShiftCur_set_callback(PropertyContext * ctx,
				      GValue * value)
{
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->userData;
    chewing_set_autoShiftCur(engine->context,
			     (g_value_get_boolean(value)) ? 1 : 0);
#endif
}

static void addPhraseDirection_set_callback(PropertyContext * ctx,
					    GValue * value)
{
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->userData;
    chewing_set_addPhraseDirection(engine->context,
				   (g_value_get_boolean(value)) ? 1 : 0);
#endif
}

static void easySymbolInput_set_callback(PropertyContext * ctx,
					 GValue * value)
{
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->userData;
    chewing_set_easySymbolInput(engine->context,
				(g_value_get_boolean(value)) ? 1 : 0);
    if (g_value_get_boolean(value)) {
	engine->chewingFlags |= CHEWING_FLAG_EASY_SYMBOL_INPUT;
    } else {
	engine->chewingFlags &= ~CHEWING_FLAG_EASY_SYMBOL_INPUT;
    }
#endif
}

static void escCleanAllBuf_set_callback(PropertyContext * ctx,
					GValue * value)
{
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->userData;
    chewing_set_escCleanAllBuf(engine->context,
			       (g_value_get_boolean(value)) ? 1 : 0);
#endif
}

/* Additional symbol buffer length */
static void maxChiSymbolLen_set_callback(PropertyContext * ctx,
					 GValue * value)
{
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->userData;
    chewing_set_maxChiSymbolLen(engine->context, g_value_get_int(value));
#endif
}

static void forceLowercaseEnglish_set_callback(PropertyContext * ctx,
					       GValue * value)
{
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->userData;
    if (g_value_get_boolean(value)) {
	engine->chewingFlags |= CHEWING_FLAG_FORCE_LOWERCASE_ENGLISH;
    } else {
	engine->chewingFlags &= ~CHEWING_FLAG_FORCE_LOWERCASE_ENGLISH;
    }
#endif
}

static void syncCapsLock_set_callback(PropertyContext * ctx,
				      GValue * value)
{
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->userData;
    const gchar *str = g_value_get_string(value);
    if (strcmp(str, "keyboard") == 0) {
	engine->syncCapsLock_local = CHEWING_MODIFIER_SYNC_FROM_KEYBOARD;
    } else if (strcmp(str, "input method") == 0) {
	engine->syncCapsLock_local = CHEWING_MODIFIER_SYNC_FROM_IM;
    } else {
	engine->syncCapsLock_local = CHEWING_MODIFIER_SYNC_DISABLE;
    }
#endif
}

static void numpadAlwaysNumber_set_callback(PropertyContext * ctx,
					    GValue * value)
{
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->userData;
    if (g_value_get_boolean(value)) {
	engine->chewingFlags |= CHEWING_FLAG_NUMPAD_ALWAYS_NUMBER;
    } else {
	engine->chewingFlags &= ~CHEWING_FLAG_NUMPAD_ALWAYS_NUMBER;
    }
#endif
}

static void candPerPage_set_callback(PropertyContext * ctx, GValue * value)
{
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->userData;
    chewing_set_candPerPage(engine->context, g_value_get_int(value));
    if (engine->table) {
	ibus_lookup_table_clear(engine->table);
	engine->table->page_size = g_value_get_int(value);
    } else {
	engine->table =
	    g_object_ref_sink(ibus_lookup_table_new
			      (g_value_get_int(value), 0, FALSE, TRUE));
    }
#endif
}

static void phraseChoiceRearward_set_callback(PropertyContext * ctx,
					      GValue * value)
{
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->userData;
    chewing_set_phraseChoiceRearward(engine->context,
				     (g_value_get_boolean(value)) ? 1 : 0);
#endif
}

static void spaceAsSelection_set_callback(PropertyContext * ctx,
					  GValue * value)
{
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->userData;
    chewing_set_spaceAsSelection(engine->context,
				 (g_value_get_boolean(value)) ? 1 : 0);
#endif
}

static void plainZhuyin_set_callback(PropertyContext * ctx, GValue * value)
{
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->userData;
    if (g_value_get_boolean(value)) {
	engine->chewingFlags |= CHEWING_FLAG_PLAIN_ZHUYIN;
    } else {
	engine->chewingFlags &= ~CHEWING_FLAG_PLAIN_ZHUYIN;
    }
#endif
}

/*===== End of Callback functions =====*/

PropertySpec propSpecs[] = {
    {G_TYPE_STRING, "KBType", "Keyboard", N_("Keyboard Type"),
     "default", kbType_ids, NULL, 0, 0,
     NULL, KBType_set_callback,
     MAKER_DIALOG_PROPERTY_FLAG_INEDITABLE |
     MAKER_DIALOG_PROPERTY_FLAG_HAS_TRANSLATION, 0, 0,
     N_("Select Zhuyin keyboard layout."),
     }
    ,
    {G_TYPE_STRING, "selKeys", "Keyboard", N_("Selection keys"),
     "1234567890", selKeys_array, NULL, 0, 0,
     NULL, selKeys_set_callback,
     0, 0, 0,
     N_
     ("Keys used to select candidate. For example \"asdfghjkl;\", press 'a' to select the 1st candidate, 's' for 2nd, and so on."),
     }
    ,
    {G_TYPE_INT, "hsuSelKeyType", "Keyboard", N_("Hsu's selection key"),
     "1", NULL, NULL, 1, 2,
     NULL, hsuSelKeyType_set_callback,
     0, 0, 0,
     N_
     ("Hsu's keyboard selection keys, 1 for asdfjkl789, 2 for asdfzxcv89 ."),
     }
    ,

    {G_TYPE_BOOLEAN, "autoShiftCur", "Editing", N_("Auto move cursor"),
     "1", NULL, NULL, 0, 1,
     NULL, autoShiftCur_set_callback,
     0, 0, 0,
     N_("Automatically move cursor to next character."),
     }
    ,
    {G_TYPE_BOOLEAN, "addPhraseDirection", "Editing",
     N_("Add phrases in front"),
     "1", NULL, NULL, 0, 1,
     NULL, addPhraseDirection_set_callback,
     0, 0, 0,
     N_("Add phrases in the front."),
     }
    ,
    {G_TYPE_BOOLEAN, "easySymbolInput", "Editing", N_("Easy symbol input"),
     "1", NULL, NULL, 0, 1,
     NULL, easySymbolInput_set_callback,
     0, 0, 0,
     N_("Easy symbol input."),
     }
    ,
    {G_TYPE_BOOLEAN, "escCleanAllBuf", "Editing",
     N_("Esc clean all buffer"),
     "0", NULL, NULL, 0, 1,
     NULL, escCleanAllBuf_set_callback,
     0, 0, 0,
     N_("Escape key cleans the text in pre-edit-buffer."),
     }
    ,
    {G_TYPE_INT, "maxChiSymbolLen", "Editing",
     N_("Maximum Chinese characters"),
     "20", NULL, NULL, 8, 50,
     NULL, maxChiSymbolLen_set_callback,
     0, 0, 0,
     N_
     ("Maximum Chinese characters in pre-edit buffer, including inputing Zhuyin symbols"),
     }
    ,
    {G_TYPE_BOOLEAN, "forceLowercaseEnglish", "Editing",
     N_("Force lowercase in En mode"),
     "0", NULL, NULL, 0, 1,
     NULL, forceLowercaseEnglish_set_callback,
     0, 0, 0,
     N_("Ignore CapsLock status and input lowercase by default.\n\
		It is handy if you wish to enter lowercase by default. Uppercase can still be inputted with Shift."),
     }
    ,

    /* Sync between CapsLock and IM */
    {G_TYPE_STRING, "syncCapsLock", "Editing",
     N_("Sync between CapsLock and IM"),
     "disable", syncCapsLock_strs, "Sync", 0, 1,
     NULL, syncCapsLock_set_callback,
     MAKER_DIALOG_PROPERTY_FLAG_INEDITABLE |
     MAKER_DIALOG_PROPERTY_FLAG_HAS_TRANSLATION |
     MAKER_DIALOG_PROPERTY_FLAG_TRANSLATION_WITH_CONTEXT,
     0, 0,
     N_("Occasionally, the CapsLock status does not match the IM, \
		this option determines how these status be synchronized. Valid values:\n\
		\"disable\": Do nothing.\n\
		\"keyboard\": IM status follows keyboard status.\n\
		\"IM\": Keyboard status follows IM status."),
     }
    ,

    {G_TYPE_BOOLEAN, "numpadAlwaysNumber", "Editing",
     N_("Number pad always input number"),
     "1", NULL, NULL, 0, 1,
     NULL, numpadAlwaysNumber_set_callback,
     0, 0, 0,
     N_("Always input numbers when number keys from key pad is inputted."),
     }
    ,
    {G_TYPE_BOOLEAN, "plainZhuyin", "Selecting", N_("Plain Zhuyin mode"),
     "0", NULL, NULL, 0, 1,
     NULL, plainZhuyin_set_callback,
     0, 0, 0,
     N_
     ("In plain Zhuyin mode, automatic candidate selection and related options are disabled or ignored."),
     }
    ,
    {G_TYPE_INT, "candPerPage", "Selecting", N_("Candidate per page"),
     "10", NULL, NULL, 8, 10,
     NULL, candPerPage_set_callback,
     0, 0, 0,
     N_("Number of candidate per page."),
     }
    ,
    {G_TYPE_BOOLEAN, "phraseChoiceRearward", "Selecting",
     N_("Choose phrases from backward"),
     "1", NULL, NULL, 0, 1,
     NULL, phraseChoiceRearward_set_callback,
     0, 0, 0,
     N_("Choose phrases from the back, without moving cursor."),
     }
    ,
    {G_TYPE_BOOLEAN, "spaceAsSelection", "Selecting",
     N_("Space to select"),
     "0", NULL, NULL, 0, 1,
     NULL, spaceAsSelection_set_callback,
     0, 0, 0,
     "Press Space to select the candidate.",
     }
    ,

    {G_TYPE_INVALID, "", "", "",
     "", NULL, NULL, 0, 0,
     NULL, NULL,
     0, 0, 0,
     NULL,
     }
    ,
};

/*============================================
 * Supporting functions
 */
#if IBUS_CHECK_VERSION(1, 4, 0)
void g_variant_to_g_value(GVariant * gVar, GValue * gValue)
{
    const GVariantType *gVType = g_variant_get_type(gVar);
    if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_BOOLEAN)) {
	g_value_init(gValue, G_TYPE_BOOLEAN);
	g_value_set_boolean(gValue, g_variant_get_boolean(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_UINT16)) {
	g_value_init(gValue, G_TYPE_UINT);
	g_value_set_uint(gValue, g_variant_get_uint16(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_UINT32)) {
	g_value_init(gValue, G_TYPE_UINT);
	g_value_set_uint(gValue, g_variant_get_uint32(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_UINT64)) {
	g_value_init(gValue, G_TYPE_UINT64);
	g_value_set_uint64(gValue, g_variant_get_uint64(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_INT16)) {
	g_value_init(gValue, G_TYPE_INT);
	g_value_set_int(gValue, g_variant_get_int16(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_INT32)) {
	g_value_init(gValue, G_TYPE_INT);
	g_value_set_int(gValue, g_variant_get_int32(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_INT64)) {
	g_value_init(gValue, G_TYPE_INT);
	g_value_set_int64(gValue, g_variant_get_int64(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_STRING)) {
	g_value_init(gValue, G_TYPE_STRING);
	g_value_set_string(gValue, g_variant_get_string(gVar, NULL));
    }
}

GVariant *g_value_to_g_variant(GValue * gValue)
{
    GType gType = G_VALUE_TYPE(gValue);
    GVariant *gVar = NULL;
    switch (gType) {
    case G_TYPE_BOOLEAN:
	gVar = g_variant_new_boolean(g_value_get_boolean(gValue));
	break;
    case G_TYPE_UINT:
	gVar = g_variant_new_uint32(g_value_get_uint(gValue));
	break;
    case G_TYPE_UINT64:
	gVar = g_variant_new_uint64(g_value_get_uint(gValue));
	break;
    case G_TYPE_INT:
	gVar = g_variant_new_int32(g_value_get_int(gValue));
	break;
    case G_TYPE_INT64:
	gVar = g_variant_new_int64(g_value_get_int(gValue));
	break;
    case G_TYPE_STRING:
	gVar = g_variant_new_string(g_value_get_string(gValue));
	break;
    default:
	break;
    }
    return gVar;
}
#endif

/*============================================
 * Class methods
 */

IBusChewingConfig *IBusChewingConfig_new(IBusService * service)
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
    if (config == NULL) {
	return NULL;
    }
    IBusChewingConfig *self = g_new0(IBusChewingConfig, 1);
    self->config = config;
    IBusChewingConfig_load(self);
    return self;
}

void IBusChewingConfig_load(IBusChewingConfig * self)
{
#define BUFFER_SIZE_LOCAL 200
    int i;
    for (i = 0; propSpecs[i].valueType != G_TYPE_INVALID; i++) {
	GValue gValue = { 0 };
	gchar *key = propSpecs[i].key;
	IBUS_CHEWING_LOG(DEBUG, "IBusChewingConfig_new(), key=%s", key);
	if (!IBusChewingConfig_get_value(self, key, &gValue)) {
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
		    IBUS_CHEWING_LOG(WARN,
				     "IBusChewingConfig_new(): Unsupported value type for key %s, skip",
				     key);
		    break;
		}
		if (G_IS_VALUE(&gValue)) {
		    IBusChewingConfig_set_value(self, key, &gValue);
		    g_strlcat(logString, _("Use default value:"),
			      BUFFER_SIZE_LOCAL);
		    g_strlcat(logString, defaultValue, BUFFER_SIZE_LOCAL);
		}
	    } else {
		g_strlcat(logString, _("No default value, skipped."),
			  BUFFER_SIZE_LOCAL);
	    }
	    IBUS_CHEWING_LOG(INFO, logString);
	}
	if (G_IS_VALUE(&gValue)) {
	    g_value_unset(&gValue);
	}
    }
#undef BUFFER_SIZE_LOCAL
}


gboolean IBusChewingConfig_get_value(IBusChewingConfig * self,
				     const gchar * key, GValue * gValue)
{
    return IBusChewingConfig_get_ibus_value(self,
					    IBUS_CHEWING_CONFIG_SECTION,
					    key, gValue);
}

gboolean IBusChewingConfig_get_ibus_value(IBusChewingConfig * self,
					  const gchar * section,
					  const gchar * key,
					  GValue * gValue)
{
#if IBUS_CHECK_VERSION(1, 4, 0)
    GVariant *gVar = ibus_config_get_value(self->config, section,
					   key);
    if (gVar == NULL) {
	return FALSE;
    }
    g_variant_ref_sink(gVar);
    g_variant_to_g_value(gVar, gValue);
    g_variant_unref(gVar);
    return TRUE;
#else
    return ibus_config_get_value(self->config, section, key, gValue);
#endif
}

gboolean IBusChewingConfig_set_value(IBusChewingConfig * self,
				     const gchar * key, GValue * gValue)
{
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
	IBUS_CHEWING_LOG(WARN,
			 "IBusChewingConfig_set_value(-, %s, -) %s %s",
			 key, _("Failed to set variable"), key);
	return FALSE;
    }

    if (!self->config) {
	IBUS_CHEWING_LOG(WARN,
			 "IBusChewingConfig_set_value(-, %s, -) %s",
			 key, _("Failed to connect to IBusService"));
	return FALSE;
    }

    PropertyContext ctx;
    ctx.spec = IBusChewingConfig_find_key(key);
    ctx.spec->setFunc(&ctx, gValue);
    return TRUE;
}

PropertySpec *IBusChewingConfig_find_key(const gchar * key)
{
    int i;
    for (i = 0; propSpecs[i].valueType != G_TYPE_INVALID; i++) {
	if (STRING_EQUALS(propSpecs[i].key, key)) {
	    return &propSpecs[i];
	}
    }
    return NULL;
}

gboolean IBusChewingConfig_foreach_properties(gboolean stopOnError,
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

#if 0
/**
 * IBusChewingConfig_save:
 * @self: this instances.
 * @keySuffix: key to be set.
 * @returns: TRUE if successful, FALSE otherwise.
 *
 * Save the property value to disk.
 */
gboolean IBusChewingConfig_save(IBusChewingConfig * self,
				const gchar * keySuffix)
{
    IBUS_CHEWING_LOG(DEBUG, "IBusChewingConfig_save(%s)", keySuffix);
    GValue gValue = { 0 };
    maker_dialog_get_widget_value(MAKER_DIALOG(self->settingDialog),
				  keySuffix, &gValue);
    return IBusChewingConfig_set_value(self, keySuffix, &gValue);
}

/**
 * IBusChewingConfig_save_all:
 * @self: this instances.
 * @returns: TRUE if all successful, FALSE otherwise.
 *
 * Save all property values to disk.
 */
gboolean IBusChewingConfig_save_all(IBusChewingConfig * self)
{
    int i;
    gboolean success = TRUE;
    for (i = 0; propSpecs[i].valueType != G_TYPE_INVALID; i++) {
	if (!IBusChewingConfig_save(self, propSpecs[i].key)) {
	    success = FALSE;
	}
    }
    return success;
}
#endif
