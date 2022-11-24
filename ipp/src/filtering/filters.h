#pragma once
#include "image.h"

typedef unsigned int u32;
typedef unsigned char u8;

typedef struct Kernel {
    float* vals;
    u8 radius;
} Kernel;

void FreeKernel(Kernel* kernel);
void PrintKernel(const Kernel* kernel);

void Invert(Image* img);

Kernel* GetGaussianKernel(float sigma, size_t r);
void Convolve(const Image* img, u32* out, const Kernel* ker);

void GaussianBlur(Image* img, u32* buf, float sigma, size_t r);
void MeanFilter(Image* img, u32* buf, size_t r);
void BilateralFilter(Image* img, u32* buf, size_t r, float col, float sp);
void MedianFilter(Image* img, u32* buf, size_t block);
void SobelOperator(const Image* img, u32* out, float* dirs, float* max_mag);

void GammaFilter(Image* img, float f);
void Dilate(Image* img, u32* buf, size_t block);
void Erode(Image* img, u32* buf, size_t block);
