#include "hough_lines.h"
#include <math.h>
#include <sys/time.h>

Line* LineFrom(double theta, double rho)
{
    Line* l = (Line*) malloc(sizeof(Line));
    l->theta = theta;
    l->rho = rho;
    return l;
}

Line** HoughLines(const Image* img, size_t* found_count)
{
    double theta_step = M_PI/480;
    double rho_accuracy = 1;

    size_t w = img->width, h = img->height;
    int hsp_width = M_PI / theta_step;
    int hsp_height = (int)ceil(sqrt(w*w + h*h))*2;

    Image* accumulator = CreateImage(0, hsp_width, hsp_height, NULL);
    if (accumulator == NULL)
    {
        printf("Not enough memory for acc (%ix%i) \n", hsp_width, hsp_height);
        return NULL;
    }
    const unsigned int* pixels = img->pixels;
    unsigned int* acc = accumulator->pixels;

    size_t lines = 0;
    unsigned int max = 0;
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);       
    for(size_t y = 0; y < h; y++)
    {
        for(size_t x = 0; x < w; x++)
        {
            if (pixels[y * w + x] == 0)
            {
                size_t col = 0;
                for(double th = 0; th <= M_PI; th += theta_step, col++)
                {
                    double p = hsp_height / 2 + x * cos(th) + y * sin(th);
                    size_t idx = hsp_width * (size_t)p + col;
                    acc[idx] += 1;
                    if (acc[idx] > max) max = acc[idx];
                }
            }
        }
    }

    Line** out_lines = (Line**)malloc(MAX_LINES * sizeof(Line*));
    unsigned int threshold = (unsigned int) round(max * 0.75);
    for(size_t y = 0; y < hsp_height; y++)
    {
        for(size_t x = 0; x < hsp_width; x++)
        {
            size_t i = y * hsp_width + x;
            if (acc[i] > threshold)
            {
                double theta = x * theta_step;
                double p = (double)y - hsp_height / 2;
                out_lines[lines] = LineFrom(theta, p);
                lines++;
            }
            acc[i] *= 100;
        }
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000
        + (end.tv_nsec - start.tv_nsec) / 1000;
    printf("done in %fms\n", delta_us * 0.001);

    printf("------\n");
    printf("Nb lines: %lu\n", lines);
    printf("Threshold: %u\n", threshold);
    printf("Max: %u\n", max);
    printf("Accumulator saved as acc.png\n");
    SaveImageFile(accumulator, "acc.png");
    printf("------\n");
    DestroyImage(accumulator);

    *found_count = lines;
    return out_lines;
}

void FreeLines(Line** lines, size_t len)
{
    if (lines == NULL) return;
    for(size_t i = 0; i < len; i++)
    {
        free(lines[i]);
    }
    free(lines);
}

void RenderLines(Image* image, unsigned int color, Line** lines, int len)
{
    // TODO
}
