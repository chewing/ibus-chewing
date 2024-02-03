/* vim:set et sts=4: */
/*
 * Copyright © 2009  Red Hat, Inc. All rights reserved.
 * Copyright © 2009  Ding-Yi Chen <dchen at redhat.com>
 *
 * This file is part of the ibus-chewing Project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <ibus.h>
#include <stdlib.h>
#include <locale.h>
#include <chewing.h>
#include <glib/gi18n.h>
#include "ibus-chewing-engine.h"
#include "IBusChewingUtil.h"
#include "maker-dialog.h"

MakerDialog *makerDialog = NULL;
static IBusBus *bus = NULL;
static IBusFactory *factory = NULL;

/* options */
static gboolean showFlags = FALSE;
static gboolean ibus = FALSE;
static gboolean xml = FALSE;
gint ibus_chewing_verbose = VERBOSE_LEVEL;


static const GOptionEntry entries[] = {
    {"show_flags", 's', 0, G_OPTION_ARG_NONE, &showFlags,
     "Show compile flag only", NULL},
    {"ibus", 'i', 0, G_OPTION_ARG_NONE, &ibus,
     "component is executed by ibus", NULL},
    {"verbose", 'v', 0, G_OPTION_ARG_INT, &ibus_chewing_verbose,
     "Verbose level. The higher the level, the more the debug messages.",
     "[integer]"},
    {"xml", 'x', 0, G_OPTION_ARG_NONE, &xml,
     "read chewing engine desc from xml file", NULL},
    {NULL},
};


static void ibus_disconnected_cb(IBusBus * bus, gpointer user_data)
{
    g_debug("bus disconnected");
    ibus_quit();
}

static void start_component(void)
{
    IBUS_CHEWING_LOG(INFO, "start_component");
    ibus_init();
    bus = ibus_bus_new();
    g_signal_connect(bus, "disconnected", G_CALLBACK(ibus_disconnected_cb),
                     NULL);

    if (!ibus_bus_is_connected(bus)) {
        IBUS_CHEWING_LOG(ERROR, _("Cannot connect to IBus!"));
        exit(2);
    }

    IBusComponent *component = NULL;

    if (xml) {
        component = ibus_component_new_from_file(QUOTE_ME(DATA_DIR)
                                                 "/ibus/component/chewing.xml");
    } else {
        component = ibus_component_new(QUOTE_ME(PROJECT_SCHEMA_ID),
                                       _("Chewing component"),
                                       QUOTE_ME(PRJ_VER), "GPLv2+",
                                       _("Peng Huang, Ding-Yi Chen"),
                                       "http://code.google.com/p/ibus",
                                       QUOTE_ME(LIBEXEC_DIR)
                                       "/ibus-engine-chewing --ibus",
                                       QUOTE_ME(PROJECT_NAME));
    }

    IBusEngineDesc *engineDesc =
        ibus_engine_desc_new_varargs
            ("name", "chewing",
             "longname", _("Chewing"),
             "description", _("Chinese chewing input method"),
             "language", "zh_TW",
             "license", "GPLv2+",
             "author", _("Peng Huang, Ding-Yi Chen"),
             "icon", QUOTE_ME(PRJ_DATA_DIR)"/icons/"QUOTE_ME(PROJECT_NAME)".png",
             "layout", "us",
             "setup", QUOTE_ME(LIBEXEC_DIR)"/ibus-setup-chewing",
             "version", QUOTE_ME(PRJ_VER),
             "textdomain", QUOTE_ME(PROJECT_NAME),
             NULL);

    ibus_component_add_engine(component, engineDesc);

    factory = ibus_factory_new(ibus_bus_get_connection(bus));
    ibus_factory_add_engine(factory, "chewing", IBUS_TYPE_CHEWING_ENGINE);

    if (ibus) {
        guint32 ret =
            ibus_bus_request_name(bus, QUOTE_ME(PROJECT_SCHEMA_ID), 0);
        IBUS_CHEWING_LOG(INFO, "start_component: request_name: %u", ret);
    } else {
        ibus_bus_register_component(bus, component);
    }

    g_object_unref(component);
    ibus_main();
}

const char *locale_env_strings[] = {
    "LC_ALL",
    "LANG",
    "LANGUAGE",
    "GDM_LANG",
    NULL
};

void determine_locale()
{
#ifndef STRING_BUFFER_SIZE
#    define STRING_BUFFER_SIZE 100
#endif
    gchar *localePtr = NULL;
    gchar localeStr[STRING_BUFFER_SIZE];
    int i;

    for (i = 0; locale_env_strings[i] != NULL; i++) {
        if (getenv(locale_env_strings[i])) {
            localePtr = getenv(locale_env_strings[i]);
            break;
        }
    }
    if (!localePtr) {
        localePtr = "en_US.utf8";
    }
    /* Use UTF8 as charset unconditionally */
    for (i = 0; localePtr[i] != '\0'; i++) {
        if (localePtr[i] == '.')
            break;
        localeStr[i] = localePtr[i];
    }
    localeStr[i] = '\0';
    g_strlcat(localeStr, ".utf8", STRING_BUFFER_SIZE);
#undef STRING_BUFFER_SIZE
    setlocale(LC_ALL, localeStr);
    IBUS_CHEWING_LOG(INFO, "determine_locale %s", localeStr);
}


int main(gint argc, gchar * argv[])
{
    GError *error = NULL;
    GOptionContext *context;

    gtk_init();

    /* Init i18n messages */
    setlocale(LC_ALL, "");
    bindtextdomain(QUOTE_ME(PROJECT_NAME), QUOTE_ME(DATA_DIR) "/locale");
    textdomain(QUOTE_ME(PROJECT_NAME));
    determine_locale();

    context = g_option_context_new("- ibus chewing engine component");

    g_option_context_add_main_entries(context, entries, QUOTE_ME(PROJECT_NAME));

    if (!g_option_context_parse(context, &argc, &argv, &error)) {
        g_print("Option parsing failed: %s\n", error->message);
        exit(-1);
    }

    g_option_context_free(context);
    mkdg_log_set_level(ibus_chewing_verbose);

    if (showFlags) {
        printf("PROJECT_NAME=" QUOTE_ME(PROJECT_NAME) "\n");
        printf("DATA_DIR=" QUOTE_ME(DATA_DIR) "\n");
        printf("CHEWING_DATADIR_REAL=" QUOTE_ME(CHEWING_DATADIR_REAL)
               "\n");
    } else {
        start_component();
    }
    return 0;
}
