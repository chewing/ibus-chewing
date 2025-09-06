#include "ibus-chewing-lookup-table.h"
#include "ibus-chewing-util.h"
#include <ctype.h>

IBusLookupTable *ibus_chewing_lookup_table_new(ChewingContext *context) {
    guint size = 10;
    gboolean cursorShow = TRUE;
    gboolean wrapAround = TRUE;
    IBusLookupTable *iTable = ibus_lookup_table_new(size, 0, cursorShow, wrapAround);

    ibus_chewing_lookup_table_resize(iTable, context);

    return iTable;
}

void ibus_chewing_lookup_table_resize(IBusLookupTable *iTable, ChewingContext *context) {
    gint selKSym[MAX_SELKEY];
    g_autoptr(GSettings) settings = g_settings_new(QUOTE_ME(PROJECT_SCHEMA_ID));
    g_autofree char *selKeyStr = g_settings_get_string(settings, "sel-keys");
    guint candPerPage = g_settings_get_uint(settings, "cand-per-page");

    /* Users are allowed to specify their own selKeys,
     * we have to check the length and take the smaller one.
     */
    int len = MIN(strlen(selKeyStr), candPerPage);
    int i;
    IBusText *iText;

    if (iTable != NULL) {
        ibus_lookup_table_set_page_size(iTable, len);

        for (i = 0; i < len; i++) {
            selKSym[i] = (gint)selKeyStr[i];

            iText = g_object_ref_sink(ibus_text_new_from_printf("%c.", toupper(selKeyStr[i])));
            ibus_lookup_table_set_label(iTable, i, iText);
            g_object_unref(iText);
        }
    }
    chewing_set_candPerPage(context, len);
    chewing_set_selKey(context, selKSym, MAX_SELKEY);

    gboolean verticalLookupTable = g_settings_get_boolean(settings, "vertical-lookup-table");

    ibus_lookup_table_set_orientation(iTable, verticalLookupTable);
}

guint ibus_chewing_lookup_table_update(IBusLookupTable *iTable, ChewingContext *context) {
    IBusText *iText = NULL;
    gint i;
    gint choicePerPage = chewing_cand_ChoicePerPage(context);
    gint totalChoice = chewing_cand_TotalChoice(context);
    gint currentPage = chewing_cand_CurrentPage(context);

    IBUS_CHEWING_LOG(INFO,
                     "***** ibus_chewing_lookup_table_update(): "
                     "choicePerPage=%d, totalChoice=%d, currentPage=%d",
                     choicePerPage, totalChoice, currentPage);

    ibus_lookup_table_clear(iTable);
    chewing_cand_Enumerate(context);
    for (i = 0; i < choicePerPage; i++) {
        if (chewing_cand_hasNext(context)) {
            gchar *candidate = chewing_cand_String(context);

            iText = g_object_ref_sink(ibus_text_new_from_string(candidate));
            ibus_lookup_table_append_candidate(iTable, iText);
            chewing_free(candidate);
            g_object_unref(iText);
        } else {
            break;
        }
    }
    return i;
}
