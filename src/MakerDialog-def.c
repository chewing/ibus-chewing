
static gchar *widget_get_id(gchar * buffer, gint buffer_size,
                            const gchar * widget_label,
                            const gchar * widget_type)
{
    g_snprintf(buffer, buffer_size, "%s%s%s%s",
               WIDGET_ID_PREFIX, widget_label,
               (widget_type) ? "_" : "", (widget_type) ? widget_type : "");
    return buffer;
}


typedef struct {
    MakerDialog *self;
    gint currentMaxWidth;
    const gchar *pageName;
    gfloat xalign;
    gfloat yalign;
} WidgetAlignment;

static void calculate_max_label_width_callback(gpointer key,
                                               gpointer value,
                                               gpointer widgetAlignment)
{
    gchar *wKey = (gchar *) key;
    gchar *pageName = (gchar *) value;
    WidgetAlignment *wAlignment = (WidgetAlignment *) widgetAlignment;

    if (!STRING_EQUALS(wAlignment->pageName, pageName)) {
        /* Different Page */
        return;
    }

    GtkWidget *widget =
        maker_dialog_get_widget(wAlignment->self, wKey, "label");
    GtkRequisition requisition;

    gtk_widget_size_request(widget, &requisition);
    wAlignment->currentMaxWidth =
        MAX(wAlignment->currentMaxWidth, requisition.width);
}

static void set_label_width_callback(gpointer key, gpointer value,
                                     gpointer userData)
{
    WidgetAlignment *wAlignment = (WidgetAlignment *) userData;
    gchar *wKey = (gchar *) key;
    gchar *pageName = (gchar *) value;

    if (!STRING_EQUALS(wAlignment->pageName, pageName)) {
        /* Different Page */
        return;
    }
    GtkWidget *widget =
        maker_dialog_get_widget(wAlignment->self, wKey, "label");
    gtk_widget_set_size_request(widget, wAlignment->currentMaxWidth, -1);
    gtk_misc_set_alignment(GTK_MISC(widget), wAlignment->xalign,
                           wAlignment->yalign);
    gtk_widget_show(widget);
}
