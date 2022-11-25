#include "filters.h"
#include "../utils.h"

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

Image* CannyEdgeDetection(const Image* src, u32* mat)
{
    size_t len = src->width * src->height;
    Image* out = LoadBufImage(src->pixels, src->width, src->height, NULL);
    if (out == NULL) return NULL;

    PrintStage(1, 2, "Gaussian blur (3x3)", 0);
    GaussianBlur(out, mat, 1, 5);
    PrintStage(1, 2, "Gaussian blur (3x3)", 1);

    memset(mat, 0, len * sizeof(u32));
    float* dirs = calloc(len, sizeof(float));
    float max = 0;

    PrintStage(2, 3, "Sobel Operator", 0);
    SobelOperator(out, mat, dirs, &max);
    PrintStage(2, 3, "Sobel Operator", 1);

    PrintStage(3, 3, "Processing edges", 0);
    NonMaximumSuppression(mat, dirs, src->width, src->height);
    DoubleThresholding(mat, len, max, 0.25, 0.10, 50, 255);
    Hysteresis(mat, src->width, src->height, 50, 255);
    PrintStage(3, 3, "Processing edges", 1);

    // Rendering
    for (size_t i = 0; i < len; i++)
    {
        u8 c = 2 * mat[i] / 3;
        out->pixels[i] = (c << 16) | (c << 8) | c;
    }

    free(dirs);
    return out;
}
