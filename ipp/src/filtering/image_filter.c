#include "image_filter.h"
#include "../utils.h"

const float GAUS_KERNEL_3[] =
{
    1/16.0f, 2/16.0f, 1/16.0f,
    2/16.0f, 4/16.0f, 2/16.0f,
    1/16.0f, 2/16.0f, 1/16.0f,
};
const float CANN_KERNEL_5[] =
{
    2/159.0f,  4/159.0f,  5/159.0f,  4/159.0f, 2/159.0f,
    4/159.0f,  9/159.0f, 12/159.0f,  9/159.0f, 4/159.0f,
    5/159.0f, 12/159.0f, 15/159.0f, 12/159.0f, 5/159.0f,
    4/159.0f,  9/159.0f, 12/159.0f,  9/159.0f, 4/159.0f,
    2/159.0f,  4/159.0f,  5/159.0f,  4/159.0f, 2/159.0f,
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

void FilterImage(Image* img)
{
    u8 s = 1, t = 4;

    PrintStage(s, t, "Grayscale filter", 0);
    u8 min = 255, max = 0;
    GrayscaleFilter(img, &min, &max);
    PrintStage(s++, t, "Grayscale filter", 1);

    PrintStage(s, t, "Contrast stretching", 0);
    printf(" --> Min/Max: %hhu/%hhu", min, max);
    StretchContrast(img, min, max);
    PrintStage(s++, t, "Contrast stretching", 1);

    if (SaveImageFile(img, "grayscale.png"))
        printf("Successfully saved grayscale.png\n");

    u32 histogram[256] = { 0, };
    PrintStage(s, t, "Median filter (3x3)", 0);
    MedianFilter(img, 5, histogram);
    PrintStage(s++, t, "Median filter (3x3)", 1);

    if (SaveImageFile(img, "median.png"))
        printf("Successfully saved median.png\n");

    PrintStage(s, t, "Thresholding (Otsu's method)", 0);
    u8 threshold = ComputeOtsuThreshold(img->width * img->height, histogram);
    printf(" --> Threshold: %hhu", threshold);
    ThresholdImage(img, threshold);
    PrintStage(s++, t, "Thresholding (Otsu's method)", 1);
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

void EnhanceContrast(Image* img, u32 histogram[256])
{
    size_t len = img->width * img->height;
    for (size_t i = 0; i < len; i++)
    {
        u8 col = img->pixels[i] & 0xFF;
        float f = (float)histogram[col] / (float)len;
        float v = (259.0f * (255.0f + f)) / (255.0f * (259.0f - f));
        col = v * (col - 128.0f) + 128.0f;
        img->pixels[i] = (col << 16) | (col << 8) | col;
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
    memset(histogram, 0, 256 * sizeof(u32));
    size_t w = img->width, h = img->height;
    size_t side = block / 2;
    u32* dst = calloc(w * h, sizeof(u32));
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
            dst[y * w + x] = (c << 16) | (c << 8) | c;

            // Prepapre for next stage: Otsu's method
            histogram[c]++;
        }
    }


    memcpy(img->pixels, dst, w * h * sizeof(unsigned int));
    free(dst);
    free(vals);
}

void GaussianBlur(Image* img, const float* kernel, size_t r)
{
    size_t w = img->width, h = img->height;
    u32* dst = calloc(w * h, sizeof(u32));
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
            dst[y * w + x] = (c << 16) | (c << 8) | c;
        }
    }

    memcpy(img->pixels, dst, w * h * sizeof(unsigned int));
    free(dst);
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
    memset(histogram, 0, 256 * sizeof(u32));
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
    size_t w = img->width, h = img->height;
    u32* dst = calloc(w * h, sizeof(u32));
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
            float m = (sum / (r * r)) + threshold;
            dst[y * w + x] = (img->pixels[y * w + x] & 0xFF)
                >= m ?  0xFFFFFF : 0;
        }
    }

    memcpy(img->pixels, dst, w * h * sizeof(unsigned int));
    free(dst);
}

void SobelOperator(const Image* img, u32* out, float* dirs, u32* max_mag)
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
    for (size_t y = 0; y < h; y++)
    {
        for (size_t x = 0; x < w; x++)
        {
            int gx = 0, gy = 0;
            for (ssize_t dy = -1; dy <= 1; dy++)
            {
                for (ssize_t dx = -1; dx <= 1; dx++)
                {
                    ssize_t nx = x + dx;
                    ssize_t ny = y + dy;
                    if (nx < 0 || ny < 0 || nx >= w || ny >= h) continue;
                    u8 c = (img->pixels[ny * w + nx] & 0xFF);
                    gx += c * Ix[(dy + 1) * 3 + dx + 1];
                    gy += c * Iy[(dy + 1) * 3 + dx + 1];
                }
            }
            u32 mag = sqrt(gx * gx + gy * gy);
            if (mag > *max_mag) *max_mag = mag;
            out[y * w + x] = mag;
            dirs[y * w + x] = atan2(gy, gx);
        }
    }
}

void NonMaximumSuppression(u32* mat, float* dirs, size_t w, size_t h)
{
    for (size_t y = 1; y < h - 1; y++)
    {
        for (size_t x = 1; x < w - 1; x++)
        {
            size_t i = y * w + x;
            u32 mag = mat[i];
            char ring_pos = round(dirs[i] / (M_PI / 4));
            switch (ring_pos)
            {
                case 3:
                case -1: // -PI/4 && -3PI/4
                    if (mat[(y + 1) * w + (x - 1)] > mag
                            || mat[(y - 1) * w + (x + 1)] > mag)
                        mat[i] = 0;
                    break;
                case -4:
                case 4:
                case 0: // 0 && PI && -PI
                    if (mat[y * w + (x + 1)] > mag
                            || mat[y * w + (x - 1)] > mag)
                        mat[i] = 0;
                    break;
                case -3:
                case 1: // PI/4 & 3PI/4
                    if (mat[(y - 1) * w + (x - 1)] > mag
                            || mat[(y + 1) * w + (x + 1)] > mag)
                        mat[i] = 0;
                    break;
                case -2:
                case 2: // PI/2 && -PI/2
                    if (mat[(y + 1) * w + x] > mag
                            || mat[(y - 1) * w + x] > mag)
                        mat[i] = 0;
                    break;
                default:
                    printf("NonMaximumSuppression::Unrecheable:"
                           " Unknown ring pos: %hhi\n", ring_pos);
                    break;
            }
        }
    }
}

void DoubleThresholding(u32* mat, size_t len, u32 max, float weak_t,
        float strong_t, u32 weak, u32 strong)
{
    u32 high = max * strong_t;
    u32 low = high * weak_t;

    for (size_t i = 0; i < len; i++)
    {
        u8 c = mat[i];
        mat[i] = (c >= high ? strong : (c >= low ? weak : 0));
    }
}

void Hysteresis(u32* mat, size_t w, size_t h, u32 weak, u32 strong)
{
    for (size_t y = 1; y < h - 1; y++)
    {
        for (size_t x = 1; x < w - 1; x++)
        {
            u32 mag = mat[y * w + x];
            if (mag != weak) continue;
            u32 new_val = 0;
            for (size_t d = 0; d < 9; d++)
            {
                size_t dx = x + (d % 3) - 1;
                size_t dy = y + (d / 3) - 1;
                if (mat[dy * w + dx] == strong)
                {
                    new_val = strong;
                    break;
                }
            }
            mat[y * w + x] = new_val;
        }
    }
}

Image* CannyEdgeDetection(const Image* src)
{
    size_t len = src->width * src->height;
    Image* out = LoadBufImage(src->pixels, src->width, src->height, NULL);
    if (out == NULL) return NULL;

    PrintStage(1, 2, "Gaussian blur (3x3)", 0);
    GaussianBlur(out, CANN_KERNEL_5, 3);
    PrintStage(1, 2, "Gaussian blur (3x3)", 1);

    u32* mat = calloc(len, sizeof(u32));
    float* dirs = calloc(len, sizeof(float));
    u32 max = 0;

    PrintStage(2, 3, "Sobel Operator", 0);
    SobelOperator(out, mat, dirs, &max);
    PrintStage(2, 3, "Sobel Operator", 1);

    PrintStage(3, 3, "Edge thinning", 0);
    NonMaximumSuppression(mat, dirs, src->width, src->height);
    DoubleThresholding(mat, len, max, 0.25, 0.10, 50, 255);
    Hysteresis(mat, src->width, src->height, 50, 255);
    PrintStage(3, 3, "Edge thinning", 1);

    // Rendering
    for (size_t i = 0; i < len; i++)
    {
        u8 c = 2 * mat[i] / 3;
        out->pixels[i] = (c << 16) | (c << 8) | c;
    }

    free(dirs);
    free(mat);
    return out;
}
