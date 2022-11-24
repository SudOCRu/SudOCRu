#pragma once
#include "image.h"

typedef unsigned int u32;
typedef unsigned char u8;

/* Apply all filters to the given image. The image must not be null. */
void FilterImage(Image* image, int flags);

/* Applys a grayscale filter to the image and fills the histogram array */
void GrayscaleFilter(Image* image, u8* min, u8* max);
void StretchContrast(Image* img, u8 min, u8 max);

void EnhanceContrast(Image* image, u8 f, u8* min, u8* max);
void BilateralFilter(Image* img, u32* buf, size_t r, float col, float sp);
void AutoBrigthness(Image* image, float clip);

void FillHistogram(const Image* image, u32 histogram[256]);
u8 ComputeOtsuThreshold(size_t len, const u32 histogram[256]);

/* Applys a binary tresholding filter to the image */
void ThresholdImage(Image* image, u8 threshold);
void AdaptiveThresholding(Image* img, u32* buf, size_t r, float threshold);
