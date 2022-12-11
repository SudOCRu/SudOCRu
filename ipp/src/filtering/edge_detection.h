#pragma once
#include "image.h"

void SobelOperator(const Image* img, u32* out, float* dirs, float* max_mag);
Image* CannyEdgeDetection(const Image* src, u32* tmp);
