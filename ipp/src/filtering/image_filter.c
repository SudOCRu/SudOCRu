#include "image_filter.h"
#include "filters.h"
#include "edge_detection.h"
#include "../utils.h"

void FilterImage(Image* img, int flags)
{
    u32* tmp = calloc(img->width * img->height, sizeof(u32));
    u8 s = 1, t = 6;

    PrintStage(s, t, "Grayscale filter", 0);
    u8 min = 255, max = 0;
    GrayscaleFilter(img, &min, &max);
    PrintStage(s++, t, "Grayscale filter", 1);

    PrintStage(s, t, "Contrast stretching", 0);
    printf(" --> Min/Max: %hhu/%hhu", min, max);
    StretchContrast(img, min, max);
    PrintStage(s++, t, "Contrast stretching", 1);

    if ((flags & SC_FLG_DGRS) != 0 && SaveImageFile(img, "grayscale.png"))
        printf("Successfully saved grayscale.png\n");

    PrintStage(s, t, "Median Filter (5x5)", 0);
    MedianFilter(img, tmp, 5);
    PrintStage(s++, t, "Median Filter (5x5)", 1);

    PrintStage(s, t, "Bilateral Filter (11x11)", 0);
    BilateralFilter(img, tmp, 11, 70, 70);
    PrintStage(s++, t, "Bilateral Filter (11x11)", 1);

    if ((flags & SC_FLG_DMED) != 0 && SaveImageFile(img, "smoothed.png"))
        printf("Successfully saved smoothed.png\n");

    PrintStage(s, t, "AdaptiveThresholding (7x7)", 0);
    AdaptiveThresholding(img, tmp, 7, 5);
    PrintStage(s++, t, "AdaptiveThresholding (7x7)", 1);

    PrintStage(s, t, "Dilate (3x3)", 0);
    Dilate(img, tmp, 3);
    PrintStage(s++, t, "Dilate (3x3)", 1);

    size_t len = img->width * img->height;
    for (size_t i = 0; i < len; i++)
    {
        u32 pix = img->pixels[i] & 0xFF;
        img->pixels[i] = (pix << 16) | (pix << 8) | pix;
    }
    free(tmp);
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

void AutoBrigthness(Image* image, float clip) {
    u32 hist[256] = { 0, };
    FillHistogram(image, hist);

    u32 acc[256] = { 0, };
    acc[0] = hist[0];
    for (size_t i = 1; i < 256; i++)
        acc[i] = acc[i - 1] + hist[i];

    float threshold = clip * 0.5 * acc[255];
    u8 min = 0;
    while (min < 255 && acc[min] < threshold) min++;

    threshold = (1 - clip) * 0.5 * acc[255];
    u8 max = 255;
    while (max > 0 && acc[max] >= threshold) max--;

    size_t len = image->width * image->height;
    float alpha = 255 / (max - min);
    float beta = -min * alpha;

    for (size_t i = 0; i < len; i++)
    {
        float col = alpha * (float)(image->pixels[i] & 0xFF) + beta;
        u8 c = col > 255 ? 255 : (col < 0 ? 0 : (u8)col);

        image->pixels[i] = (c << 16) | (c << 8) | c;
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

void AdaptiveThresholding(Image* img, u32* buf, size_t r, float threshold)
{
    size_t w = img->width, h = img->height;
    memset(buf, 0, w * h * sizeof(u32));
    size_t side = r / 2;
    for (size_t y = side; y < h - side; y++)
    {
        for (size_t x = side; x < w - side; x++)
        {
            u32 sum = 0;
            size_t endX = x + side;
            size_t endY = y + side;
            for (size_t dy = y - side; dy <= endY; dy++)
            {
                for (size_t dx = x - side; dx <= endX; dx++)
                {
                    if (dy >= 0 && dx >= 0 && dy < h && dx < w)
                        sum += img->pixels[dy * w + dx] & 0xFF;
                }
            }
            float m = ((float)sum / (float)(r * r)) - threshold;
            buf[y * w + x] = (img->pixels[y * w + x] & 0xFF) >= m ?
                0 : 0xFFFFFF;
        }
    }

    memcpy(img->pixels, buf, w * h * sizeof(u32));
}
