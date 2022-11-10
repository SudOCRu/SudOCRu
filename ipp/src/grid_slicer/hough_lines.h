#pragma once
#include "image.h"
#include "geom.h"

#define MAX_LINES 4096
#define THETA_STEPS 180
#define BLACK_EDGE 0
#define WHITE_EDGE 1

typedef struct SudokuGrid {
    Rect bounds;
    double angle;
} SudokuGrid;

Line** HoughLines(const Image* img, size_t* found_count, int white_edge,
        size_t theta_steps, int threshold, int save);

Line** AverageLines(Line** lines, size_t len, size_t* out_len);

PSet** GroupParallelLines(Line** lines, size_t len, size_t* out_len);
Rect** FindRects(PSet** pairs, size_t len, size_t* found_count);

Rect** GetBestRects(Rect** rects, size_t len, size_t keep);
Rect* FindSudokuBoard(Rect** rects, size_t rect_count);
