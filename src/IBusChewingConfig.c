#define GETTEXT_PACKAGE "gtk20"
#include <glib.h>
#include <glib/gi18n.h>
#include "MakerDialogUtil.h"
#include "MakerDialogProperty.h"
#include "ibus-chewing-util.h"
#include "IBusChewingConfig.h"
#include "IBusConfigBackend.h"

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

PropertySpec propSpecs[] = {
    {G_TYPE_STRING, "KBType", PAGE_KEYBOARD, N_("Keyboard Type"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "default", kbType_ids, NULL, 0, 0,
     MKDG_SPEC_FUNC(KBType_apply_callback),
     MKDG_PROPERTY_FLAG_NO_NEW | MKDG_PROPERTY_FLAG_HAS_TRANSLATION,
     N_("Select Zhuyin keyboard layout."),
     }
    ,
    {G_TYPE_STRING, "selKeys", PAGE_KEYBOARD, N_("Selection keys"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "1234567890", selKeys_array, NULL, 0,
     0,
     MKDG_SPEC_FUNC(selKeys_apply_callback), 0,
     N_
     ("Keys used to select candidate. For example \"asdfghjkl;\", press 'a' to select the 1st candidate, 's' for 2nd, and so on."),
     }
    ,
    {G_TYPE_INT, "hsuSelKeyType", PAGE_KEYBOARD, N_("Hsu's selection key"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "1", NULL, NULL, 1, 2,
     MKDG_SPEC_FUNC(hsuSelKeyType_apply_callback), 0,
     N_
     ("Hsu's keyboard selection keys, 1 for asdfjkl789, 2 for asdfzxcv89 ."),
     }
    ,

    {G_TYPE_BOOLEAN, "autoShiftCur", PAGE_EDITING, N_("Auto move cursor"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "1", NULL, NULL, 0, 1,
     MKDG_SPEC_FUNC(autoShiftCur_apply_callback), 0,
     N_("Automatically move cursor to next character."),
     }
    ,
    {G_TYPE_BOOLEAN, "addPhraseDirection", PAGE_EDITING,
     N_("Add phrases in front"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "1", NULL, NULL, 0, 1,
     MKDG_SPEC_FUNC(addPhraseDirection_apply_callback), 0,
     N_("Add phrases in the front."),
     }
    ,
    {G_TYPE_BOOLEAN, "easySymbolInput", PAGE_EDITING,
     N_("Easy symbol input"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "1", NULL, NULL, 0, 1,
     MKDG_SPEC_FUNC(easySymbolInput_apply_callback), 0,
     N_("Easy symbol input."),
     }
    ,
    {G_TYPE_BOOLEAN, "escCleanAllBuf", PAGE_EDITING,
     N_("Esc clean all buffer"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "0", NULL, NULL, 0, 1,
     MKDG_SPEC_FUNC(escCleanAllBuf_apply_callback), 0,
     N_("Escape key cleans the text in pre-edit-buffer."),
     }
    ,
    {G_TYPE_INT, "maxChiSymbolLen", PAGE_EDITING,
     N_("Maximum Chinese characters"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "20", NULL, NULL, 8, 50,
     MKDG_SPEC_FUNC(maxChiSymbolLen_apply_callback), 0,
     N_
     ("Maximum Chinese characters in pre-edit buffer, including inputing Zhuyin symbols"),
     }
    ,
    {G_TYPE_BOOLEAN, "forceLowercaseEnglish", PAGE_EDITING,
     N_("Force lowercase in En mode"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "0", NULL, NULL, 0, 1,
     MKDG_SPEC_FUNC(forceLowercaseEnglish_apply_callback), 0,
     N_("Ignore CapsLock status and input lowercase by default.\n\
		It is handy if you wish to enter lowercase by default. Uppercase can still be inputted with Shift."),
     }
    ,

    /* Sync between CapsLock and IM */
    {G_TYPE_STRING, "syncCapsLock", PAGE_EDITING,
     N_("Sync between CapsLock and IM"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "disable", syncCapsLock_strs, "Sync",
     0, 1,
     MKDG_SPEC_FUNC(syncCapsLock_apply_callback),
     MKDG_PROPERTY_FLAG_NO_NEW | MKDG_PROPERTY_FLAG_HAS_TRANSLATION,
     N_("Occasionally, the CapsLock status does not match the IM, \
		this option determines how these status be synchronized. Valid values:\n\
		\"disable\": Do nothing.\n\
		\"keyboard\": IM status follows keyboard status.\n\
		\"IM\": Keyboard status follows IM status."),
     }
    ,

    {G_TYPE_BOOLEAN, "numpadAlwaysNumber", PAGE_EDITING,
     N_("Number pad always input number"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "1", NULL, NULL, 0, 1,
     MKDG_SPEC_FUNC(numpadAlwaysNumber_apply_callback), 0,
     N_("Always input numbers when number keys from key pad is inputted."),
     }
    ,
    {G_TYPE_BOOLEAN, "plainZhuyin", PAGE_SELECTING,
     N_("Plain Zhuyin mode"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "0", NULL, NULL, 0, 1,
     MKDG_SPEC_FUNC(plainZhuyin_apply_callback), 0,
     N_
     ("In plain Zhuyin mode, automatic candidate selection and related options are disabled or ignored."),
     }
    ,
    {G_TYPE_INT, "candPerPage", PAGE_SELECTING, N_("Candidate per page"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "10", NULL, NULL, 8, 10,
     MKDG_SPEC_FUNC(candPerPage_apply_callback), 0,
     N_("Number of candidate per page."),
     }
    ,
    {G_TYPE_BOOLEAN, "phraseChoiceRearward", PAGE_SELECTING,
     N_("Choose phrases from backward"),
     IBUS_CHEWING_CONFIG_SUBSECTION, "1", NULL, NULL, 0, 1,
     MKDG_SPEC_FUNC(phraseChoiceRearward_apply_callback), 0,
     N_("Choose phrases from the back, without moving cursor."),
     }
    ,
    {G_TYPE_BOOLEAN, "spaceAsSelection", PAGE_SELECTING,
     N_("Space to select"),
     IBUS_CHEWING_CONFIG_SUBSECTION,
     "0", NULL, NULL, 0, 1,
     MKDG_SPEC_FUNC(spaceAsSelection_apply_callback), 0,
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

IBusChewingConfig *ibus_chewing_config_new(MkdgBackend * backend,
					   gpointer parent,
					   gpointer auxData)
{
    IBusChewingConfig *self = g_new0(IBusChewingConfig, 1);
    self->backend = backend;
    self->properties =
	mkdg_properties_from_spec_array(propSpecs, backend, parent,
					auxData);
    return self;
}

GValue *ibus_chewing_config_get_value(IBusChewingConfig * self,
				      const gchar * key)
{
    if (self == NULL || self->backend == NULL) {
	return NULL;
    }
    PropertyContext *ctx =
	mkdg_properties_find_by_key(self->properties, key);
    return property_context_get(ctx);
}

GValue *ibus_chewing_config_load_value(IBusChewingConfig * self,
				       const gchar * key,
				       gpointer userData)
{
    if (self == NULL || self->backend == NULL) {
	return NULL;
    }
    return mkdg_properties_load_by_key(self->properties, key, userData);
}

GValue *ibus_chewing_config_load_ibus_config(IBusChewingConfig * self,
					     GValue * value,
					     const gchar * section,
					     const gchar * key,
					     gpointer userData)
{
    if (self == NULL || self->backend == NULL) {
	return NULL;
    }
    return mkdg_backend_read(self->backend, value, section, key, userData);
}
