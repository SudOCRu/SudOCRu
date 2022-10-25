#include <stdio.h>
#include "image_filter.h"

const char IND_LOAD[] = "..";
const char IND_OK[] = "\033[32;1mOK\033[0m";

size_t max(size_t a, size_t b)
{
    return a > b ? a : b;
}

size_t min(size_t a, size_t b)
{
    return a < b ? a : b;
}

void array_insert(u8* begin, u8* end, u8 val)
{
    for(; end > begin && val < *(end - 1); end--)
        *end = *(end - 1);
    *end = val;
}

void PrintStage(u8 id, u8 total, char* stage, int ok)
{
    const char* indicator = ok ? IND_OK : IND_LOAD;
    if (ok)
    {
        printf("\r%hhu/%hhu [%s] %s\n", id, total, indicator, stage);
    }
    else
    {
        printf("%hhu/%hhu [%s] %s", id, total, indicator, stage);
        fflush(stdout);
    }
}

void FilterImage(Image* img)
{
    printf("Processing image...\n");

    PrintStage(1, 4, "Grayscale filter", 0);
    u8 min = 255, max = 0;
    GrayscaleFilter(img, &min, &max);
    PrintStage(1, 4, "Grayscale filter", 1);

    PrintStage(2, 4, "Contrast stretching", 0);
    StretchContrast(img, min, max);
    PrintStage(2, 4, "Contrast stretching", 1);

    PrintStage(3, 4, "Median filter", 0);
    u32 histogram[256] = { 0, };
    MedianFilter(img, 3, histogram);
    PrintStage(3, 4, "Median filter", 1);

    PrintStage(4, 4, "Thresholding (Otsu's method)", 0);
    u8 threshold = ComputeOtsuThreshold(img->width * img->height, histogram);
    ThresholdImage(img, threshold);
    PrintStage(4, 4, "Thresholding (Otsu's method)", 1);
}

void GrayscaleFilter(Image* image, u8* min, u8* max)
{
    size_t len = image->width * image->height;
    for (size_t i = 0; i < len; i++)
    {
        u32 c = image->pixels[i];
        u8 red = (c >> 16) & 0xFF, green = (c >> 8) & 0xFF, blue = c & 0xFF;
        u8 g = 0.299F * red + 0.587F * green + 0.114F * blue;

        // Prepapre for next stage: Contrast stretching
        if (g > *max) *max = g;
        else if (g < *min) *min = g;

        image->pixels[i] = g | (g << 8) | (g << 16);
    }
}

void StretchContrast(Image* img, u8 min, u8 max)
{
    if (min == 0 && max == 255) return;

    size_t len = img->width * img->height;
    for (size_t i = 0; i < len; i++)
    {
        u8 col = img->pixels[i] & 0xFF;
        col = ((col - min) * 255) / (max - min);
        img->pixels[i] = (col << 16) | (col << 8) | col;
    }
}

void MedianFilter(Image* img, size_t block, u32 histogram[256])
{
    size_t w = img->width, h = img->height;
    size_t side = block / 2;
    u8* vals = calloc(block * block, sizeof(u8));
    for (size_t y = 0; y < h; y++)
    {
        for (size_t x = 0; x < w; x++)
        {
            size_t i = 0;
            ssize_t endX = x + side;
            ssize_t endY = y + side;
            for (ssize_t dy = y - side; dy <= endY; dy++)
            {
                for (ssize_t dx = x - side; dx <= endX; dx++)
                {
                    u8 col = 0;
                    if (dy >= 0 && dx >= 0 && dy < h && dx < w)
                        col = img->pixels[dy * w + dx] & 0xFF;

                    array_insert(vals, vals + i, col);
                    i++;
                }
            }

            u8 c = vals[i / 2];
            img->pixels[y * w + x] = (c << 16) | (c << 8) | c;

            // Prepapre for next stage: Otsu's method
            histogram[c]++;
        }
    }

    free(vals);
}

u8 ComputeOtsuThreshold(size_t len, const u32 histogram[256])
{
    u32 sum = 0, sumP = 0;
    for (size_t i = 0; i < 256; i++)
        sum += histogram[i] * i;

    u32 threshold = 0, var_max = 0, q1 = 0, q2 = 0, u1 = 0, u2 = 0;
    for (size_t t = 0; t < 256; t++)
    {
        q1 += histogram[t];
        if (q1 == 0) continue;
        q2 = len - q1 + 1;

        sumP += histogram[t] * t;
        u1 = sumP / q1;
        u2 = (sum - sumP) / q2;

        u32 var = q1 * q2 * (u1 - u2) * (u1 - u2);

        if (var > var_max)
        {
            threshold = t;
            var_max = var;
        }
    }

    return threshold;
}

void ThresholdImage(Image* image, u8 threshold)
{
    size_t len = image->width * image->height;

    for (size_t i = 0; i < len; i++)
    {
        u32 c = image->pixels[i];
        image->pixels[i] = (c & 0xFF) > threshold ? 0 : 0xFFFFFF;
    }
}
