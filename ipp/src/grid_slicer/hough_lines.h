#pragma once
#include "image.h"
#include "geom.h"

#define MAX_LINES 4096
#define THETA_STEPS 480
#define BLACK_EDGE 0
#define WHITE_EDGE 1

typedef struct SudokuGrid {
    Rect bounds;
    double angle;
} SudokuGrid;

Line** HoughLines(const Image* img, size_t* found_count, int white_edge,
        size_t theta_steps, int threshold);

Line** AverageLines(Line** lines, size_t len, size_t* out_len);

Rect** FindRects(Image* image, Line** lines, size_t len, size_t* found_count);
Rect* FindSudokuBoard(Rect** rects, size_t rect_count);
