#include <stdlib.h>
#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include "utils.h"
#include "windows/windows.h"
#include <neuralnetwork.h>

void SudOCRu_init(SudOCRu* app, GtkBuilder* ui)
{
    app->ui = ui;

    SetupThresholding(app);
    SetupGridDetection(app);
    SetupOCRResults(app);
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

    SetupMainWindow(&state);

    gtk_main();
    SudOCRu_destroy(&state);
    return 0;
}
