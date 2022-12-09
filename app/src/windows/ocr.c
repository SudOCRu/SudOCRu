#include "windows.h"

struct CellDetails {
    SudOCRu* app;
    unsigned char id;
    GtkButton* button;
};

gboolean SaveCell(GtkButton* button, gpointer user_data)
{
    UNUSED(button);
    struct CellDetails* details = user_data;
    SudOCRu* app = details->app;

    GtkEntry* num_input = GTK_ENTRY(gtk_builder_get_object(app->ui,
                "CellTextBox"));
    const gchar *text = gtk_entry_get_text(num_input);
    if (text[0] >= '0' && text[0] <= '9')
    {
        SudokuCell* cell = app->cells[details->id];
        cell->value = text[0] - '0';
        char digit[3] = { 0, };
        if (cell->value != 0)
            snprintf(digit, sizeof(digit), "%hhu", cell->value);
        gtk_button_set_label(details->button, digit);
    }

    GtkButton* save = GTK_BUTTON(gtk_builder_get_object(app->ui,
                "SaveCellButton"));
    g_signal_handlers_disconnect_by_func(save, G_CALLBACK(SaveCell), details);
    gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(app->ui,
        "CellModifPopup")));
    return TRUE;
}

gboolean EditCell(GtkButton* button, gpointer user_data)
{
    UNUSED(button);
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

    SudokuCell* cell = app->cells[details->id];
    DrawImage(cell->data, display);
    char digit[3] = { 0, };
    if (cell->value != 0)
        snprintf(digit, sizeof(digit), "%hhu", cell->value);
    gtk_entry_set_text(num_input, digit);

    char name[14];
    snprintf(name, sizeof(name), "Edit Cell %02hhu", details->id);

    gtk_window_set_title(dialog, name);
    gtk_window_set_destroy_with_parent(dialog, TRUE);
    gtk_window_set_modal(dialog, TRUE);
    g_signal_connect(G_OBJECT(save), "clicked", G_CALLBACK(SaveCell), details);
    g_signal_connect(G_OBJECT(dialog),
        "delete-event", G_CALLBACK(hide_window), NULL);
    g_signal_connect(dialog, "destroy", G_CALLBACK(hide_window), NULL);
    gtk_widget_show(GTK_WIDGET(dialog));
    gtk_window_set_keep_above(dialog, TRUE);
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
    DrawImage(app->cropped_grid, img);

    GtkContainer* container = GTK_CONTAINER(gtk_builder_get_object(app->ui,
                "CellGrid"));
    GList *children, *iter;

    children = gtk_container_get_children(container);
    unsigned char i = 80;
    char digit[3] = { 0, };
    for(iter = children; iter != NULL; iter = g_list_next(iter))
    {
        struct CellDetails* details = malloc(sizeof(struct CellDetails));
        details->app = app;
        details->id = i--;
        details->button = GTK_BUTTON(iter->data);

        SudokuCell* cell = app->cells[details->id];
        if (cell->value != 0)
            snprintf(digit, sizeof(digit), "%hhu", cell->value);
        gtk_button_set_label(details->button, digit);
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
