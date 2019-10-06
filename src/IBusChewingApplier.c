#include <chewing.h>
#include "IBusChewingPreEdit.h"
#include "IBusChewingLookupTable.h"

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


/*============================================
 * Callback routines
 */
gboolean KBType_apply_callback(PropertyContext * ctx, gpointer userData)
{
    GValue *value = &(ctx->value);

    IBUS_CHEWING_LOG(DEBUG, "KBType_apply_callback(%s,%s)", ctx->spec->key,
                     mkdg_g_value_to_string(value));
    ChewingKbType kbType = kbType_id_get_index(g_value_get_string(value));
    IBusChewingPreEdit *icPreEdit = (IBusChewingPreEdit *) ctx->parent;

    chewing_set_KBType(icPreEdit->context, kbType);
    return TRUE;
}

gboolean selKeys_apply_callback(PropertyContext * ctx, gpointer userData)
{
    GValue *value = &(ctx->value);

    IBUS_CHEWING_LOG(DEBUG, "selKeys_apply_callback(%s,%s)",
                     ctx->spec->key, mkdg_g_value_to_string(value));
    IBusChewingPreEdit *icPreEdit = (IBusChewingPreEdit *) ctx->parent;

    ibus_chewing_lookup_table_resize(icPreEdit->iTable,
                                     icPreEdit->iProperties,
                                     icPreEdit->context);
    return TRUE;
}

gboolean autoShiftCur_apply_callback(PropertyContext * ctx, gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingPreEdit *icPreEdit = (IBusChewingPreEdit *) ctx->parent;

    chewing_set_autoShiftCur(icPreEdit->context,
                             (g_value_get_boolean(value)) ? 1 : 0);
    return TRUE;
}

gboolean
addPhraseDirection_apply_callback(PropertyContext * ctx, gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingPreEdit *icPreEdit = (IBusChewingPreEdit *) ctx->parent;

    chewing_set_addPhraseDirection(icPreEdit->context,
                                   (g_value_get_boolean(value)) ? 1 : 0);
    return TRUE;
}


gboolean
cleanBufferFocusOut_apply_callback(PropertyContext * ctx, gpointer userData)
{
    return TRUE;
}


gboolean
easySymbolInput_apply_callback(PropertyContext * ctx, gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingPreEdit *icPreEdit = (IBusChewingPreEdit *) ctx->parent;

    chewing_set_easySymbolInput(icPreEdit->context,
                                (g_value_get_boolean(value)) ? 1 : 0);
    /* Use MkdgProperty directly, no need to call IBusChewingEngine */
    return TRUE;
}

gboolean escCleanAllBuf_apply_callback(PropertyContext * ctx, gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingPreEdit *icPreEdit = (IBusChewingPreEdit *) ctx->parent;

    chewing_set_escCleanAllBuf(icPreEdit->context,
                               (g_value_get_boolean(value)) ? 1 : 0);
    return TRUE;
}

/* Additional symbol buffer length */
gboolean
maxChiSymbolLen_apply_callback(PropertyContext * ctx, gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingPreEdit *icPreEdit = (IBusChewingPreEdit *) ctx->parent;

    chewing_set_maxChiSymbolLen(icPreEdit->context, g_value_get_int(value) + 5);        /* 5 for incomplete bopomofos */
    return TRUE;
}

gboolean
    defaultEnglishLetterCase_apply_callback
    (PropertyContext * ctx, gpointer userData) {
    /* Use MkdgProperty directly, no need to call IBusChewingEngine */
    return TRUE;
}

gboolean syncCapsLock_apply_callback(PropertyContext * ctx, gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingPreEdit *icPreEdit = (IBusChewingPreEdit *) ctx->parent;
    const gchar *str = g_value_get_string(value);

    if (strcmp(str, "keyboard") == 0) {
        ibus_chewing_pre_edit_set_flag(icPreEdit, FLAG_SYNC_FROM_KEYBOARD);
        ibus_chewing_pre_edit_clear_flag(icPreEdit, FLAG_SYNC_FROM_IM);
    } else if (strcmp(str, "input method") == 0) {
        ibus_chewing_pre_edit_set_flag(icPreEdit, FLAG_SYNC_FROM_IM);
        ibus_chewing_pre_edit_clear_flag(icPreEdit, FLAG_SYNC_FROM_KEYBOARD);
    } else {
        ibus_chewing_pre_edit_clear_flag(icPreEdit,
                                         FLAG_SYNC_FROM_IM
                                         | FLAG_SYNC_FROM_KEYBOARD);
    }
    return TRUE;
}

gboolean
shiftToggleChinese_apply_callback(PropertyContext * ctx, gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingPreEdit *icPreEdit = (IBusChewingPreEdit *) ctx->parent;

    ibus_chewing_pre_edit_set_property_boolean
        (icPreEdit, "shift-toggle-chinese", g_value_get_boolean(value));
    return TRUE;
}

gboolean candPerPage_apply_callback(PropertyContext * ctx, gpointer userData)
{
    IBusChewingPreEdit *icPreEdit = (IBusChewingPreEdit *) ctx->parent;

    ibus_chewing_lookup_table_resize(icPreEdit->iTable,
                                     icPreEdit->iProperties,
                                     icPreEdit->context);
    return TRUE;
}

gboolean
    capslockToggleChinese_apply_callback
    (PropertyContext * ctx, gpointer userData) {
    /* Use MkdgProperty directly, no need to call IBusChewingEngine */
    return TRUE;
}

gboolean showPageNumber_apply_callback(PropertyContext * ctx, gpointer userData)
{
    /* Use MkdgProperty directly, no need to call IBusChewingEngine */
    return TRUE;
}

gboolean
    phraseChoiceRearward_apply_callback
    (PropertyContext * ctx, gpointer userData) {
    GValue *value = &(ctx->value);
    IBusChewingPreEdit *icPreEdit = (IBusChewingPreEdit *) ctx->parent;

    chewing_set_phraseChoiceRearward(icPreEdit->context,
                                     (g_value_get_boolean(value)) ? 1 : 0);
    return TRUE;
}

gboolean
spaceAsSelection_apply_callback(PropertyContext * ctx, gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingPreEdit *icPreEdit = (IBusChewingPreEdit *) ctx->parent;

    chewing_set_spaceAsSelection(icPreEdit->context,
                                 (g_value_get_boolean(value)) ? 1 : 0);
    return TRUE;
}

gboolean plainZhuyin_apply_callback(PropertyContext * ctx, gpointer userData)
{
    /* Use MkdgProperty directly, no need to call IBusChewingEngine */
    return TRUE;
}
