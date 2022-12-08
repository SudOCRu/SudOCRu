#include "windows.h"

gboolean hide_window(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    gtk_widget_hide(widget);
    return TRUE;
}

void ShowLoadingDialog(SudOCRu* app)
{
    GtkWindow* dialog = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "ProcessingPopup"));

    gtk_window_set_destroy_with_parent(dialog, TRUE);
    gtk_window_set_modal(dialog, TRUE);
    gtk_window_set_keep_above(dialog, TRUE);
    gtk_widget_show(GTK_WIDGET(dialog));
}

void ShowErrorMessage(SudOCRu* app, const char* error_type,
        const char* error_desc)
{
    GtkWindow* dialog = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "ErrorPopup"));
    GtkLabel* err = GTK_LABEL(gtk_builder_get_object(app->ui,
                "ErrorType"));
    GtkLabel* desc = GTK_LABEL(gtk_builder_get_object(app->ui,
                "ErrorDescription"));

    gtk_label_set_text(err, error_type);
    gtk_label_set_text(desc, error_desc);

    gtk_window_set_destroy_with_parent(dialog, TRUE);
    gtk_window_set_modal(dialog, TRUE);
    gtk_window_set_keep_above(dialog, TRUE);
    gtk_widget_show(GTK_WIDGET(dialog));

    g_signal_connect(G_OBJECT(dialog),
        "delete-event", G_CALLBACK(hide_window), NULL);
    g_signal_connect(dialog, "destroy", G_CALLBACK(dialog), NULL);
}
