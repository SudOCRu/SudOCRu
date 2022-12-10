#include "windows.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <image.h>
#include <filtering/filters.h>
#include "../utils.h"

typedef void (*SudOCRu_Callback)(SudOCRu*, const char*);

void ShowSaveFileDialog(SudOCRu* app, SudOCRu_Callback callback,
        const char* current_name, int accept_img)
{
    GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "SaveWindow"));
    GtkFileChooserNative *dialog = gtk_file_chooser_native_new(
            "Save Reconstructed Sudoku",
            GTK_WINDOW(win), 
            GTK_FILE_CHOOSER_ACTION_SAVE,
            "Save", "Cancel");
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), current_name);

    GtkFileFilter *filter;
    if (accept_img)
    {
        filter = gtk_file_filter_new();
        gtk_file_filter_set_name(filter, "Images (png)");
        gtk_file_filter_add_pattern(filter, "*.png");
        gtk_file_filter_add_mime_type(filter, "image/png");
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
    }

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
    int size = app->cells[0]->height / 1.6f;
    TTF_Font* font = TTF_OpenFont("fonts/Lemon-Regular.ttf", size);
    TTF_Font* font_outline = TTF_OpenFont("fonts/Lemon-Regular.ttf", size);
    if (font == NULL || font_outline == NULL)
    {
        printf(".. Eror loading font!\n");
        return NULL;
    }

    SDL_Color col = { 254, 142, 6, 255 };
    TTF_SetFontOutline(font_outline, 2); 
    SDL_Color col_outline = { 151, 84, 2, 255 };

    Image* cropped = app->cropped_grid;
    Invert(cropped);

    SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat(0,
            cropped->width, cropped->height,
            32, SDL_PIXELFORMAT_RGB888);
    PrintStage(1, 2, "Preparing", 1);

    PrintStage(2, 2, "Rendering", 0);
    char digit[3] = { 0, };
    char message[16] = { 0, };
    SDL_Surface* digit_surf, *outline;
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

            outline = TTF_RenderText_Solid(font_outline, digit, col_outline); 
            dst.x = cell->x + cell->width / 2 - outline->w / 2;
            dst.y = cell->y + cell->height / 2 - outline->h / 2;
            SDL_BlitSurface(outline, NULL, surf, &dst);

            SDL_FreeSurface(digit_surf);
        }
    }
    PrintStage(2, 2, "Rendering 81/81", 1);

    Image* img = SurfaceAsImage(surf);
    BBox from = { 0, 0, 0, cropped->height, cropped->width,
        cropped->height, cropped->width, 0 };

    for (size_t i = 0; i < cropped->width * cropped->height; i++)
    {
        if (img->pixels[i] != 0)
            cropped->pixels[i] = img->pixels[i];
    }

    if (UnwarpPerspective(img, &from, app->original_image,
            app->grid->bounds))
    {
        DestroyImage(img);
        return app->original_image;
    }
    DestroyImage(img);
    return cropped;
}

void SaveStandaloneGrid(SudOCRu* app, const char* name)
{
    if (SaveImageFile(app->cropped_grid, name))
    {
        printf("Successfully saved `%s`\n", name);
    } else {
        printf("Failed to save sudoku\n");
    }
}

gboolean SaveResultGrid(GtkButton* button, gpointer user_data)
{
    UNUSED(button);
    SudOCRu* app = user_data;

    ShowSaveFileDialog(app, SaveStandaloneGrid, "grid.png", 1);
    return TRUE;
}

void SaveSudokuAsText(SudOCRu* app, const char* name)
{
    if (SaveSudoku(app->sudoku, name))
        printf("Successfully exported sudoku as `%s`\n", name);
}

gboolean SaveResultAsText(GtkButton* button, gpointer user_data)
{
    UNUSED(button);
    ShowSaveFileDialog(user_data, SaveSudokuAsText, "sudoku", 0);
    return TRUE;
}

void SaveOriginalImage(SudOCRu* app, const char* name)
{
    if (SaveImageFile(app->original_image, name))
        printf("Successfully saved reconstructed as `%s`\n", name);
}

gboolean SaveResult(GtkButton* button, gpointer user_data)
{
    UNUSED(button);
    ShowSaveFileDialog(user_data, SaveOriginalImage, "result.png", 0);
    return TRUE;
}

void SetupSolveResults(SudOCRu* app)
{
    GtkWindow* main = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "MainWindow"));
    GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "SaveWindow"));
    GtkButton* save = GTK_BUTTON(gtk_builder_get_object(app->ui,
                "SaveAsGrid"));
    GtkButton* saveText = GTK_BUTTON(gtk_builder_get_object(app->ui,
                "SaveAsTextButton"));
    GtkButton* saveReconstructed = GTK_BUTTON(gtk_builder_get_object(app->ui,
                "SaveReconstructed"));

    GtkWindowGroup* grp = gtk_window_get_group(main);
    gtk_window_group_add_window(grp, win);
    gtk_window_set_screen(win, gdk_screen_get_default());

    g_signal_connect(save, "clicked", G_CALLBACK(SaveResultGrid), app);
    g_signal_connect(saveText, "clicked", G_CALLBACK(SaveResultAsText), app);
    g_signal_connect(saveReconstructed, "clicked",
            G_CALLBACK(SaveResult), app);
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
    }

    gtk_widget_show(GTK_WIDGET(win));
    gtk_window_set_keep_above(win, TRUE);
}
