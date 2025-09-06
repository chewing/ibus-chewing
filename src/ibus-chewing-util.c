/**
 * Utility routines that do not depend on
 * IBusChewingEngine
 */

#include "ibus-chewing-util.h"

/*=====================================
 * Tone
 */
const gchar *toneKeys[] = {"6347", // Default
                           "jfds", // hsu
                           "uiop", // ibm
                           "zaq1", // gin-yieh
                           "4321", // eten
                           "kjfd", // eten26
                           "6347", // dvorak
                           "thdn", // dvorak_hsu
                           "yert", // dachen_26
                           "1234", // hanyu
                           NULL};

/*=====================================
 * Key
 */

KSym key_sym_KP_to_normal(KSym k) {
    if (k < IBUS_KP_0 || k > IBUS_KP_9) {
        switch (k) {
        case IBUS_KP_Multiply:
            return IBUS_asterisk;
        case IBUS_KP_Add:
            return IBUS_plus;
        case IBUS_KP_Separator:
            return IBUS_comma;
        case IBUS_KP_Subtract:
            return IBUS_minus;
        case IBUS_KP_Decimal:
            return IBUS_period;
        case IBUS_KP_Divide:
            return IBUS_slash;
        default:
            return 0;
        }
    }
    return k - IBUS_KP_0 + IBUS_0;
}

/*=====================================
 * Modifiers
 */

static const gchar *modifier_get_string(guint modifier) {
    switch (modifier) {
    case IBUS_SHIFT_MASK:
        return "SHIFT";
    case IBUS_LOCK_MASK:
        return "LOCK";
    case IBUS_CONTROL_MASK:
        return "CONTROL";
    case IBUS_MOD1_MASK:
        return "MOD1";
    case IBUS_MOD2_MASK:
        return "MOD2";
    case IBUS_MOD3_MASK:
        return "MOD3";
    case IBUS_MOD4_MASK:
        return "MOD4";
    case IBUS_MOD5_MASK:
        return "MOD5";
    case IBUS_HANDLED_MASK:
        return "HANDLED";
    case IBUS_FORWARD_MASK:
        return "FORWARD";
    case IBUS_SUPER_MASK:
        return "SUPER";
    case IBUS_HYPER_MASK:
        return "HYPER";
    case IBUS_META_MASK:
        return "META";
    case IBUS_RELEASE_MASK:
        return "RELEASE";
    default:
        break;
    }
    return "UNKN";
}

#define MODIFIER_BUFFER_SIZE 128
const gchar *modifiers_to_string(guint modifier) {
    static gchar modifierBuf[MODIFIER_BUFFER_SIZE];

    g_strlcpy(modifierBuf, "", MODIFIER_BUFFER_SIZE);
    gboolean first = TRUE;
    gint i, mask;

    for (i = 0; i < 32; i++) {
        mask = 1 << i;
        if (modifier & mask) {
            if (first) {
                g_strlcpy(modifierBuf, modifier_get_string(mask), MODIFIER_BUFFER_SIZE);
                first = FALSE;
            } else {
                g_strlcat(modifierBuf, "|", MODIFIER_BUFFER_SIZE);
                g_strlcat(modifierBuf, modifier_get_string(mask), MODIFIER_BUFFER_SIZE);
            }
        }
    }
    return modifierBuf;
}

static MkdgLogLevel debugLevel = WARN;

void mkdg_log_set_level(MkdgLogLevel level) { debugLevel = level; }

void mkdg_logv_domain(const gchar *domain, MkdgLogLevel level, const gchar *format,
                      va_list argList) {
    if (level > debugLevel)
        return;
    GLogLevelFlags flagSet;

    switch (level) {
    case ERROR:
        flagSet = G_LOG_FLAG_FATAL | G_LOG_LEVEL_ERROR;
        break;
    case WARN:
        flagSet = G_LOG_LEVEL_WARNING;
        break;
    case MSG:
        flagSet = G_LOG_LEVEL_MESSAGE;
        break;
    case INFO:
        flagSet = G_LOG_LEVEL_INFO;
        break;
    default:
        flagSet = G_LOG_LEVEL_DEBUG;
        break;
    }
    g_logv(domain, flagSet, format, argList);
}

void mkdg_log_domain(const gchar *domain, MkdgLogLevel level, const gchar *format, ...) {
    if (level > debugLevel)
        return;
    va_list argList;

    va_start(argList, format);
    mkdg_logv_domain(domain, level, format, argList);
    va_end(argList);
}
