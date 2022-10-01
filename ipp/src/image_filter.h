#pragma once
#include "image_filter.h"

void ProcessImageFile(Image* image);

/* Apply all filters to the image */
void FilterImage(Image* image);

/* Applys a grayscale filter to the image */
void GrayscaleFilter(Image* image);
/* Applys a binary tresholding filter to the image */
void TresholdImage(Image* image);
