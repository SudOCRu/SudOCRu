#include "hough_lines.h"
#include <math.h>
#include <sys/time.h>

Line* LineFrom(double theta, double rho, double a, double b)
{
    Line* l = (Line*) malloc(sizeof(Line));
    l->theta = theta;
    l->rho = rho;
    l->a = a;
    l->b = b;
    return l;
}

Line** HoughLines(const Image* img, size_t* found_count, int white_edge,
        size_t theta_steps, int threshold)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    double dtheta = M_PI/theta_steps;
    size_t w = img->width, h = img->height;
    int hsp_width = theta_steps; // M_PI / dtheta
    int hsp_height = (int)ceil(sqrt(w*w + h*h))*2;

    double sin_t[hsp_width];
    double isin_t[hsp_width];
    double cos_t[hsp_width];
    size_t col = 0;
    for(double th = 0; col < hsp_width; th += dtheta, col++)
    {
        sin_t[col] = sin(th);
        isin_t[col] = 1.0/sin_t[col];
        cos_t[col] = cos(th);
    }

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

    for(size_t y = 0; y < h; y++)
    {
        for(size_t x = 0; x < w; x++)
        {
            if ((pixels[y * w + x] != 0) == white_edge)
            {
                for(col = 0; col < hsp_width; col++)
                {
                    double p = hsp_height/2 + x * cos_t[col] + y * sin_t[col];
                    size_t idx = hsp_width * (size_t)p + col;
                    acc[idx] += 1;
                    if (acc[idx] > max) max = acc[idx];
                }
            }
        }
    }

    Line** out_lines = (Line**)malloc(MAX_LINES * sizeof(Line*));
    if (threshold < 0)
    {
        threshold = max * -threshold / 100;
    }
    /*unsigned int threshold = (unsigned int) round(max * 0.75);*/
    for(size_t y = 0; y < hsp_height; y++)
    {
        for(size_t x = 0; x < hsp_width; x++)
        {
            size_t i = y * hsp_width + x;
            if (acc[i] > threshold)
            {
                double theta = x * dtheta;
                double p = (double)y - hsp_height / 2;
                double a = -cos_t[x] * isin_t[x];
                double b = p * isin_t[x];
                out_lines[lines++] = LineFrom(theta, p, a, b);
                if (lines == MAX_LINES)
                    break;
            }
            acc[i] *= 100;
        }
        if (lines == MAX_LINES)
            break;
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000
        + (end.tv_nsec - start.tv_nsec) / 1000;
    printf("done in %fms\n", delta_us * 0.001);

    printf("------\n");
    printf("Nb lines: %lu\n", lines);
    printf("Threshold: %u\n", threshold);
    printf("Max: %u\n", max);
    /*
    printf("Accumulator saved as acc.png\n");
    SaveImageFile(accumulator, "acc.png");
    */
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
    size_t w = image->width;
    size_t h = image->height;
    unsigned int* pix = image->pixels;
    for(size_t i = 0; i < len; i++)
    {
        Line* l = lines[i];
        if (l->theta == 0)
        {
            if (l->rho >= 0 && l->rho <= h)
            {
                size_t x = (size_t)(l->rho);
                for(size_t y = 0; y < h; y++)
                {
                    pix[y * w + x] = color;
                }
            }
        }
        else
        {
            for(size_t x = 0; x < w; x++)
            {
                double y = l->a * (double)x + l->b;
                if (y >= 0 && y < h)
                {
                    pix[(size_t)y * w + x] = color;
                }
            }
        }
    }
}
