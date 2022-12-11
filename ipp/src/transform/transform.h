#pragma once
#include "matrix.h"
#include "../grid_slicer/geom.h"

/*
 * RotateImage(Image* image, float angle, float midX, float midY)
 *
 * Rotate the given image by `angle` rads around the specified center and
 * fills the missing pixels by black pixels using the nearest neighbour
 * interpolation. Note: image must not be null.
 */
void RotateImage(Image* image, float angle, float midX, float midY);


/*
 * CropImage(const Image* src, size_t l, size_t t, size_t r, size_t b) -> Image*
 *
 * Returns a new image that contains the cropped portion of the original image
 * according to the specified bounds (left, top, right, bottom).
 */
Image* CropImage(const Image* src, size_t l, size_t t, size_t r, size_t b);


/*
 * CropImageExact(const Image* src, size_t l, size_t t, size_t r, size_t b)
 *     -> Image*
 *
 * Returns a new image that contains the cropped portion of the original image
 * according to the specified bounds (left, top, right, bottom). Note: If the
 * width/height of the cropped image is larger than the original, all
 * out-of-bounds pixels will be black in the cropped image.
 */
Image* CropImageExact(const Image* src, size_t l, size_t t, size_t r, size_t b);


/*
 * CropRotateImage(const Image* src, float angle, float midX, float midY,
 *           int left, int top, int right, int bottom) -> Image*
 *
 * Returns a new image that is the result of rotating the given image by `angle`
 * rads around the specified center, filling the missing pixels by black pixels
 * using the nearest neighbour interpolation and cropping the image according to
 * the specified bounds (left, top, right, bottom). Note: src must not be null
 * and the cropping bounds must be in the rotated image (already rotated).
 */
Image* CropRotateImage(const Image* src, float angle, float midX, float midY,
        int l, int t, int r, int b);


/*
 * DownscaleImage(const Image* src, size_t left, size_t top, size_t right,
 *      size_t bottom, size_t width, size_t height, unsigned char margin)
 *      -> Image*
 *
 * Returns a new image that is the result of the downscaling (using the box mean
 * method) of the cropped portion of the src image according to the specified
 * bounds. Additional margin can be added to the result image, the result width
 * is (width + 2 * margin) and the result height is (height + 2 * margin).
 */
Image* DownscaleImage(const Image* src, size_t left, size_t top, size_t right,
        size_t bottom, size_t width, size_t height, unsigned char margin);

/*
 * DownscaleImageN(const Image* src, double* out, size_t left, size_t top,
 * size_t right, size_t bottom, size_t width, size_t height)
 *
 * Write into the out array the result of the downscaling (using the box mean
 * method) of the cropped portion of the src image according to the specified
 * bounds. Note that the values in the output array are normalized (0-1)
 */
void DownscaleImageN(const Image* src, double* out, size_t left, size_t top,
        size_t right, size_t bottom, size_t width, size_t height);

/*
 * GetHomographyMatrix(const BBox* from, const BBox* to) -> Matrix*
 *
 * Returns the homography matrix coefficents that correspond to the
 * transformation that converts the points of the `from` bounding box to the
 * `to` bounding box. If such transformation does not exist, this function
 * returns NULL.
 *
 * The returned matrix (H) will be of dimension (8x1) and can be turned into a
 * transformation matrix (T) as:
 *
 * H = |---|     =>    T = |-----------|
 *     | a |               | a | b | c |
 *     | b |               | d | e | f |
 *     | c |               | g | h | 1 |
 *     | d |               |-----------|
 *     | e |
 *     | f |
 *     | g |
 *     | h |
 *     |---|
 */
Matrix* GetHomographyMatrix(const BBox* from, const BBox* to);


/*
 * WarpPerspective(const Image* image, BBox* from) -> Image*
 *
 * Returns a new image that contains the perspective-corrected image within
 * the `from` bounding box. If the extraction failed, this function returns
 * NULL. Note that the points in the bounding box must be
 * in the correct order in to prevent unwanted mirroring or rotation, the points
 * must be in the order: (top-left, bottom-left, top-right, bottom-right).
 */
Image* WarpPerspective(const Image* image, const BBox* from);

int UnwarpPerspective(const Image* src, const BBox* from, const Image* dst,
        const BBox* to);
