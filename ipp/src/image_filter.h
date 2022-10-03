#pragma once
#include "image.h"

/* Apply all filters to the given image. The image must not be null. */
void FilterImage(Image* image);

/* Applys a grayscale filter to the image */
void GrayscaleFilter(Image* image);

/* Applys a binary tresholding filter to the image */
void TresholdImage(Image* image);

/* Add filters here */
