#include <stdio.h>
#include "image_filter.h"

const char IND_LOAD[] = "..";
const char IND_OK[] = "\033[32;1mOK\033[0m";
const float GAUS_KERNEL_3[] =
{
    1/16.0f, 2/16.0f, 1/16.0f,
    2/16.0f, 4/16.0f, 2/16.0f,
    1/16.0f, 2/16.0f, 1/16.0f,
};
const float GAUS_KERNEL_5[] =
{
    1/273.0F, 4/273.0F, 7/273.0F, 4/273.0F, 1/273.0F,
    4/273.0F, 16/273.0F, 26/273.0F, 16/273.0F, 4/273.0F,
    7/273.0F, 26/273.0F, 41/273.0F, 26/273.0F, 7/273.0F,
    4/273.0F, 16/273.0F, 26/273.0F, 16/273.0F, 4/273.0F,
    1/273.0F, 4/273.0F, 7/273.0F, 4/273.0F, 1/273.0F,
};

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
    u8 s = 1;
    printf("Processing image...\n");

    PrintStage(s, 5, "Grayscale filter", 0);
    u8 min = 255, max = 0;
    GrayscaleFilter(img, &min, &max);
    PrintStage(s++, 5, "Grayscale filter", 1);

    PrintStage(s, 5, "Contrast stretching", 0);
    StretchContrast(img, min, max);
    PrintStage(s++, 5, "Contrast stretching", 1);

    PrintStage(s, 5, "Gaussian blur (3x3)", 0);
    GaussianBlur(img, GAUS_KERNEL_3, 3);
    PrintStage(s++, 5, "Gaussian blur (3x3)", 1);

    u32 histogram[256] = { 0, };
    PrintStage(s, 5, "Median filter (3x3)", 0);
    MedianFilter(img, 3, histogram);
    PrintStage(s++, 5, "Median filter (3x3)", 1);

    PrintStage(s, 5, "Thresholding (Otsu's method)", 0);
    u8 threshold = ComputeOtsuThreshold(img->width * img->height, histogram);
    ThresholdImage(img, threshold);
    PrintStage(s++, 5, "Thresholding (Otsu's method)", 1);
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

void EnhanceContrast(Image* image, u8 f, u8* min, u8* max)
{
    size_t len = image->width * image->height;
    for (size_t i = 0; i < len; i++)
    {
        u32 c = image->pixels[i] & 0xFF;
        for (size_t k = 1; k < f; k++)
        {
            if (c >= (k * 255) / f && c <= ((k + 1) * 255) / f)
            {
                c = ((k + 1) * 255) / f;
                image->pixels[i] = c | (c << 8) | (c << 16);

                // Prepapre for next stage: Contrast stretching
                if (c > *max) *max = c;
                else if (c < *min) *min = c;
                break;
            }
        }
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

void GaussianBlur(Image* img, const float* kernel, size_t r)
{
    size_t w = img->width, h = img->height;
    size_t side = r / 2;
    for (size_t y = side; y < h - side; y++)
    {
        for (size_t x = side; x < w - side; x++)
        {
            float sum = 0;
            for (size_t dy = 0; dy < r; dy++)
            {
                for (size_t dx = 0; dx < r; dx++)
                {
                    u32 c = img->pixels[(y + dy - side) * w + (x + dx - side)];
                    sum += (float)(c & 0xFF) * kernel[dy * r + dx];
                }
            }

            u8 c = sum;
            img->pixels[y * w + x] = (c << 16) | (c << 8) | c;
        }
    }
}

void GammaFilter(Image* img, float f)
{
    size_t len = img->width * img->height;

    for (size_t i = 0; i < len; i++)
    {
        float c = 255.0f * pow((float)(img->pixels[i] & 0xFF) / 255.0f, f);
        u8 g = (c > 255 ? 255 : (c < 0 ? 0 : c));

        img->pixels[i] = (g << 16) | (g << 8) | g;
    }
}

void FillHistogram(const Image* image, u32 histogram[256])
{
    for (size_t i = 0; i < 256; i++)
        histogram[i] = 0;
    size_t len = image->width * image->height;

    for (size_t i = 0; i < len; i++)
    {
        u8 c = image->pixels[i] & 0xFF;
        histogram[c]++;
    }
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

void AdapativeThresholding(Image* img, size_t r, float threshold)
{
    // FIXME: Not working as attended
    size_t w = img->width, h = img->height;
    size_t side = r / 2;
    for (size_t y = side; y < h - side; y++)
    {
        for (size_t x = side; x < w - side; x++)
        {
            float sum = 0;
            ssize_t endX = x + side;
            ssize_t endY = y + side;
            for (ssize_t dy = y - side; dy <= endY; dy++)
            {
                for (ssize_t dx = x - side; dx <= endX; dx++)
                {
                    if (dy >= 0 && dx >= 0 && dy < h && dx < w)
                        sum += img->pixels[dy * w + dx] & 0xFF;
                }
            }
            float m = (sum / (r * r));
            img->pixels[y * w + x] = (img->pixels[y * w + x] & 0xFF)
                >= m ? 0 : 0xFFFFFF;
        }
    }
}

void SobelOperator(const Image* img, u32* out, u32* max_mag)
{
    int Iy[9] = 
    {
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1,
    };
    int Ix[9] = 
    {
         1,  2,  1,
         0,  0,  0,
        -1, -2, -1,
    };
    size_t w = img->width, h = img->height;
    *max_mag = 0;
    for (size_t y = 1; y < h - 1; y++)
    {
        for (size_t x = 1; x < w - 1; x++)
        {
            int gx = 0, gy = 0;
            for (ssize_t dy = -1; dy <= 1; dy++)
            {
                for (ssize_t dx = -1; dx <= 1; dx++)
                {
                    gx += (img->pixels[(y + dy) * w + x + dx] & 0xFF)
                        * Ix[(dy + 1) * 3 + dx + 1];
                    gy += (img->pixels[(y + dy) * w + x + dx] & 0xFF)
                        * Iy[(dy + 1) * 3 + dx + 1];
                }
            }
            u32 mag = sqrt(gx * gx + gy * gy);
            if (mag > *max_mag) *max_mag = mag;
            out[y * w + x] = mag;
        }
    }
}

Image* CannyEdgeDetection(const Image* src)
{
    size_t len = src->width * src->height;
    Image* out = CreateImage(0, src->width, src->height, NULL);
    if (out == NULL) return NULL;

    u32* mat = calloc(len, sizeof(u32));
    u32 max = 0;
    SobelOperator(src, buf, &max);

    // Rendering
    for (size_t i = 0; i < len; i++)
    {
        u8 c = (mat[i] * 255) / max;
        out->pixels[i] = (c << 16) | (c << 8) | c;
    }

    free(mat);
    return out;
}
