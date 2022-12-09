#include <stdlib.h>
#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include "utils.h"
#include "windows/windows.h"
#include <neuralnetwork.h>

void SudOCRu_init(SudOCRu* ins, GtkBuilder* ui)
{
    ins->current_window = LOAD_IMAGE;
    ins->original_image = NULL;
    ins->processed_image = NULL;
    ins->thresholded_image = NULL;
    ins->cropped_grid = NULL;
    ins->tmp_buffer = NULL;
    ins->grid = NULL;
    ins->sudoku = NULL;
    ins->ui = ui;
    ins->nn = NULL;
}

void SudOCRu_destroy(SudOCRu* app)
{
    DestroyImage(app->original_image);
    DestroyImage(app->processed_image);
    DestroyImage(app->thresholded_image);
    DestroyImage(app->cropped_grid);
    DestroySudoku(app->sudoku);
    FreeSudokuGrid(app->grid);
    free(app->tmp_buffer);
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

    SudOCRu state;
    SudOCRu_init(&state, builder);
    SetupMainWindow(&state);

    gtk_main();
    SudOCRu_destroy(&state);
    return 0;
}
