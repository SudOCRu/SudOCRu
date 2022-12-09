#include "windows.h"

struct CellDetails {
    SudOCRu* app;
    unsigned char id;
};

gboolean EditCell(GtkButton* button, gpointer user_data)
{
    struct CellDetails* details = user_data;
    SudOCRu* app = details->app;

    GtkWindow* dialog = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "CellModifPopup"));
    GtkButton* save = GTK_BUTTON(gtk_builder_get_object(app->ui,
                "SaveCellButton"));
    GtkImage* display = GTK_IMAGE(gtk_builder_get_object(app->ui,
                "CellImage"));
    GtkEntry* num_input = GTK_ENTRY(gtk_builder_get_object(app->ui,
                "CellTextBox"));

    DrawImage(app->cells[details->id]->data, display);

    char name[14];
    snprintf(name, sizeof(name), "Edit Cell %02hhu", details->id);
    gtk_window_set_title(dialog, name);
    gtk_window_set_destroy_with_parent(dialog, TRUE);
    gtk_window_set_modal(dialog, TRUE);
    gtk_window_set_keep_above(dialog, TRUE);
    g_signal_connect(G_OBJECT(dialog),
        "delete-event", G_CALLBACK(hide_window), NULL);
    g_signal_connect(dialog, "destroy", G_CALLBACK(hide_window), NULL);
    gtk_widget_show(GTK_WIDGET(dialog));
    return TRUE;
}

void ShowOCRResults(SudOCRu* app)
{
    GtkWindow* main = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "MainWindow"));
    GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "OCRCorrection"));

    GtkWindowGroup* grp = gtk_window_get_group(main);
    gtk_window_group_add_window(grp, win);
    gtk_window_set_screen(win, gdk_screen_get_default());

    GtkImage* img = GTK_IMAGE(gtk_builder_get_object(app->ui,
                "OCRImage"));
    DrawImage(app->original_image, img);

    GtkContainer* container = GTK_CONTAINER(gtk_builder_get_object(app->ui,
                "CellGrid"));
    GList *children, *iter;

    children = gtk_container_get_children(container);
    unsigned char i = 80;
    for(iter = children; iter != NULL; iter = g_list_next(iter))
    {
        struct CellDetails* details = malloc(sizeof(struct CellDetails));
        details->app = app;
        details->id = i--;
        g_signal_connect(GTK_WIDGET(iter->data),
                "clicked", G_CALLBACK(EditCell), details);
    }
    g_list_free(children);

    g_signal_connect(win, "destroy", G_CALLBACK(hide_window), NULL);

    gtk_window_set_destroy_with_parent(win, TRUE);
    gtk_window_set_modal(win, TRUE);
    gtk_window_set_keep_above(win, TRUE);
    gtk_widget_show(GTK_WIDGET(win));
}
