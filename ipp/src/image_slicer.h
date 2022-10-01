#pragma once
#include "image.h"

Image* SliceImages(const Image* src, Board* candidate, int* out_count);
