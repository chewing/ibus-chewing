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
#include "ibus-chewing-util.h"
#include "maker-dialog.h"

MakerDialog *makerDialog=NULL;
static IBusBus *bus = NULL;
static IBusFactory *factory = NULL;

/* options */
static gboolean showFlags=FALSE;
static gboolean ibus = FALSE;
static gboolean xml = FALSE;
int ibus_chewing_verbose= 0;


static const GOptionEntry entries[] =
{
    { "show_flags", 's', 0, G_OPTION_ARG_NONE, &showFlags, "Show compile flag only", NULL },
    { "ibus", 'i', 0, G_OPTION_ARG_NONE, &ibus, "component is executed by ibus", NULL },
    { "verbose", 'v', 0, G_OPTION_ARG_INT, &ibus_chewing_verbose,
        "Verbose level. The higher the level, the more the debug messages.",
        "[integer]" },
    { "xml", 'x', 0, G_OPTION_ARG_NONE, &xml, "read chewing engine desc from xml file", NULL },
    { NULL },
};


static void
ibus_disconnected_cb (IBusBus  *bus,
                      gpointer  user_data)
{
    g_debug ("bus disconnected");
    ibus_quit ();
}


static void
start_component (void)
{
    ibus_init();
    bus = ibus_bus_new ();
    g_signal_connect (bus, "disconnected", G_CALLBACK (ibus_disconnected_cb), NULL);

    factory = ibus_factory_new (ibus_bus_get_connection (bus));

    ibus_factory_add_engine (factory, "chewing", IBUS_TYPE_CHEWING_ENGINE);

    if (ibus) {
        ibus_bus_request_name (bus, "org.freedesktop.IBus.Chewing", 0);
    }else {
        IBusComponent *component=NULL;
        if (xml){
            component = ibus_component_new_from_file (
                    quote_me(DATA_DIR) "/ibus/component/chewing.xml");
        }else{
            component=ibus_component_new("org.freedesktop.IBus.Chewing",
                    _("Chewing component"), quote_me(PRJ_VER), "GPLv2+",
                _("Peng Huang, Ding-Yi Chen"),
                "http://code.google.com/p/ibus",
                quote_me(LIBEXEC_DIR) "/ibus-engine-chewing --ibus",
                quote_me(PROJECT_NAME));
        }
        ibus_component_add_engine(component,
                ibus_engine_desc_new("chewing", _("Chewing"),
                "Chinese chewing input method",
                "zh_TW", "GPLv2+", _("Peng Huang, Ding-Yi Chen"),
                quote_me(PRJ_DATA_DIR) "/icons/" quote_me(PROJECT_NAME) ".png",
                "us")
        );

        ibus_bus_register_component (bus, component);
    }
    ibus_main ();
}

const char *locale_env_strings[]={
    "LC_ALL",
    "LANG",
    "LANGUAGE",
    "GDM_LANG",
    NULL
};
void determine_locale(){
#ifndef STRING_BUFFER_SIZE
#define STRING_BUFFER_SIZE 100
#endif
    gchar *localePtr=NULL;
    gchar localeStr[STRING_BUFFER_SIZE];
    int i;
    for(i=0;locale_env_strings[i]!=NULL;i++){
        if (getenv(locale_env_strings[i])){
            localePtr=getenv(locale_env_strings[i]);
            break;
        }
    }
    if (!localePtr){
        localePtr="en_US.utf8";
    }
    /* Use UTF8 as charset unconditionally */
    for (i=0;localePtr[i]!='\0';i++){
        if (localePtr[i]=='.')
            break;
        localeStr[i]=localePtr[i];
    }
    localeStr[i]='\0';
    g_strlcat(localeStr,".utf8",STRING_BUFFER_SIZE);
#undef STRING_BUFFER_SIZE
    setlocale (LC_ALL, localeStr);
    IBUS_CHEWING_LOG(1,"[I1] determine_locale %s",localeStr);
}


int
main (gint argc, gchar *argv[])
{
    GError *error = NULL;
    GOptionContext *context;
    gtk_init(&argc,&argv);

    /* Init i18n messages */
    setlocale (LC_ALL, "zh_TW.utf8");
    bindtextdomain(quote_me(PROJECT_NAME), quote_me(DATA_DIR) "/locale");
    textdomain(quote_me(PROJECT_NAME));

    context = g_option_context_new ("- ibus chewing engine component");

    g_option_context_add_main_entries (context, entries, quote_me(PROJECT_NAME));

    if (!g_option_context_parse (context, &argc, &argv, &error)) {
        g_print ("Option parsing failed: %s\n", error->message);
        exit (-1);
    }

    g_option_context_free (context);

    if (showFlags){
        printf("PROJECT_NAME=" quote_me(PROJECT_NAME) "\n");
        printf("DATA_DIR=" quote_me(DATA_DIR) "\n");
        printf("CHEWING_DATADIR=" quote_me(CHEWING_DATADIR) "\n");
    }else{
        start_component ();
    }
    return 0;
}
