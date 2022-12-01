#pragma once
#include "matrix.h"
#include "../grid_slicer/geom.h"

Matrix* GetHomographyMatrix(const BBox* from, const BBox* to);
Image* WarpPerspective(const Image* image, BBox* from);
