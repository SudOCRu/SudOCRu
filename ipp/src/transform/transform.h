#pragma once
#include "matrix.h"
#include "../grid_slicer/geom.h"

/* Rotate the given image by `angle` rads and fills the missing pixels by
 * the `fill` color */
void RotateImage(Image* image, float angle, float midX, float midY);

/* Crop the given image by the specified bounds (left, top, right, bottom) */
Image* CropImage(const Image* src, size_t l, size_t t, size_t r, size_t b);

/* Rotate the given image by `angle` rads, fills the missing pixels in black
 * and crops the result image according to the specified bounds (left, top,
 * right, bottom).
 */
Image* CropRotateImage(const Image* src, float angle, float midX, float midY,
        int l, int t, int r, int b);

Matrix* GetHomographyMatrix(const BBox* from, const BBox* to);
Image* WarpPerspective(const Image* image, BBox* from);
