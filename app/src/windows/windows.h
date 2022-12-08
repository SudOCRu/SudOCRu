#pragma once

#include <image.h>
#include <filtering/image_filter.h>
#include <transform/transform.h>
#include <grid_slicer/grid_slicer.h>
#include <solver.h>
#include <gtk/gtk.h>
#include <SDL2/SDL.h>

typedef enum SudOCRuWindowKind {
    LOAD_IMAGE,
    CHANGE_THRESHOLD,
    FIX_OCR,
    SAVE_RESULT
} SudOCRuWindowKind;

typedef struct SudOCRu {
    SudOCRuWindowKind current_window;

    u32* tmp_buffer;
    Image* processed_image;
    Image* thresholded_image;

    SudokuGrid* grid;
    Sudoku* sudoku;

    GtkBuilder* ui;
} SudOCRu;

void SetupMainWindow(SudOCRu* app);
