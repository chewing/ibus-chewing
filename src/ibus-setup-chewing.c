/* vim:set et sts=4: */
/*
 * Copyright © 2014  Red Hat, Inc. All rights reserved.
 * Copyright © 2014  Ding-Yi Chen <dchen at redhat.com>
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
#include "MakerDialogUtil.h"
#include "MakerDialogBackend.h"
#include "GConf2Backend.h"
#include "IBusChewingConfig.h"
#include "ibus-chewing-util.h"
#include "maker-dialog.h"


/* options */
static gboolean showFlags = FALSE;
gint verbose = 0;

static const GOptionEntry entries[] = {
    {"show_flags", 's', 0, G_OPTION_ARG_NONE, &showFlags,
     "Show compile flag only", NULL},
    {"verbose", 'v', 0, G_OPTION_ARG_INT, &verbose,
     "Verbose level. The higher the level, the more the debug messages.",
     "[integer]"},
    {NULL},
};


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
#define STRING_BUFFER_SIZE 100
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
    IBUS_CHEWING_LOG(INFO, "ibus-setup-chewing:determine_locale %s",
		     localeStr);
}

gint start_dialog()
{
    MkdgBackend *backend =
	gconf2_backend_new("/desktop/ibus/engine", NULL);
    IBusChewingConfig *iConfig =
	ibus_chewing_config_new(backend, NULL, NULL);

    MakerDialog *mDialog =
	maker_dialog_new_full(iConfig->properties, _("Setting"),
			      MKDG_WIDGET_FLAG_SET_IMMEDIATELY,
                              MKDG_BUTTON_FLAG_OK | MKDG_BUTTON_FLAG_CANCEL);
    GtkWidget *sDialog = GTK_WIDGET(mDialog);
    gtk_widget_show_all(sDialog);
    gint result = gtk_dialog_run(GTK_DIALOG(sDialog));
    gtk_widget_hide(sDialog);
    if (result != GTK_RESPONSE_OK) {
	return 3;
    }
    return 0;
}

gint main(gint argc, gchar * argv[])
{
    GError *error = NULL;
    GOptionContext *context;
    gtk_init(&argc, &argv);

    /* Init i18n messages */
    setlocale(LC_ALL, "");
    bindtextdomain(QUOTE_ME(PROJECT_NAME), QUOTE_ME(DATA_DIR) "/locale");
    textdomain(QUOTE_ME(PROJECT_NAME));

    context = g_option_context_new("- ibus chewing engine setup");

    g_option_context_add_main_entries(context, entries,
				      QUOTE_ME(PROJECT_NAME));

    if (!g_option_context_parse(context, &argc, &argv, &error)) {
	g_print("Option parsing failed: %s\n", error->message);
	exit(1);
    }

    g_option_context_free(context);
    if (showFlags) {
	printf("PROJECT_NAME=" QUOTE_ME(PROJECT_NAME) "\n");
	printf("DATA_DIR=" QUOTE_ME(DATA_DIR)
	       "\n");
	printf("CHEWING_DATADIR=" QUOTE_ME(CHEWING_DATADIR)
	       "\n");
	return 0;
    }
    return start_dialog();
}

/* vim:set et sts=4: */
