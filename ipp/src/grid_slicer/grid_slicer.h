#pragma once
#include "../image.h"
#include "geom.h"

Image* CropPerspective(Image* img, BBox* bounding_box);
Image** ExtractSudokuCells(const Image* src, int* out_count);
