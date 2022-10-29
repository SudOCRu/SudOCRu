#pragma once
#include "../image.h"
#include "geom.h"

Image* ExtractSudoku(Image* original, Image* img, int threshold, int flags);
Image** ExtractSudokuCells(Image* original, Image* img, size_t* out_count,
        int threshold, int flags);
