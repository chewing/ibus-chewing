#include "MakerDialogUtil.h"
#include <glib.h>
#include <stdarg.h>

static MkdgLogLevel debugLevel = WARN;

#define MKDG_LOG_DOMAIN_LEN 20
static gchar mkdgLogDomain[MKDG_LOG_DOMAIN_LEN] = "MKDG";

void mkdg_log_set_level(MkdgLogLevel level) { debugLevel = level; }

void mkdg_logv_domain(const gchar *domain, MkdgLogLevel level,
                      const gchar *format, va_list argList) {
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

void mkdg_log(MkdgLogLevel level, const gchar *format, ...) {
    if (level > debugLevel)
        return;
    va_list argList;

    va_start(argList, format);
    mkdg_logv_domain(mkdgLogDomain, level, format, argList);
    va_end(argList);
}

void mkdg_log_domain(const gchar *domain, MkdgLogLevel level,
                     const gchar *format, ...) {
    if (level > debugLevel)
        return;
    va_list argList;

    va_start(argList, format);
    mkdg_logv_domain(domain, level, format, argList);
    va_end(argList);
}
