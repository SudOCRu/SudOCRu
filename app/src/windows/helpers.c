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
