#include "windows.h"
#include "../utils.h"

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
    } else {
        char code[4];
        snprintf(code, sizeof(code), "E%02i", task->result);
        ShowErrorMessage(app, (const char*)&code, "Unable to load image");
    }
    g_free(task->name);
    free(task);
    return G_SOURCE_REMOVE;
}

gpointer ThreadProcessImage(gpointer thr_data) {
    struct ProcessImageTask* task = thr_data;
    SudOCRu* app = task->app;
    char* file = task->name;

    ImageStatus status = ImageOk;
    Image* img = LoadImageFile(file, &status);
    if (img != NULL && status == ImageOk)
    {
        DestroyImage(app->original_image);
        DestroyImage(app->processed_image);
        DestroyImage(app->thresholded_image);
        FreeSudokuGrid(app->grid);
        DestroySudoku(app->sudoku);
        if (app->cells != NULL)
        {
            for (size_t i = 0; i < app->cells_len; i++)
            {
                FreeSudokuCell(app->cells[i]);
            }
            free(app->cells);
        }
        app->cells_len = 0;
        DestroyImage(app->cropped_grid);

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
    ShowLoadingDialog(task->app, "ProcessingPopup");
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

    g_object_unref(dialog);
}

void ShowAboutDialog(GtkButton *button, gpointer user_data)
{
    UNUSED(button);
    SudOCRu* app = user_data;
    GtkWindow* dialog = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "AboutUsPopup"));

    gtk_window_set_destroy_with_parent(dialog, TRUE);
    gtk_window_set_modal(dialog, TRUE);
    gtk_window_set_keep_above(dialog, TRUE);
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

    GtkWindowGroup* grp = gtk_window_get_group(win);
    gtk_window_group_add_window(grp, dialog);
    gtk_window_set_screen(win, gdk_screen_get_default());
    gtk_window_set_screen(dialog, gdk_screen_get_default());

    g_signal_connect(load, "clicked", G_CALLBACK(ShowImportFileDialog), app);
    g_signal_connect(openAbout, "clicked", G_CALLBACK(ShowAboutDialog), app);
    g_signal_connect(G_OBJECT(dialog), 
        "delete-event", G_CALLBACK(hide_window), NULL);
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}
