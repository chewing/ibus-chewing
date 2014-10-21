#define GETTEXT_PACKAGE "gtk20"
#include <glib.h>
#include <glib/gi18n.h>
#include "MakerDialogUtil.h"
#include "MakerDialogProperty.h"
#include "IBusChewingUtil.h"
#include "IBusChewingProperties.h"
#include "IBusConfigBackend.h"
#ifdef GSETTINGS_SUPPORT
#include "GSettingsBackend.h"
#endif

#define PAGE_EDITING  N_("Editing")
#define PAGE_SELECTING  N_("Selecting")
#define PAGE_KEYBOARD  N_("Keyboard")

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

MkdgPropertySpec propSpecs[] = {
    {G_TYPE_STRING, "kb-type", PAGE_KEYBOARD, N_("Keyboard Type"),
	IBUS_CHEWING_PROPERTIES_SUBSECTION, "default", kbType_ids, NULL, 0, 0,
	KBType_apply_callback,
	MKDG_PROPERTY_FLAG_NO_NEW | MKDG_PROPERTY_FLAG_HAS_TRANSLATION,
	N_("Select Zhuyin keyboard layout."),
    }
    ,
    {G_TYPE_STRING, "sel-keys", PAGE_KEYBOARD, N_("Selection keys"),
	IBUS_CHEWING_PROPERTIES_SUBSECTION, "1234567890", selKeys_array, NULL,
	0,
	0,
	selKeys_apply_callback, 0,
	N_("Keys used to select candidate. For example \"asdfghjkl;\", press 'a' to select the 1st candidate, 's' for 2nd, and so on."),
    }
    ,
    {G_TYPE_INT, "hsu-sel-key-type", PAGE_KEYBOARD,
	N_("Hsu's selection key"),
	IBUS_CHEWING_PROPERTIES_SUBSECTION, "1", NULL, NULL, 1, 2,
	hsuSelKeyType_apply_callback, 0,
	N_("Hsu's keyboard selection keys, 1 for asdfjkl789, 2 for asdfzxcv89 ."),
    }
    ,
    {G_TYPE_BOOLEAN, "auto-shift-cur", PAGE_EDITING,
	N_("Auto move cursor"),
	IBUS_CHEWING_PROPERTIES_SUBSECTION, "1", NULL, NULL, 0, 1,
	autoShiftCur_apply_callback, 0,
	N_("Automatically move cursor to next character."),
    }
    ,
    {G_TYPE_BOOLEAN, "add-phrase-direction", PAGE_EDITING,
	N_("Add phrases in front"),
	IBUS_CHEWING_PROPERTIES_SUBSECTION, "1", NULL, NULL, 0, 1,
	addPhraseDirection_apply_callback, 0,
	N_("Add phrases in the front."),
    }
    ,
    {G_TYPE_BOOLEAN, "easy-symbol-input", PAGE_EDITING,
	N_("Easy symbol input"),
	IBUS_CHEWING_PROPERTIES_SUBSECTION, "1", NULL, NULL, 0, 1,
	easySymbolInput_apply_callback, 0,
	N_("Easy symbol input."),
    }
    ,
    {G_TYPE_BOOLEAN, "esc-clean-all-buf", PAGE_EDITING,
	N_("Esc clean all buffer"),
	IBUS_CHEWING_PROPERTIES_SUBSECTION, "0", NULL, NULL, 0, 1,
	escCleanAllBuf_apply_callback, 0,
	N_("Escape key cleans the text in pre-edit-buffer."),
    }
    ,
    {G_TYPE_INT, "max-chi-symbol-len", PAGE_EDITING,
	N_("Maximum Chinese characters"),
	IBUS_CHEWING_PROPERTIES_SUBSECTION, "20", NULL, NULL, 8, 39,
	maxChiSymbolLen_apply_callback, 0,
	N_("Maximum Chinese characters in pre-edit buffer, including inputing Zhuyin symbols"),
    }
    ,
    {G_TYPE_BOOLEAN, "force-lowercase-english", PAGE_EDITING,
	N_("Force lowercase in En mode"),
	IBUS_CHEWING_PROPERTIES_SUBSECTION, "0", NULL, NULL, 0, 1,
	forceLowercaseEnglish_apply_callback, 0,
	N_("Ignore CapsLock status and input lowercase by default.\n\
It is handy if you wish to enter lowercase by default.\n\
Uppercase can still be inputted with Shift."),
    }
    ,

	/* Sync between CapsLock and IM */
    {G_TYPE_STRING, "sync-caps-lock", PAGE_EDITING,
	N_("Sync between CapsLock and IM"),
	IBUS_CHEWING_PROPERTIES_SUBSECTION, "disable", syncCapsLock_strs,
	"Sync",
	0, 1,
	syncCapsLock_apply_callback,
	MKDG_PROPERTY_FLAG_NO_NEW | MKDG_PROPERTY_FLAG_HAS_TRANSLATION,
	N_("Occasionally, the CapsLock status does not match the IM, this option determines how these status be synchronized. Valid values:\n\
\"disable\": Do nothing.\n\
\"keyboard\": IM status follows keyboard status.\n\
\"IM\": Keyboard status follows IM status."),
    }
    ,
    {G_TYPE_BOOLEAN, "numpad-always-number", PAGE_EDITING,
	N_("Number pad always input number"),
	IBUS_CHEWING_PROPERTIES_SUBSECTION, "1", NULL, NULL, 0, 1,
	numpadAlwaysNumber_apply_callback, 0,
	N_("Always input numbers when number keys from key pad is inputted."),
    }
    ,
    {G_TYPE_BOOLEAN, "shift-toggle-chinese", PAGE_EDITING,
        N_("Shift toggle Chinese Mode"),
        IBUS_CHEWING_PROPERTIES_SUBSECTION, "1", NULL, NULL, 0, 1,
        easySymbolInput_apply_callback, 0,
        N_("Shift key to toggle Chinese Mode"),
    }
    ,
    {G_TYPE_BOOLEAN, "plain-zhuyin", PAGE_SELECTING,
	N_("Plain Zhuyin mode"),
	IBUS_CHEWING_PROPERTIES_SUBSECTION, "0", NULL, NULL, 0, 1,
	plainZhuyin_apply_callback, 0,
	N_("In plain Zhuyin mode, automatic candidate selection and related options are disabled or ignored."),
    }
    ,
    {G_TYPE_INT, "cand-per-page", PAGE_SELECTING, N_("Candidate per page"),
        IBUS_CHEWING_PROPERTIES_SUBSECTION, "10", NULL, NULL, 8, 10,
        candPerPage_apply_callback, 0,
        N_("Number of candidate per page."),
    }
    ,
    {G_TYPE_BOOLEAN, "phrase-choice-from-last", PAGE_SELECTING,
        N_("Choose phrases from backward"),
        IBUS_CHEWING_PROPERTIES_SUBSECTION, "1", NULL, NULL, 0, 1,
        phraseChoiceRearward_apply_callback, 0,
        N_("Choose phrases from the back, without moving cursor."),
    }
    ,
    {G_TYPE_BOOLEAN, "space-as-selection", PAGE_SELECTING,
        N_("Space to select"),
        IBUS_CHEWING_PROPERTIES_SUBSECTION,
        "0", NULL, NULL, 0, 1,
        spaceAsSelection_apply_callback, 0,
        "Press Space to select the candidate.",
    }
    ,
    {G_TYPE_INVALID, "", "", "",
        IBUS_CHEWING_PROPERTIES_SUBSECTION, "", NULL, NULL, 0, 0,
        NULL,
        0,
        NULL,
    }
    ,
};

/*============================================
 * Class methods
 */

IBusChewingProperties *ibus_chewing_properties_new(MkdgBackend * backend,
						   gpointer parent,
						   gpointer auxData)
{
    IBusChewingProperties *self = g_new0(IBusChewingProperties, 1);
    self->properties =
	mkdg_properties_from_spec_array(propSpecs, backend, parent,
					auxData);
    if (STRING_EQUALS(backend->id, GSETTINGS_BACKEND_ID)) {
	self->confObjTable = g_hash_table_new(g_str_hash, g_str_equal);
    } else {
	self->confObjTable = NULL;
    }
    return self;
}

static GString *ibus_section_to_schema(const gchar * section)
{
    GString *result = g_string_new("org.freedesktop");
    gchar **strArr = g_strsplit(section, "/", -1);
    gint i;
    for (i = 0; strArr[i] != NULL; i++) {
	g_string_append_printf(result, ".%s", strArr[i]);
    }
    g_strfreev(strArr);
    return result;
}

GValue *ibus_chewing_properties_read_general(IBusChewingProperties * self,
					     GValue * value,
					     const gchar * section,
					     const gchar * key,
					     gpointer userData)
{
    g_assert(self);
    g_assert(value);
    if (STRING_EQUALS(self->properties->backend->id, GSETTINGS_BACKEND_ID)) {
	GSettings *confObj;
	if (!g_hash_table_contains(self->confObjTable, (gpointer) section)) {
	    GString *schemaIdStr = ibus_section_to_schema(section);
	    confObj = g_settings_new(schemaIdStr->str);
	    g_hash_table_insert(self->confObjTable, (gpointer) section,
				(gpointer) confObj);
	    g_string_free(schemaIdStr, TRUE);
	} else {
	    confObj =
		(GSettings *) g_hash_table_lookup(self->confObjTable,
						  (gconstpointer) section);
	}
	g_assert(confObj);
	return mkdg_g_settings_read_value(confObj, value, key);
    }
    return mkdg_backend_read(self->properties->backend, value, section,
			     key, userData);
}

gboolean ibus_chewing_properties_read_boolean_general(IBusChewingProperties * self,
					     const gchar * section,
					     const gchar * key,
					     gpointer userData)
{
    GValue gValue = { 0 };
    g_value_init(&gValue, G_TYPE_BOOLEAN);
    ibus_chewing_properties_read_general(self, &gValue, section, key, userData);
    gboolean result=g_value_get_boolean(&gValue);
    g_value_unset(&gValue);
    return result;
}

gint ibus_chewing_properties_read_int_general(IBusChewingProperties * self,
					     const gchar * section,
					     const gchar * key,
					     gpointer userData)
{
    GValue gValue = { 0 };
    g_value_init(&gValue, G_TYPE_INT);
    ibus_chewing_properties_read_general(self, &gValue, section, key, userData);
    gint result=g_value_get_int(&gValue);
    g_value_unset(&gValue);
    return result;
}
