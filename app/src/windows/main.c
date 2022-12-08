#include "windows.h"

void import_file (GtkButton *button, gpointer user_data)
{
    SudOCRu* app = user_data;
    // TODO
}

void show_about_dialog(GtkButton *button, gpointer user_data)
{
    SudOCRu* app = user_data;
    GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(app->ui, "MainWindow"));
    GtkWindow* dialog = GTK_WINDOW(gtk_builder_get_object(app->ui, "AboutUsPopup"));

    gtk_window_set_destroy_with_parent(dialog, TRUE);
    gtk_window_set_modal(dialog, TRUE);
    gtk_window_set_keep_above(dialog, TRUE);
    gtk_widget_show(GTK_WIDGET(dialog));
}

gboolean hide_window(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    gtk_widget_hide(widget);
    return TRUE;
}

void SetupMainWindow(SudOCRu* app)
{
    GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(app->ui, "MainWindow"));
    GtkWindow* dialog = GTK_WINDOW(gtk_builder_get_object(app->ui, "AboutUsPopup"));
    GtkButton* load = GTK_BUTTON(gtk_builder_get_object(app->ui,
                "LoadFileButton"));
    GtkButton* openAbout = GTK_BUTTON(gtk_builder_get_object(app->ui,
                "AboutUsButton"));
    GtkButton* close = GTK_BUTTON(gtk_builder_get_object(app->ui,
                "CloseButton"));

    GtkWindowGroup* grp = gtk_window_get_group(win);
    gtk_window_group_add_window(grp, dialog);
    gtk_window_set_screen(win, gdk_screen_get_default());
    gtk_window_set_screen(dialog, gdk_screen_get_default());

    g_signal_connect(load, "clicked", G_CALLBACK(import_file), app);
    g_signal_connect(close, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(openAbout, "clicked", G_CALLBACK(show_about_dialog), app);
    g_signal_connect(G_OBJECT(dialog), 
        "delete-event", G_CALLBACK(hide_window), NULL);
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}
