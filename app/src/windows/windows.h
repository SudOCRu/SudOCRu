#pragma once

#include <image.h>
#include <filtering/image_filter.h>
#include <transform/transform.h>
#include <grid_slicer/grid_slicer.h>
#include <neuralnetwork.h>
#include <solver.h>
#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#define UNUSED(x) (void)(x)

typedef struct SudOCRu {
    u32* tmp_buffer;
    Image* original_image;
    Image* processed_image;
    Image* thresholded_image;

    SudokuGrid* grid;
    Sudoku* sudoku;
    size_t cells_len;
    SudokuCell** cells;
    Image* cropped_grid;

    NeuralNetwork* nn;

    GtkBuilder* ui;
} SudOCRu;

void SetupMainWindow(SudOCRu* app);

void SetupThresholding(SudOCRu* app);
void ShowThresholding(SudOCRu* app);

void SetupGridDetection(SudOCRu* app);
void ShowGridDetection(SudOCRu* app);

void SetupOCRResults(SudOCRu* app);
void ShowOCRResults(SudOCRu* app);

/* Helpers */
gboolean hide_window(GtkWidget *widget, GdkEvent *event, gpointer data);
void DrawImage(Image* img, GtkImage* to);
void ShowLoadingDialog(SudOCRu* app, char* name);
void ShowErrorMessage(SudOCRu* app, const char* error_type,
        const char* error_desc);

