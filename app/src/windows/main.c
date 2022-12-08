#include "windows.h"

void import_file (GtkButton *button, gpointer user_data)
{
    SudOCRu* app = user_data;

    GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "MainWindow"));
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Choose a file",
            GTK_WINDOW(win), 
            GTK_FILE_CHOOSER_ACTION_OPEN,
            "Cancel", GTK_RESPONSE_CANCEL,
            "Open", GTK_RESPONSE_ACCEPT, NULL);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Images (png, jpg)");
    gtk_file_filter_add_pattern(filter, "*.png");
    gtk_file_filter_add_mime_type(filter, "image/png");
    gtk_file_filter_add_pattern(filter, "*.jpg");
    gtk_file_filter_add_mime_type(filter, "image/jpg");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    gtk_widget_show_all(dialog);
    gint resp = gtk_dialog_run(GTK_DIALOG(dialog));
    if (resp == GTK_RESPONSE_OK)
        g_print("%s\n", gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
    else
        g_print("You pressed the cancel\n");
    gtk_widget_destroy(dialog);
}

void show_about_dialog(GtkButton *button, gpointer user_data)
{
    SudOCRu* app = user_data;
    GtkWindow* dialog = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "AboutUsPopup"));

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
    GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "MainWindow"));
    GtkWindow* dialog = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "AboutUsPopup"));
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
