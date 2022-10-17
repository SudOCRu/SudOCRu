#pragma once
#include "../image.h"
#include "hough_lines.h"

Image** ExtractSudokuCells(const Image* src, int* out_count);
