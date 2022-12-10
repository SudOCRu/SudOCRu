#include <stdlib.h>
#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <err.h>
#include "utils.h"
#include "windows/windows.h"
#include <neuralnetwork.h>

void SudOCRu_init(SudOCRu* app, GtkBuilder* ui)
{
    app->ui = ui;

    SetupThresholding(app);
    SetupGridDetection(app);
    SetupOCRResults(app);
    SetupSolveResults(app);
}

void SudOCRu_destroy(SudOCRu* app)
{
    free(app->tmp_buffer);

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

    if (app->nn != NULL)
        DestroyNeuralNetwork(app->nn);
}

void SetupStyle()
{
    GtkCssProvider *provider = gtk_css_provider_new();

    GError *error = 0;
    gtk_css_provider_load_from_file(provider,
            g_file_new_for_path("SudOCRu.css"), &error);
    if (error != 0)
    {
        printf("<!> Error loading css\n");
    }

    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
            GTK_STYLE_PROVIDER(provider),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

int main()
{
    printf("\xF0\x9F\xA6\x90 \033[31;1m\xC2\xBB\033[0m Starting "
            "\033[35;1mGUI\033[0m...");
    fflush(stdout);

    if (TTF_Init() == -1) {
        errx(EXIT_FAILURE, "TTF_Init: %s\n", TTF_GetError());
    }

    gtk_init(NULL, NULL);
    GtkBuilder* builder = gtk_builder_new();
    SetupStyle();

    GError* error = NULL;
    if (gtk_builder_add_from_file(builder, "SudOCRu.glade", &error) == 0)
    {
        g_printerr("Error loading layout file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    SudOCRu state = { 0, };
    SudOCRu_init(&state, builder);

    printf("\033[32;1mOK\033[0m\n");

    SetupMainWindow(&state);

    gtk_main();
    SudOCRu_destroy(&state);
    printf("\xF0\x9F\xA6\x90 \033[31;1m\xC2\xBB\033[0m Closing "
            "\033[35;1mapp\033[0m, bye!\n");
    return 0;
}
