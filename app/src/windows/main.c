#include "windows.h"
#include "../utils.h"
#include <stdlib.h>

struct ProcessImageTask {
    SudOCRu* app;
    char* name;
    int result;
};

gboolean DoneProcessing(gpointer user_data)
{
    struct ProcessImageTask* task = user_data;
    SudOCRu* app = task->app;
    gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(app->ui,
            "ProcessingPopup")));
    if (!task->result)
    {
        ShowThresholding(app);
    }
    else
    {
        char code[4];
        snprintf(code, sizeof(code), "E%02i", task->result);
        ShowErrorMessage(app, "MainWindow", (const char*)&code,
                "Unable to load image");
    }
    g_free(task->name);
    free(task);
    return G_SOURCE_REMOVE;
}

void CleanState(SudOCRu* app)
{
    DestroyImage(app->original_image);
    app->original_image = NULL;
    DestroyImage(app->processed_image);
    app->processed_image = NULL;
    DestroyImage(app->thresholded_image);
    app->thresholded_image = NULL;
    FreeSudokuGrid(app->grid);
    app->grid = NULL;
    DestroySudoku(app->sudoku);
    app->sudoku = NULL;
    if (app->cells != NULL)
    {
        for (size_t i = 0; i < app->cells_len; i++)
        {
            FreeSudokuCell(app->cells[i]);
        }
        free(app->cells);
        app->cells = NULL;
    }
    app->cells_len = 0;
    DestroyImage(app->cropped_grid);
    app->cropped_grid = NULL;
}

gpointer ThreadProcessImage(gpointer thr_data) {
    struct ProcessImageTask* task = thr_data;
    SudOCRu* app = task->app;
    char* file = task->name;

    ImageStatus status = ImageOk;
    Image* img = LoadImageFile(file, &status);
    if (img != NULL && status == ImageOk)
    {
        CleanState(app);

        if (app->tmp_buffer == NULL)
        {
            app->tmp_buffer =
                calloc(img->width * img->height, sizeof(unsigned int));
        } else {
            app->tmp_buffer = realloc(app->tmp_buffer, img->width *
                    img->height * sizeof(unsigned int));
        }
        app->cropped_grid = NULL;
        app->original_image = img;
        app->processed_image =
            LoadBufImage(img->pixels, img->width, img->height, NULL);
        FilterImage(app->processed_image, app->tmp_buffer, 0);

        app->thresholded_image = LoadBufImage(app->processed_image->pixels,
            app->processed_image->width, app->processed_image->height, NULL);
        BinarizeImage(app->thresholded_image, app->tmp_buffer, THRESH_OPTIMAL);
    }
    task->result = status;
    gdk_threads_add_idle(DoneProcessing, task);
    return NULL;
}

void WaitFor(struct ProcessImageTask* task)
{
    ShowLoadingDialog(task->app, "MainWindow", "ProcessingPopup");
    PrintProcedure("Image Processing");
    g_thread_new("process_image", ThreadProcessImage, task);
}

void ShowImportFileDialog(GtkButton *button, gpointer user_data)
{
    UNUSED(button);
    SudOCRu* app = user_data;

    GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "MainWindow"));
    GtkFileChooserNative *dialog = gtk_file_chooser_native_new("Choose a file",
            GTK_WINDOW(win),
            GTK_FILE_CHOOSER_ACTION_OPEN,
            "Open", "Cancel");

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Images (png, jpg)");
    gtk_file_filter_add_pattern(filter, "*.png");
    gtk_file_filter_add_mime_type(filter, "image/png");
    gtk_file_filter_add_pattern(filter, "*.jpg");
    gtk_file_filter_add_mime_type(filter, "image/jpg");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    gint res = gtk_native_dialog_run(GTK_NATIVE_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char *file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        struct ProcessImageTask* task = malloc(sizeof(struct ProcessImageTask));
        task->app = app;
        task->name = file;
        WaitFor(task);
    }

    gtk_native_dialog_destroy(GTK_NATIVE_DIALOG(dialog));
}

gboolean OpenLinkGithub(GtkButton* button, gpointer user_data)
{
    UNUSED(button);
    UNUSED(user_data);
    system("xdg-open https://github.com/SudOCRu/SudOCRu/");
    return TRUE;
}

gboolean OpenLinkWebsite(GtkButton* button, gpointer user_data)
{
    UNUSED(button);
    UNUSED(user_data);
    system("xdg-open https://sudocru.com/");
    return TRUE;
}

void ShowAboutDialog(GtkButton *button, gpointer user_data)
{
    UNUSED(button);
    SudOCRu* app = user_data;
    GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "MainWindow"));
    GtkWindow* dialog = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "AboutUsPopup"));

    gtk_window_set_transient_for(dialog, win);
    gtk_window_set_destroy_with_parent(dialog, TRUE);
    gtk_window_set_modal(dialog, TRUE);
    gtk_widget_show(GTK_WIDGET(dialog));
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
    GtkButton* openGithub = GTK_BUTTON(gtk_builder_get_object(app->ui,
                "OpenGithub"));
    GtkButton* openWebsite = GTK_BUTTON(gtk_builder_get_object(app->ui,
                "OpenWebsite"));

    GtkWindowGroup* grp = gtk_window_get_group(win);
    gtk_window_group_add_window(grp, dialog);
    gtk_window_set_screen(win, gdk_screen_get_default());
    gtk_window_set_screen(dialog, gdk_screen_get_default());

    g_signal_connect(load, "clicked", G_CALLBACK(ShowImportFileDialog), app);
    g_signal_connect(openAbout, "clicked", G_CALLBACK(ShowAboutDialog), app);
    g_signal_connect(openWebsite, "clicked", G_CALLBACK(OpenLinkWebsite), NULL);
    g_signal_connect(openGithub, "clicked", G_CALLBACK(OpenLinkGithub), NULL);
    g_signal_connect(G_OBJECT(dialog),
        "delete-event", G_CALLBACK(hide_window), NULL);
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_window_present(win);
}
