#include "maker-dialog.h"
#include "IBusChewingEngine-def.c"

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
    MakerDialog *dialog=MAKER_DIALOG(maker_dialog_new());
    maker_dialog_set_verbose_level(dialog,verbose);
    int i;
    for (i=0; propSpecs[i].valueType!=G_TYPE_INVALID;i++){
	maker_dialog_add_property(dialog,&propSpecs[i],NULL,NULL);
    }
    gboolean ret=maker_dialog_write_gconf_schemas_file(dialog,schemasFilename,
	    "ibus-chewing", "/desktop/ibus/engine/Chewing",localeStr);
    gtk_widget_destroy(GTK_WIDGET(dialog));
    return ret;
}

int main (gint argc, gchar *argv[])
{
    GError *error = NULL;
    GOptionContext *context;
    gtk_init(&argc,&argv);

    /* Init i18n messages */
    setlocale (LC_ALL, "");
    bindtextdomain(PROJECT_NAME, DATADIR "/locale");
    textdomain(PROJECT_NAME);

    context = g_option_context_new ("schemasFile");

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
}

