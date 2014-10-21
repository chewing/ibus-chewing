#include <chewing.h>
#include "IBusChewingPreEdit.h"
#include "IBusChewingLookupTable.h"
#include "ibus-chewing-engine.h"

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

/*
 * Only refresh the parameter, not the table itself
 * So no need to depend IBusChewingEngine at this point
 */
static void lookup_table_properties_refresh(IBusChewingPreEdit * icBuffer)
{
    IBusChewingProperties *iProperties = icBuffer->iProperties;
    ChewingContext *context = icBuffer->context;
    gint selKSym[MAX_SELKEY];
    const gchar *selKeyStr =
	mkdg_properties_get_string_by_key(iProperties->properties,
					       "sel-keys");
    gint candPerPage =
	mkdg_properties_get_int_by_key(iProperties->properties,
					    "cand-per-page");

    gint len = MIN(strlen(selKeyStr), MAX_SELKEY);
    len = MIN(len, candPerPage);
    gint i;
    for (i = 0; i < len; i++) {
	selKSym[i] = (gint) selKeyStr[i];
    }
    chewing_set_candPerPage(context, len);
    chewing_set_selKey(context, selKSym, len);
}


/*============================================
 * Callback functions
 */
gboolean KBType_apply_callback(PropertyContext * ctx, gpointer userData)
{
    GValue *value = &(ctx->value);
    IBUS_CHEWING_LOG(DEBUG, "KBType_apply_callback(%s,%s)", ctx->spec->key,
		     mkdg_g_value_to_string(value));
    ChewingKbType kbType = kbType_id_get_index(g_value_get_string(value));
    IBusChewingPreEdit *icBuffer = (IBusChewingPreEdit *) ctx->parent;
    chewing_set_KBType(icBuffer->context, kbType);
    return TRUE;
}

gboolean selKeys_apply_callback(PropertyContext * ctx, gpointer userData)
{
    GValue *value = &(ctx->value);
    IBUS_CHEWING_LOG(DEBUG, "selKeys_apply_callback(%s,%s)",
		     ctx->spec->key, mkdg_g_value_to_string(value));
    IBusChewingPreEdit *icBuffer = (IBusChewingPreEdit *) ctx->parent;
    lookup_table_properties_refresh(icBuffer);
    return TRUE;
}

gboolean hsuSelKeyType_apply_callback(PropertyContext * ctx,
				      gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingPreEdit *icBuffer = (IBusChewingPreEdit *) ctx->parent;
    chewing_set_hsuSelKeyType(icBuffer->context, g_value_get_int(value));
    return TRUE;
}

gboolean autoShiftCur_apply_callback(PropertyContext * ctx,
				     gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingPreEdit *icBuffer = (IBusChewingPreEdit *) ctx->parent;
    chewing_set_autoShiftCur(icBuffer->context,
			     (g_value_get_boolean(value)) ? 1 : 0);
    return TRUE;
}

gboolean addPhraseDirection_apply_callback(PropertyContext * ctx,
					   gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingPreEdit *icBuffer = (IBusChewingPreEdit *) ctx->parent;
    chewing_set_addPhraseDirection(icBuffer->context,
				   (g_value_get_boolean(value)) ? 1 : 0);
    return TRUE;
}

gboolean easySymbolInput_apply_callback(PropertyContext * ctx,
					gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingPreEdit *icBuffer = (IBusChewingPreEdit *) ctx->parent;
    chewing_set_easySymbolInput(icBuffer->context,
				(g_value_get_boolean(value)) ? 1 : 0);
    /* Use MkdgProperty directly, no need to keep flag */
    return TRUE;
}

gboolean escCleanAllBuf_apply_callback(PropertyContext * ctx,
				       gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingPreEdit *icBuffer = (IBusChewingPreEdit *) ctx->parent;
    chewing_set_escCleanAllBuf(icBuffer->context,
			       (g_value_get_boolean(value)) ? 1 : 0);
    return TRUE;
}

/* Additional symbol buffer length */
gboolean maxChiSymbolLen_apply_callback(PropertyContext * ctx,
					gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingPreEdit *icBuffer = (IBusChewingPreEdit *) ctx->parent;
    chewing_set_maxChiSymbolLen(icBuffer->context, g_value_get_int(value));
    return TRUE;
}

gboolean forceLowercaseEnglish_apply_callback(PropertyContext * ctx,
					      gpointer userData)
{
    /* Use MkdgProperty directly, no need to keep flag */
    return TRUE;
}

gboolean syncCapsLock_apply_callback(PropertyContext * ctx,
				     gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingPreEdit *icBuffer = (IBusChewingPreEdit *) ctx->parent;
    const gchar *str = g_value_get_string(value);
    if (strcmp(str, "keyboard") == 0) {
	ibus_chewing_pre_edit_set_flag(icBuffer, FLAG_SYNC_FROM_KEYBOARD);
	ibus_chewing_pre_edit_clear_flag(icBuffer, FLAG_SYNC_FROM_IM);

    } else if (strcmp(str, "input method") == 0) {
	ibus_chewing_pre_edit_set_flag(icBuffer, FLAG_SYNC_FROM_IM);
	ibus_chewing_pre_edit_clear_flag(icBuffer, FLAG_SYNC_FROM_KEYBOARD);
    } else {
	ibus_chewing_pre_edit_clear_flag(icBuffer,
				       FLAG_SYNC_FROM_IM |
				       FLAG_SYNC_FROM_KEYBOARD);
    }
    return TRUE;
}

gboolean numpadAlwaysNumber_apply_callback(PropertyContext * ctx,
					   gpointer userData)
{
    /* Use MkdgProperty directly, no need to keep flag */
    return TRUE;
}

gboolean candPerPage_apply_callback(PropertyContext * ctx,
				    gpointer userData)
{
    IBusChewingPreEdit *icBuffer = (IBusChewingPreEdit *) ctx->parent;
    lookup_table_properties_refresh(icBuffer);
    return TRUE;
}

gboolean phraseChoiceRearward_apply_callback(PropertyContext * ctx,
					     gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingPreEdit *icBuffer = (IBusChewingPreEdit *) ctx->parent;
    chewing_set_phraseChoiceRearward(icBuffer->context,
				     (g_value_get_boolean(value)) ? 1 : 0);
    return TRUE;
}

gboolean spaceAsSelection_apply_callback(PropertyContext * ctx,
					 gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingPreEdit *icBuffer = (IBusChewingPreEdit *) ctx->parent;
    chewing_set_spaceAsSelection(icBuffer->context,
				 (g_value_get_boolean(value)) ? 1 : 0);
    return TRUE;
}

gboolean plainZhuyin_apply_callback(PropertyContext * ctx,
				    gpointer userData)
{
    /* Use MkdgProperty directly, no need to keep flag */
    return TRUE;
}
