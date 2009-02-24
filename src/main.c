/* vim:set et sts=4: */

#include <ibus.h>
#include <stdlib.h>
#include <locale.h>
#include <chewing.h>
#include "ibus-chewing-engine.h"

GConfClient *gConfClient=NULL;
static IBusBus *bus = NULL;
static IBusFactory *factory = NULL;

/* options */
static gboolean ibus = FALSE;
gboolean ibus_chewing_verbose = FALSE;

static const GOptionEntry entries[] = 
{
    { "ibus", 'i', 0, G_OPTION_ARG_NONE, &ibus, "component is executed by ibus", NULL },
    { "verbose", 'v', 0, G_OPTION_ARG_NONE, &ibus_chewing_verbose, "verbose", NULL },
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

    ibus_init ();
    
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
main (gint argc, gchar **argv)
{
    GError *error = NULL;
    GOptionContext *context;

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
