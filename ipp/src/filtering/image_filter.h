#pragma once
#include "image.h"
#define THRESH_OPTIMAL 0.025

typedef unsigned int u32;
typedef unsigned char u8;

/* Apply all filters to the given image. The image must not be null. */
void FilterImage(Image* image, u32* tmp, int flags);
void BinarizeImage(Image* image, u32* tmp, float threshold);

/* Prepare cell for OCR */
int CleanCell(Image* img, u8* markers);
Image* PrepareCell(const Image* cell, u8* markers);

/* Applys a grayscale filter to the image and fills the histogram array */
void GrayscaleFilter(Image* image, u8* min, u8* max);
void StretchContrast(Image* img, u8 min, u8 max);
void AutoBrigthness(Image* image, float clip);

// Thresholding

void FillHistogram(const Image* image, u32 histogram[256]);
u8 ComputeOtsuThreshold(size_t len, const u32 histogram[256]);
/* Applys a binary tresholding filter to the image */
void ThresholdImage(Image* image, u8 threshold);
void AdaptiveThresholding(Image* img, u32* buf, size_t r, float threshold);
