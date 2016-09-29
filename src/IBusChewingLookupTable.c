#include "IBusChewingUtil.h"
#include "IBusChewingLookupTable.h"

IBusLookupTable *ibus_chewing_lookup_table_new(IBusChewingProperties *
                                               iProperties,
                                               ChewingContext * context)
{
    guint size = mkdg_properties_get_uint_by_key
        (iProperties->properties, "cand-per-page");
    gboolean cursorShow = FALSE;
    gboolean wrapAround = TRUE;
    IBusLookupTable *iTable = ibus_lookup_table_new
        (size, 0, cursorShow, wrapAround);

    ibus_chewing_lookup_table_resize(iTable, iProperties, context);
    return iTable;
}

void ibus_chewing_lookup_table_resize(IBusLookupTable * iTable,
                                      IBusChewingProperties * iProperties,
                                      ChewingContext * context)
{
    gint selKSym[MAX_SELKEY];
    const gchar *selKeyStr =
        mkdg_properties_get_string_by_key(iProperties->properties, "sel-keys");
    guint candPerPage =
        mkdg_properties_get_uint_by_key(iProperties->properties, "cand-per-page");

    int len = MIN(strlen(selKeyStr), MAX_SELKEY);
    len = MIN(len, candPerPage);
    IBusText *iText;
    int i;
    if (iTable != NULL) {
        ibus_lookup_table_clear(iTable);
        for (i = 0; i < len; i++) {
            selKSym[i] = (gint) selKeyStr[i];
            iText = g_object_ref_sink(ibus_text_new_from_unichar
		                  ((gunichar) selKeyStr[i]));
            ibus_lookup_table_set_label(iTable, i, iText);
            g_object_unref(iText);
        }
    }
    chewing_set_candPerPage(context, len);
    chewing_set_selKey(context, selKSym, len);
}

guint ibus_chewing_lookup_table_update(IBusLookupTable * iTable,
				       IBusChewingProperties * iProperties,
				       ChewingContext * context)
{
    ibus_chewing_lookup_table_resize(iTable, iProperties, context);
    IBusText *iText = NULL;
    guint i;
    gint choicePerPage = chewing_cand_ChoicePerPage(context);
    gint totalChoice = chewing_cand_TotalChoice(context);
    gint currentPage = chewing_cand_CurrentPage(context);
    IBUS_CHEWING_LOG(INFO,
		     "***** ibus_chewing_lookup_table_update(): choicePerPage=%d, totalChoice=%d, currentPage=%d",
		     choicePerPage, totalChoice, currentPage);
    chewing_cand_Enumerate(context);
    for (i = 0; i < choicePerPage; i++) {
	if (chewing_cand_hasNext(context)) {
	    gchar *candidate = chewing_cand_String(context);
	    iText =
		g_object_ref_sink(ibus_text_new_from_string(candidate));
	    ibus_lookup_table_append_candidate(iTable, iText);
	    g_free(candidate);
	    g_object_unref(iText);
	} else {
	    break;
	}
    }
    return i;
}
