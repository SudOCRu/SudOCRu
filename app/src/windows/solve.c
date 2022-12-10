#include "windows.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <image.h>
#include <filtering/filters.h>
#include "../utils.h"

typedef void (*SudOCRu_Callback)(SudOCRu*, const char*);

void SaveReconstructedGrid(SudOCRu* app, const char* name)
{
    if (SaveImageFile(app->cropped_grid, name))
    {
        printf("Successfully saved `%s`\n", name);
    } else {
        printf("Failed to save sudoku\n");
    }
}

void ShowSaveFileDialog(SudOCRu* app, SudOCRu_Callback callback)
{
    GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "SaveWindow"));
    GtkFileChooserNative *dialog = gtk_file_chooser_native_new(
            "Save Reconstructed Sudoku",
            GTK_WINDOW(win), 
            GTK_FILE_CHOOSER_ACTION_SAVE,
            "Save", "Cancel");
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "sudoku.png");

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Images (png)");
    gtk_file_filter_add_pattern(filter, "*.png");
    gtk_file_filter_add_mime_type(filter, "image/png");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "All files");
    gtk_file_filter_add_pattern(filter, "*");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    gint res = gtk_native_dialog_run(GTK_NATIVE_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char *file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        callback(app, file);
    }

    g_object_unref(dialog);
}

Image* ReconstructGrid(SudOCRu* app)
{
    PrintProcedure("Grid Reconstruction");

    PrintStage(1, 2, "Preparing", 0);
    TTF_Font* font = TTF_OpenFont("fonts/Lemon-Regular.ttf", 32);
    if (font == NULL)
    {
        printf(".. Eror loading font!\n");
        return NULL;
    }
    SDL_Color col = {254, 142, 6, 0};

    Invert(app->cropped_grid);
    SDL_Surface* surf = ImageAsSurface(app->cropped_grid);
    PrintStage(1, 2, "Preparing", 1);

    PrintStage(2, 2, "Rendering", 0);
    char digit[3] = { 0, };
    char message[16] = { 0, };
    SDL_Surface* digit_surf;
    SDL_Rect dst;
    for (size_t i = 0; i < app->cells_len; i++)
    {
        SudokuCell* cell = app->cells[i];
        if (cell->value == 0)
        {
            snprintf(digit, sizeof(digit), "%hhu", app->sudoku->board[i]);
            snprintf(message, sizeof(message), "Rendering %02lu/81", i + 1);
            PrintStage(2, 2, message, 2);

            digit_surf = TTF_RenderText_Solid(font, digit, col); 
            dst.x = cell->x + cell->width / 2 - digit_surf->w / 2;
            dst.y = cell->y + cell->height / 2 - digit_surf->h / 2;
            dst.w = cell->width;
            dst.h = cell->height;
            SDL_BlitSurface(digit_surf, NULL, surf, &dst);
            SDL_FreeSurface(digit_surf);
        }
    }
    PrintStage(2, 2, "Rendering 81/81", 1);

    return SurfaceAsImage(surf);
}

gboolean SaveGrid(GtkButton* button, gpointer user_data)
{
    UNUSED(button);
    SudOCRu* app = user_data;

    ShowSaveFileDialog(app, SaveReconstructedGrid);
    return TRUE;
}

void SetupSolveResults(SudOCRu* app)
{
    GtkWindow* main = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "MainWindow"));
    GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "SaveWindow"));
    GtkButton* save = GTK_BUTTON(gtk_builder_get_object(app->ui,
                "SaveAsGridButton"));

    GtkWindowGroup* grp = gtk_window_get_group(main);
    gtk_window_group_add_window(grp, win);
    gtk_window_set_screen(win, gdk_screen_get_default());

    g_signal_connect(save, "clicked", G_CALLBACK(SaveGrid), app);
    g_signal_connect(win, "destroy", G_CALLBACK(hide_window), NULL);
    g_signal_connect(G_OBJECT(win),
        "delete-event", G_CALLBACK(hide_window), NULL);
    g_signal_connect(win, "destroy", G_CALLBACK(hide_window), NULL);

    gtk_window_set_destroy_with_parent(win, TRUE);
    gtk_window_set_modal(win, TRUE);
}

void ShowSolveResults(SudOCRu* app)
{
    GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "SaveWindow"));
    GtkImage* img = GTK_IMAGE(gtk_builder_get_object(app->ui,
                "SaveImage"));
    Image* reconstructed = ReconstructGrid(app);
    if (reconstructed != NULL)
    {
        DrawImage(reconstructed, img);
        DestroyImage(app->cropped_grid);
        app->cropped_grid = reconstructed;
    }
    DrawImage(app->cropped_grid, img);

    gtk_widget_show(GTK_WIDGET(win));
    gtk_window_set_keep_above(win, TRUE);
}