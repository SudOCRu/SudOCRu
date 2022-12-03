#pragma once
#include "../image.h"
#include "geom.h"

typedef struct SudokuGrid {
    float angle;
    BBox* bounds;
} SudokuGrid;

typedef struct SudokuCell {
    Image* data;
    size_t x, y, width, height;
    unsigned char value;
} SudokuCell;

SudokuGrid* ExtractSudoku(Image* org, Image* edges, int threshold, int flags);
SudokuCell** ExtractSudokuCells(const Image* original, SudokuGrid* grid,
        int flags, size_t* out_count);

void FreeSudokuCell(SudokuCell* cell);
void FreeSudokuGrid(SudokuGrid* grid);
