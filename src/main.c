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
#include "maker-dialog.h"

MakerDialog *makerDialog=NULL;
static IBusBus *bus = NULL;
static IBusFactory *factory = NULL;

/* options */
static gboolean ibus = FALSE;
int ibus_chewing_verbose= 0;

static const GOptionEntry entries[] =
{
    { "ibus", 'i', 0, G_OPTION_ARG_NONE, &ibus, "component is executed by ibus", NULL },
    { "verbose", 'v', 0, G_OPTION_ARG_INT, &ibus_chewing_verbose,
        "Verbose level. The higher the level, the more the debug messages.",
        "[integer]" },
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
    IBusComponent *component;

//    ibus_init ();

    bus = ibus_bus_new ();
    g_signal_connect (bus, "disconnected", G_CALLBACK (ibus_disconnected_cb), NULL);

    component = ibus_component_new_from_file ( DATADIR "/ibus/component/chewing.xml");

    IBusConnection *iConnection=ibus_bus_get_connection (bus);
    factory = ibus_factory_new (iConnection);
    ibus_factory_add_engine (factory, "chewing", IBUS_TYPE_CHEWING_ENGINE);

    if (ibus) {
        ibus_bus_request_name (bus, "org.freedesktop.IBus.Chewing", 0);
    }else {
        ibus_bus_register_component (bus, component);
    }
    ibus_main ();
}

int
main (gint argc, gchar *argv[])
{
    GError *error = NULL;
    GOptionContext *context;
    gtk_init(&argc,&argv);

    /* Init i18n messages */
    setlocale (LC_ALL, "");
    bindtextdomain(PROJECT_NAME, DATADIR "/locale");
    textdomain(PROJECT_NAME);

    context = g_option_context_new ("- ibus chewing engine component");

    g_option_context_add_main_entries (context, entries, "ibus-chewing");

    if (!g_option_context_parse (context, &argc, &argv, &error)) {
        g_print ("Option parsing failed: %s\n", error->message);
        exit (-1);
    }

    g_option_context_free (context);
    start_component ();
    return 0;
}
