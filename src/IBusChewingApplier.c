#include <chewing.h>
#include "IBusChewingProperties.h"
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

/*============================================
 * IBus widgets
 */

IBusLookupTable *ibus_chewing_lookup_table_new(IBusChewingProperties *iProperties)
{
    gint size=g_value_get_int(mkdg_properties_get_by_key(iProperties->properties, "cand-per-page")); 
    gboolean cursorShow=FALSE;
    gboolean wrapAround=TRUE;
    IBusLookupTable *iTable=ibus_lookup_table_new
	(size, 0, cursorShow, wrapAround);

    GValue gValue={0};
    g_value_init(&gValue, G_TYPE_INT);
    if (ibus_chewing_properties_read_general(iProperties, &gValue, "ibus/panel", 
		"lookup-table-orientation", NULL)){
	ibus_lookup_table_set_orientation(iTable,g_value_get_int(&gValue));
    }
    g_value_unset(&gValue);
    return g_object_ref_sink(iTable);
}

void ibus_chewing_engine_set_selKeys_string(IBusChewingEngine * engine,
	const gchar * selKeys_str)
{
    g_assert(engine);
    int j;

    if (engine->table == NULL){
	g_assert(engine->iProperties);
	engine->table = ibus_chewing_lookup_table_new(engine->iProperties);
    }

    int len_min = MIN(strlen(selKeys_str), MAX_SELKEY);
    IBusText *iText;
    for (j = 0; j < len_min; j++) {
	engine->selKeys[j] = (gint) selKeys_str[j];
	iText =  g_object_ref_sink(ibus_text_new_from_unichar
		((gunichar) selKeys_str[j]));
	ibus_lookup_table_set_label(engine->table, j, iText);
    }
    chewing_set_selKey(engine->context, engine->selKeys, len_min);
}

/*============================================
 * Callback functions
 */
gboolean KBType_apply_callback(PropertyContext * ctx, gpointer userData)
{
    GValue *value = &(ctx->value);
    printf("KBType_apply_callback(%s,%s)", ctx->spec->key,
	   mkdg_g_value_to_string(value));
    IBUS_CHEWING_LOG(DEBUG, "KBType_apply_callback(%s,%s)", ctx->spec->key,
		     mkdg_g_value_to_string(value));
    ChewingKbType kbType = kbType_id_get_index(g_value_get_string(value));
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->parent;
    chewing_set_KBType(engine->context, kbType);
    return TRUE;
}

gboolean selKeys_apply_callback(PropertyContext * ctx, gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->parent;
    ibus_chewing_engine_set_selKeys_string(engine,
					   g_value_get_string(value));
    return TRUE;
}

gboolean hsuSelKeyType_apply_callback(PropertyContext * ctx,
				      gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->parent;
    chewing_set_hsuSelKeyType(engine->context, g_value_get_int(value));
    return TRUE;
}

gboolean autoShiftCur_apply_callback(PropertyContext * ctx,
				     gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->parent;
    chewing_set_autoShiftCur(engine->context,
			     (g_value_get_boolean(value)) ? 1 : 0);
    return TRUE;
}

gboolean addPhraseDirection_apply_callback(PropertyContext * ctx,
					   gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->parent;
    chewing_set_addPhraseDirection(engine->context,
				   (g_value_get_boolean(value)) ? 1 : 0);
    return TRUE;
}

gboolean easySymbolInput_apply_callback(PropertyContext * ctx,
					gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->parent;
    chewing_set_easySymbolInput(engine->context,
				(g_value_get_boolean(value)) ? 1 : 0);
    if (g_value_get_boolean(value)) {
	engine->chewingFlags |= CHEWING_FLAG_EASY_SYMBOL_INPUT;
    } else {
	engine->chewingFlags &= ~CHEWING_FLAG_EASY_SYMBOL_INPUT;
    }
    return TRUE;
}

gboolean escCleanAllBuf_apply_callback(PropertyContext * ctx,
				       gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->parent;
    chewing_set_escCleanAllBuf(engine->context,
			       (g_value_get_boolean(value)) ? 1 : 0);
    return TRUE;
}

/* Additional symbol buffer length */
gboolean maxChiSymbolLen_apply_callback(PropertyContext * ctx,
					gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->parent;
    chewing_set_maxChiSymbolLen(engine->context, g_value_get_int(value));
    return TRUE;
}

gboolean forceLowercaseEnglish_apply_callback(PropertyContext * ctx,
					      gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->parent;
    if (g_value_get_boolean(value)) {
	engine->chewingFlags |= CHEWING_FLAG_FORCE_LOWERCASE_ENGLISH;
    } else {
	engine->chewingFlags &= ~CHEWING_FLAG_FORCE_LOWERCASE_ENGLISH;
    }
    return TRUE;
}

gboolean syncCapsLock_apply_callback(PropertyContext * ctx,
				     gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->parent;
    const gchar *str = g_value_get_string(value);
    if (strcmp(str, "keyboard") == 0) {
	engine->syncCapsLock_local = CHEWING_MODIFIER_SYNC_FROM_KEYBOARD;
    } else if (strcmp(str, "input method") == 0) {
	engine->syncCapsLock_local = CHEWING_MODIFIER_SYNC_FROM_IM;
    } else {
	engine->syncCapsLock_local = CHEWING_MODIFIER_SYNC_DISABLE;
    }
    return TRUE;
}

gboolean numpadAlwaysNumber_apply_callback(PropertyContext * ctx,
					   gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->parent;
    if (g_value_get_boolean(value)) {
	engine->chewingFlags |= CHEWING_FLAG_NUMPAD_ALWAYS_NUMBER;
    } else {
	engine->chewingFlags &= ~CHEWING_FLAG_NUMPAD_ALWAYS_NUMBER;
    }
    return TRUE;
}

gboolean candPerPage_apply_callback(PropertyContext * ctx,
				    gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->parent;
    chewing_set_candPerPage(engine->context, g_value_get_int(value));
    if (engine->table) {
	ibus_lookup_table_clear(engine->table);
	engine->table->page_size = g_value_get_int(value);
    } else {
	engine->table = ibus_chewing_lookup_table_new(engine->iProperties);
    }
    return TRUE;
}

gboolean phraseChoiceRearward_apply_callback(PropertyContext * ctx,
					     gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->parent;
    chewing_set_phraseChoiceRearward(engine->context,
				     (g_value_get_boolean(value)) ? 1 : 0);
    return TRUE;
}

gboolean spaceAsSelection_apply_callback(PropertyContext * ctx,
					 gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->parent;
    chewing_set_spaceAsSelection(engine->context,
				 (g_value_get_boolean(value)) ? 1 : 0);
    return TRUE;
}

gboolean plainZhuyin_apply_callback(PropertyContext * ctx,
				    gpointer userData)
{
    GValue *value = &(ctx->value);
    IBusChewingEngine *engine = (IBusChewingEngine *) ctx->parent;
    if (g_value_get_boolean(value)) {
	engine->chewingFlags |= CHEWING_FLAG_PLAIN_ZHUYIN;
    } else {
	engine->chewingFlags &= ~CHEWING_FLAG_PLAIN_ZHUYIN;
    }
    return TRUE;
}
