#include "windows.h"
#include "../utils.h"

gboolean hide_window(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    UNUSED(event);
    UNUSED(data);
    gtk_widget_hide(widget);
    return TRUE;
}

void DrawImage(Image* img, GtkImage* to)
{
    SDL_Surface* surf = ImageAsSurface(img);
    CopySurfaceToGdkImage(surf, to);
    SDL_FreeSurface(surf);
}

void ShowLoadingDialog(SudOCRu* app)
{
    GtkWindow* dialog = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "ProcessingPopup"));

    gtk_window_set_destroy_with_parent(dialog, TRUE);
    gtk_window_set_modal(dialog, TRUE);
    gtk_widget_show(GTK_WIDGET(dialog));
    gtk_window_set_keep_above(dialog, TRUE);
}

gboolean CloseErrorMessage(GtkButton* button, gpointer user_data)
{
    UNUSED(button);
    SudOCRu* app = user_data;
    gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(app->ui, "ErrorPopup")));
    return TRUE;
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
    GtkButton* close = GTK_BUTTON(gtk_builder_get_object(app->ui,
                "CloseErrorButton"));

    gtk_label_set_text(err, error_type);
    gtk_label_set_text(desc, error_desc);

    gtk_window_set_destroy_with_parent(dialog, TRUE);
    gtk_window_set_modal(dialog, TRUE);
    gtk_widget_show(GTK_WIDGET(dialog));
    gtk_window_set_keep_above(dialog, TRUE);

    g_signal_connect(G_OBJECT(close),
        "clicked", G_CALLBACK(CloseErrorMessage), app);
    g_signal_connect(G_OBJECT(dialog),
        "delete-event", G_CALLBACK(hide_window), NULL);
    g_signal_connect(dialog, "destroy", G_CALLBACK(dialog), NULL);
}
