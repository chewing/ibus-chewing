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
#if GSETTINGS_SUPPORT == 1
#include "GSettingsBackend.h"
#endif
#if GCONF2_SUPPORT == 1
#include "GConf2Backend.h"
#endif
#include "GConf2Backend.h"
#include "IBusChewingProperties.h"
#include "IBusChewingUtil.h"
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

gint start_dialog()
{
#if GSETTINGS_SUPPORT == 1
    MkdgBackend *backend =
        mkdg_g_settings_backend_new(QUOTE_ME(PROJECT_SCHEMA_ID), QUOTE_ME(PROJECT_SCHEMA_DIR), NULL);
#elif GCONF2_SUPPORT == 1
    MkdgBackend *backend =
        gconf2_backend_new("/desktop/ibus/engine", NULL);
#else
    MkdgBackend *backend = NULL;
    g_error("Flag GSETTINGS_SUPPORT or GCONF2_SUPPORT are required!");
    return 1;
#endif /* GSETTINGS_SUPPORT */
    IBusChewingProperties *iProperties =
	ibus_chewing_properties_new(backend, NULL, NULL);

    MakerDialog *mDialog =
	maker_dialog_new_full(iProperties->properties, _("Setting"),
			      MKDG_WIDGET_FLAG_SET_IMMEDIATELY,
                              MKDG_BUTTON_FLAG_OK | MKDG_BUTTON_FLAG_CANCEL);
    GtkWidget *sDialog = GTK_WIDGET(mDialog);
    gtk_widget_show_all(sDialog);
    gint result = gtk_dialog_run(GTK_DIALOG(sDialog));
    gtk_widget_hide(sDialog);
    if (result != GTK_RESPONSE_OK) {
	return 3;
    }
    maker_dialog_save_all_widgets_values(mDialog,NULL);
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
	printf("CHEWING_DATADIR_REAL=" QUOTE_ME(CHEWING_DATADIR_REAL)
	       "\n");
	return 0;
    }
    mkdg_log_set_level(verbose);
    return start_dialog();
}

/* vim:set et sts=4: */
