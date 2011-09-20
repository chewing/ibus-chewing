#include "maker-dialog.h"
#include "IBusChewingEngine-def.c"
#include "ibus-chewing-util.h"

static gint verbose=0;
static gchar *schemasFilename=NULL;
static gchar *localeStr=NULL;
static const gchar localeDefault[]="C";

static const GOptionEntry entries[] =
{
    { "verbose", 'v', 0, G_OPTION_ARG_INT, &verbose,
	"Verbose level. The higher the level, the more the debug messages.",
       	"[integer]" },
    { "locale", 'l', 0, G_OPTION_ARG_STRING, &localeStr,
	"Supported locale. Use ';' to separate locales.",
	"[str]" },
    { NULL },
};

gboolean schemas_get_definition(){
    MakerDialog *dialog=maker_dialog_new();
    maker_dialog_set_verbose_level(dialog,verbose);
    int i;
    for (i=0; propSpecs[i].valueType!=G_TYPE_INVALID;i++){
	maker_dialog_add_property_no_gui(dialog,&propSpecs[i],NULL);
    }
    gboolean ret=maker_dialog_write_gconf_schemas_file(dialog,schemasFilename,
	    "ibus-chewing", "/desktop/ibus/engine/Chewing",localeStr);

    return ret;
}

int main (gint argc, gchar *argv[])
{
    GError *error = NULL;
    GOptionContext *context;

    g_type_init();
//    gtk_init(&argc,&argv);

    /* Init i18n messages */
    setlocale (LC_ALL, "");
    bindtextdomain(quote_me(PROJECT_NAME), quote_me(DATA_DIR) "/locale");
    textdomain(quote_me(PROJECT_NAME));

    context = g_option_context_new("schemasFile");

    g_option_context_add_main_entries (context, entries, "ibus-chewing");

    if (!g_option_context_parse (context, &argc, &argv, &error)) {
	g_print ("Option parsing failed: %s\n", error->message);
	exit (-1);
    }
    g_option_context_free (context);
    if (!localeStr)
	localeStr=(gchar *)localeDefault;
    if (argc<2){
	fprintf(stderr,"Specify filename of outputing schemas file!\n");
	exit (-1);
    }
    schemasFilename=argv[1];
    if (schemas_get_definition()){
	exit(0);
    }
    exit(1);
    return 0;
}

