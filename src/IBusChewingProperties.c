#define GETTEXT_PACKAGE "gtk20"
#include <glib.h>
#include <glib/gi18n.h>
#include "MakerDialogUtil.h"
#include "MakerDialogProperty.h"
#include "IBusChewingUtil.h"
#include "IBusChewingProperties.h"
#include "IBusConfigBackend.h"
#ifdef USE_GSETTINGS
#    include "GSettingsBackend.h"
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
#if CHEWING_CHECK_VERSION(0,3,4)
    N_("thl_pinying"),
    N_("mps2_pinyin"),
#endif
    NULL
};

#define SELKEYS_ARRAY_SIZE 8
const gchar *selKeys_array[SELKEYS_ARRAY_SIZE + 1] = {
    "1234567890",
    "asdfghjkl;",
    "asdfzxcv89",
    "asdfjkl789",
    "aoeu;qjkix",               /* Dvorak */
    "aoeuhtnsid",               /* Dvorak */
    "aoeuidhtns",               /* Dvorak */
    "1234qweras",
    NULL
};

const gchar *propDefaultEnglishLettercase_array[] = {
    N_("no default"),
    N_("lowercase"),
    N_("uppercase"),
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
     N_("Select Zhuyin keyboard layout"), NULL}
    ,
    {G_TYPE_STRING, "sel-keys", PAGE_KEYBOARD, N_("Selection keys"),
     IBUS_CHEWING_PROPERTIES_SUBSECTION, "1234567890", selKeys_array, NULL,
     0,
     0,
     selKeys_apply_callback,
     MKDG_PROPERTY_FLAG_NO_NEW,
     N_
     ("Keys used to select candidate. For example \"asdfghjkl;\", press 'a' to select the 1st candidate, 's' for 2nd, and so on."),
     NULL}
    ,
    /*== The callback will be handled in IBusChewingEngine.gob ==*/
    {G_TYPE_BOOLEAN, "show-systray", PAGE_KEYBOARD,
     N_("Show systray icons"),
     IBUS_CHEWING_PROPERTIES_SUBSECTION, "1", NULL, NULL, 0, 1,
     NULL, 0,
     N_
     ("On: Show Chinese/English and Full/Half shape status as a systray icon\n"
      "Off: Do not show the status icon"), NULL}
    ,
    {G_TYPE_BOOLEAN, "auto-shift-cur", PAGE_EDITING,
     N_("Auto move cursor"),
     IBUS_CHEWING_PROPERTIES_SUBSECTION, "1", NULL, NULL, 0, 1,
     autoShiftCur_apply_callback, 0,
     N_("Automatically move cursor to next character"), NULL}
    ,
    {G_TYPE_BOOLEAN, "add-phrase-direction", PAGE_EDITING,
     N_("Add phrases to the front"),
     IBUS_CHEWING_PROPERTIES_SUBSECTION, "1", NULL, NULL, 0, 1,
     addPhraseDirection_apply_callback, 0,
     N_("Add phrases to the front"), NULL}
    ,
    {G_TYPE_BOOLEAN, "clean-buffer-focus-out", PAGE_EDITING,
     N_("Clean pre-edit buffer when focus out"),
     IBUS_CHEWING_PROPERTIES_SUBSECTION, "0", NULL, NULL, 0, 1,
     cleanBufferFocusOut_apply_callback, 0,
     N_
     ("On: Clean pre-edit buffer when focus out to prevent program crash\n"
      "Off: Keep what you already type for convenience"), NULL}
    ,
    {G_TYPE_BOOLEAN, "easy-symbol-input", PAGE_EDITING,
     N_("Easy symbol input"),
     IBUS_CHEWING_PROPERTIES_SUBSECTION, "1", NULL, NULL, 0, 1,
     easySymbolInput_apply_callback, 0,
     N_("Press shift to input Chinese punctuation symbols"), NULL}
    ,
    {G_TYPE_BOOLEAN, "esc-clean-all-buf", PAGE_EDITING,
     N_("Esc clean all buffer"),
     IBUS_CHEWING_PROPERTIES_SUBSECTION, "0", NULL, NULL, 0, 1,
     escCleanAllBuf_apply_callback, 0,
     N_("Escape key cleans the text in pre-edit-buffer"), NULL}
    ,
    {G_TYPE_INT, "max-chi-symbol-len", PAGE_EDITING,
     N_("Maximum Chinese characters"),
     IBUS_CHEWING_PROPERTIES_SUBSECTION, "20", NULL, NULL, 11, 33,
     maxChiSymbolLen_apply_callback, 0,
     N_
     ("Maximum Chinese characters in pre-edit buffer, not including inputing Zhuyin symbols."),
     NULL}
    ,
    /* Sync between CapsLock and IM */
    {G_TYPE_STRING, "sync-caps-lock", PAGE_EDITING,
     N_("Sync between CapsLock and IM"),
     IBUS_CHEWING_PROPERTIES_SUBSECTION, "disable", syncCapsLock_strs,
     "Sync",
     0, 1,
     syncCapsLock_apply_callback,
     MKDG_PROPERTY_FLAG_NO_NEW | MKDG_PROPERTY_FLAG_HAS_TRANSLATION,
     N_
     ("Occasionally, the CapsLock status does not match the IM, this option determines how these status be synchronized. Valid values:\n"
      "\"disable\": Do nothing\n"
      "\"keyboard\": IM status follows keyboard status\n"
      "\"IM\": Keyboard status follows IM status"), NULL}
    ,
    {G_TYPE_BOOLEAN, "shift-toggle-chinese", PAGE_EDITING,
     N_("Shift toggle Chinese Mode"),
     IBUS_CHEWING_PROPERTIES_SUBSECTION, "1", NULL, NULL, 0, 1,
     shiftToggleChinese_apply_callback, 0,
     N_("Shift key to toggle Chinese Mode"), NULL}
    ,
    {G_TYPE_BOOLEAN, "capslock-toggle-chinese", PAGE_EDITING,
     N_("Caps Lock toggles Chinese Mode"),
     IBUS_CHEWING_PROPERTIES_SUBSECTION, "1", NULL, NULL, 0, 1,
     capslockToggleChinese_apply_callback, 0,
     N_("On: Caps Lock toggle Chinese/English\n"
        "Off: Caps Lock only affect English letter case"), NULL}
    ,
    {
     G_TYPE_STRING, "default-english-case", PAGE_EDITING,
     N_("Default English letter case\n"
        "  (Only effective when Caps Lock toggles Chinese is ON)"),
     IBUS_CHEWING_PROPERTIES_SUBSECTION, "lowercase",
     propDefaultEnglishLettercase_array, NULL, 0, 1,
     defaultEnglishLetterCase_apply_callback,
     MKDG_PROPERTY_FLAG_NO_NEW | MKDG_PROPERTY_FLAG_HAS_TRANSLATION,
     N_
     ("no control: No default letter case. Not recommend if you use multiple keyboards or synergy\n"
      "lowercase: Default to lowercase, press shift for uppercase.\n"
      "uppercase: Default to uppercase, press shift for lowercase."),
     NULL}
    ,
    {
     G_TYPE_BOOLEAN, "plain-zhuyin", PAGE_SELECTING,
     N_("Plain Zhuyin mode"),
     IBUS_CHEWING_PROPERTIES_SUBSECTION, "0", NULL, NULL, 0, 1,
     plainZhuyin_apply_callback, 0,
     N_
     ("In plain Zhuyin mode, automatic candidate selection and related options are disabled or ignored."),
     NULL}
    ,
    {
     G_TYPE_UINT, "cand-per-page", PAGE_SELECTING,
     N_("Candidate per page"),
     IBUS_CHEWING_PROPERTIES_SUBSECTION, "5", NULL, NULL, 4, 10,
     candPerPage_apply_callback, 0,
     N_("Number of candidate per page."),
     NULL}
    ,
    {
     G_TYPE_BOOLEAN, "show-page-number", PAGE_SELECTING,
     N_("Show page number"),
     IBUS_CHEWING_PROPERTIES_SUBSECTION, "0", NULL, NULL, 0, 1,
     showPageNumber_apply_callback, 0,
     N_("Display the page number of the candidate list."),
     NULL}
    ,
    {
     G_TYPE_BOOLEAN, "phrase-choice-from-last", PAGE_SELECTING,
     N_("Choose phrases from backward"),
     IBUS_CHEWING_PROPERTIES_SUBSECTION, "1", NULL, NULL, 0, 1,
     phraseChoiceRearward_apply_callback, 0,
     N_("Choose phrases from the back, without moving cursor."),
     NULL}
    ,
    {
     G_TYPE_BOOLEAN, "space-as-selection", PAGE_SELECTING,
     N_("Space to select"),
     IBUS_CHEWING_PROPERTIES_SUBSECTION,
     "0", NULL, NULL, 0, 1,
     spaceAsSelection_apply_callback, 0,
     "Press Space to select the candidate.",
     NULL}
    ,
    {
     G_TYPE_INVALID, "", "", "",
     IBUS_CHEWING_PROPERTIES_SUBSECTION, "", NULL, NULL, 0, 0,
     NULL, 0, NULL, NULL}
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
        mkdg_properties_from_spec_array(propSpecs, backend, parent, auxData);

    /* In schema generation, backend is NULL */
#ifdef USE_GSETTINGS
    self->confObjTable = g_hash_table_new(g_str_hash, g_str_equal);
#else
    self->confObjTable = NULL;
#endif
    return self;
}

#ifdef USE_GSETTINGS
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
#endif                          /* USE_GSETTINGS */

GValue *ibus_chewing_properties_read_general(IBusChewingProperties * self,
                                             GValue * value,
                                             const gchar * section,
                                             const gchar * key,
                                             gpointer userData)
{
    g_assert(self);
    g_assert(value);
#ifdef USE_GSETTINGS
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
#endif                          /* USE_GSETTINGS */
    return mkdg_backend_read(self->properties->backend, value, section,
                             key, userData);
}

gboolean
ibus_chewing_properties_read_boolean_general(IBusChewingProperties
                                             * self,
                                             const gchar *
                                             section,
                                             const gchar * key,
                                             gpointer userData)
{
    GValue gValue = { 0 };
    g_value_init(&gValue, G_TYPE_BOOLEAN);
    ibus_chewing_properties_read_general(self, &gValue, section, key, userData);
    gboolean result = g_value_get_boolean(&gValue);

    g_value_unset(&gValue);
    return result;
}

gint
ibus_chewing_properties_read_int_general(IBusChewingProperties * self,
                                         const gchar * section,
                                         const gchar * key, gpointer userData)
{
    GValue gValue = { 0 };
    g_value_init(&gValue, G_TYPE_INT);
    ibus_chewing_properties_read_general(self, &gValue, section, key, userData);
    gint result = g_value_get_int(&gValue);

    g_value_unset(&gValue);
    return result;
}
