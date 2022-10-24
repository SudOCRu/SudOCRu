#pragma once
#include "../image.h"
#include "geom.h"

#define SC_NO_FLG 0
#define SC_FLG_ALINES 1 // All lines
#define SC_FLG_FLINES 2 // Filtered lines
#define SC_FLG_ARECTS 4 // All rects
#define SC_FLG_FRECTS 8 // Filtered rects
#define SC_FLG_PRESTL 16 // Print result table

#define SC_FLG_FIL ((SC_FLG_FLINES) | (SC_FLG_FRECTS) | (SC_FLG_PRESTL))

Image* CropPerspective(Image* img, BBox* bounding_box);
Image** ExtractSudokuCells(Image* img, size_t* out_count, int threshold,
        int flags);
