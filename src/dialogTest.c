#include <stdlib.h>
#include <locale.h>
#include <X11/extensions/XTest.h>
#include <X11/Xlib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>
#include <ibus.h>
#include <chewing.h>
#include "ibus-chewing-engine.h"
#define G_DEBUG_MSG(msg, args...) if (1) g_debug(msg, ##args)
#define DIALOG_TEST
#include "IBusChewingEngine-def.c"

//GValue *KBType_get_callback(PropertyContext *ctx){
//    return NULL;
//}

//void KBType_set_callback(PropertyContext *ctx, GValue *value){
//    g_debug("KBType_set_callback():%s",g_value_get_string(value));
//}

//void autoShiftCur_set_callback(PropertyContext *ctx, GValue *value){
//    g_debug("autoShiftCur_set_callback():%d",g_value_get_boolean(value));
//}

//void candPerPage_set_callback(PropertyContext *ctx, GValue *value){
//    g_debug("candPerPage_set_callback():%d",g_value_get_int(value));
//}


//PropertySpec propSpecs[]={
//    {G_TYPE_STRING, "KBType", "Keyboard",  N_("Keyboard Type"), 
//        "default", kbType_ids,	0, 0, 
//        KBType_get_callback, KBType_set_callback,
//        MAKER_DIALOG_PROPERTY_FLAG_INEDITABLE, 0, 0, 
//        NULL,
//    },
//    {G_TYPE_STRING, "selKeys", "Keyboard",  N_("Selection keys"), 
//        "1234567890", selKeys_array, 0, 0,
//        NULL, NULL,
//        0, 0, 0,
//        NULL,
//    },
//    {G_TYPE_INT, "hsuSelKeyType", "Keyboard", N_("Hsu's selection key"),
//        "1", NULL, 1, 2, 
//        NULL, NULL,
//        0, 0, 0,
//        NULL,
//    },

//    {G_TYPE_BOOLEAN, "autoShiftCur", "Editing", N_("Auto move cursor"),
//        "0", NULL, 0, 1,
//        NULL, autoShiftCur_set_callback,
//        0, 0, 0,
//        NULL,
//    },
//    {G_TYPE_BOOLEAN, "addPhraseDirection", "Editing", N_("Add phrase in front"),
//        "0", NULL, 0, 1, 
//        NULL, NULL,
//        0, 0, 0,
//        NULL,
//    },
//    {G_TYPE_BOOLEAN, "easySymbolInput", "Editing", N_("Easy symbol input"),
//        "0", NULL, 0, 1,
//        NULL, NULL,
//        0, 0, 0,
//        NULL,
//    },
//    {G_TYPE_BOOLEAN, "escCleanAllBuf", "Editing", N_("Esc clean all buffer"),
//        "0", NULL, 0, 1, 
//        NULL, NULL,
//        0, 0, 0,
//        NULL,
//    },
//    {G_TYPE_INT, "maxChiSymbolLen", "Editing", N_("Maximum Chinese characters"), 
//        "16", NULL, 4, 30,
//        NULL, NULL,
//        0, 0, 0,
//        N_("Maximum Chinese characters in pre-edit buffer:"),
//    },

//    {G_TYPE_INT, "candPerPage", "Selecting", N_("Candidate per page"), 
//        "10", NULL, 8, 10,
//        NULL, candPerPage_set_callback,
//        0, 0, 0,
//        NULL,
//    },
//    {G_TYPE_BOOLEAN, "phraseChoiceRearward", "Selecting", N_("Choice phrase from the end"),
//        "1", NULL, 0, 1, 
//        NULL, NULL,
//        0, 0, 0,
//        NULL,
//    },
//    {G_TYPE_BOOLEAN, "spaceAsSelection", "Selecting", N_("Space as selection key"),
//        "1", NULL, 0, 1, 
//        NULL, NULL,
//        0, 0, 0,
//        NULL,
//    },
//    {G_TYPE_INVALID, "", "", "",
//        "", NULL, 0, 0, 
//        NULL, NULL,
//        0, 0, 0,
//        NULL,
//    },


//};

int main(int argc, char *argv[]){
    gtk_init (&argc, &argv);

    /* Init i18n messages */
    setlocale (LC_ALL, "");
    bindtextdomain(PROJECT_NAME, DATADIR "/locale");
    textdomain(PROJECT_NAME);

    chewing_Init(CHEWING_DATADIR, "/tmp");

    GtkWidget *setting_dialog=
	GTK_WIDGET(maker_dialog_new_full(_("Setting"),3,page_labels,1,button_labels,button_responses));

    int i;
    for(i=0;propSpecs[i].valueType!=G_TYPE_INVALID;i++){
	maker_dialog_add_property(MAKER_DIALOG(setting_dialog), &(propSpecs[i]),NULL, NULL);
    }
    gint result=GTK_RESPONSE_NONE;
    while(result!=GTK_RESPONSE_DELETE_EVENT && result!=GTK_RESPONSE_CANCEL && result!=GTK_RESPONSE_OK) {
	g_debug("result=%d",result);
	result = gtk_dialog_run (GTK_DIALOG (setting_dialog));
	switch (result) 
	{
	    case GTK_RESPONSE_DELETE_EVENT:
		g_print ("delete event occurred 1\n");
		gtk_widget_destroy(setting_dialog);
		break;
	    case GTK_RESPONSE_CANCEL:
		g_print ("Cancel clicked\n");
		gtk_widget_destroy(setting_dialog);
		break;
	    case GTK_RESPONSE_OK:
		g_print ("Ok clicked\n");
		gtk_widget_destroy(setting_dialog);
		break;
	    case GTK_RESPONSE_APPLY:
		g_print ("Apply clicked\n");
		break;
	    default:
		break;
	}
    }
    g_debug("End while result=%d",result);

    return 0;
}

