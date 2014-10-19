#include "IBusChewingBuffer.h"

IBusChewingBuffer *ibus_chewing_buffer_new(IBusChewingProperties *
					   iProperties)
{
    IBusChewingBuffer *self = g_new0(IBusChewingBuffer, 1);
    self->iProperties = iProperties;
    self->buffer = g_string_sized_new(IBUS_CHEWING_MAX_BYTES);
    self->wordLen = 0;

    /* Create chewing context */
    gchar buf[100];
    g_snprintf(buf, 100, "%s/.chewing", getenv("HOME"));
    gint ret = chewing_Init(QUOTE_ME(CHEWING_DATADIR_REAL), buf);
    if (ret) {
	IBUS_CHEWING_LOG(ERROR,
			 "ibus_chewing_buffer_new() chewing_Init(%s,%s)",
			 QUOTE_ME(CHEWING_DATADIR_REAL), buf);
    }
    IBUS_CHEWING_LOG(INFO, "init() chewing_new()");
    self->context = chewing_new();
    chewing_set_ChiEngMode(self->context, CHINESE_MODE);

    return self;
}

void ibus_chewing_buffer_free(IBusChewingBuffer * self)
{
    /* properties need not be freed here */
    chewing_delete(self->context);
    g_string_free(self->buffer, TRUE);
    g_free(self);
}

guint ibus_chewing_buffer_length(IBusChewingBuffer * self)
{
    return self->buffer->len;
}

guint ibus_chewing_buffer_word_length(IBusChewingBuffer * self)
{
    return self->wordLen;
}

guint ibus_chewing_buffer_word_limit(IBusChewingBuffer * self)
{
    return chewing_get_maxChiSymbolLen(self->context);
}

gchar *ibus_chewing_buffer_get_string(IBusChewingBuffer * self)
{
    return self->buffer->str;
}

void ibus_chewing_buffer_clear(IBusChewingBuffer * self)
{
    /* Clear chewing buffer */
    /* Save the orig Esc clean buffer state */
    gint origState = chewing_get_escCleanAllBuf(self->context);
    chewing_set_escCleanAllBuf(self->context, TRUE);
    chewing_handle_Esc(self->context);
    chewing_set_escCleanAllBuf(self->context, origState);

    g_string_assign(self->buffer, "");
    self->wordLen;
}

gboolean ibus_chewing_buffer_process_key(IBusChewingBuffer *self, guint keySym, guint keyCode, guint modifiers)
{
}
