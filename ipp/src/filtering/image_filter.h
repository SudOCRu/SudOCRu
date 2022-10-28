#pragma once
#include "image.h"

typedef unsigned int u32;
typedef unsigned char u8;

#define GAMMA_FACTOR 0.45

/* Apply all filters to the given image. The image must not be null. */
void FilterImage(Image* image);

/* Applys a grayscale filter to the image and fills the histogram array */
void GrayscaleFilter(Image* image, u8* min, u8* max);
void StretchContrast(Image* img, u8 min, u8 max);
void MedianFilter(Image* image, size_t block, u32 histogram[256]);
void GaussianBlur(Image* img, const float* kernel, size_t r);

void EnhanceContrast(Image* image, u32 histogram[256]);
void GammaFilter(Image* image, float f);

void FillHistogram(const Image* image, u32 histogram[256]);
u8 ComputeOtsuThreshold(size_t len, const u32 histogram[256]);

/* Applys a binary tresholding filter to the image */
void ThresholdImage(Image* image, u8 threshold);
void AdapativeThresholding(Image* img, size_t r, float threshold);

Image* CannyEdgeDetection(const Image* src);

/* Add filters here */
