#include "hough_lines.h"
#include <math.h>
#define DEBUG_VIEW

Line* LineFrom(double theta, double rho, double x1, double y1, double
        x2, double y2)
{
    Line* l = (Line*) malloc(sizeof(Line));
    l->theta = theta;
    l->rho = rho;
    l->x1 = x1;
    l->y1 = y1;
    l->x2 = x2;
    l->y2 = y2;
    return l;
}

Line** HoughLines(const Image* img, size_t* found_count, int white_edge,
        size_t theta_steps, int threshold)
{
    double dtheta = M_PI/theta_steps;
    size_t w = img->width, h = img->height;
    int hsp_width = theta_steps; // M_PI / dtheta
    int hsp_height = (int)ceil(sqrt(w*w + h*h))*2;

    // Pre-generate sin, cos, 1/sin tables for faster computation.
    double sin_t[hsp_width];
    double cos_t[hsp_width];
    size_t col = 0;
    for(double th = 0; col < hsp_width; col++, th += dtheta)
    {
        sin_t[col] = sin(th);
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
    for(size_t y = 0; y < hsp_height; y++)
    {
        for(size_t x = 0; x < hsp_width; x++)
        {
            size_t i = y * hsp_width + x;
            if (acc[i] >= threshold)
            {
                double p = (double)y - hsp_height / 2;
                double x0 = cos_t[x] * p;
                double y0 = sin_t[x] * p;
                double a = -1000*sin_t[x];
                double b = 1000*cos_t[x];
                out_lines[lines++] = LineFrom(x * dtheta, p, 
                        x0 + a, y0 + b, 
                        x0 - a, y0 - b);
                if (lines == MAX_LINES)
                    break;
            }
#ifdef DEBUG_VIEW
            // better visualization in outpout image
            unsigned int col = ((0xFF * acc[i]) / max) & 0xFF;
            acc[i] = col << 16 | col << 8 | col; 
#endif
        }
        if (lines == MAX_LINES)
            break;
    }

#ifdef DEBUG_VIEW
    printf("[DEBUG_VIEW] Hough Lines accumulator saved as acc.png\n");
    SaveImageFile(accumulator, "acc.png");
#endif
    DestroyImage(accumulator);

    *found_count = lines;
    return out_lines;
}

Line** AverageLines(Line** lines, size_t len, size_t* out_len)
{
    double deltaT = 5 * M_PI / 180; // 5° tolerance
    double deltaR = 15;
    Line** out_lines = (Line**)malloc(MAX_LINES * sizeof(Line*));
    size_t p = 0;

    for(size_t i = 0; i < len; i++)
    {
        Line* candidate = lines[i];
        int add = 1;
        for(size_t j = 0; j < p; j++)
        {
            Line* other = out_lines[j];
            if (fabs(candidate->theta - other->theta) < deltaT
                    && fabs(candidate->rho - other->rho) < deltaR)
            {
                other->rho = (candidate->rho + other->rho) / 2;
                other->theta = (candidate->theta + other->theta) / 2;
                other->x1 = (candidate->x1 + other->x1) / 2;
                other->y1 = (candidate->y1 + other->y1) / 2;
                other->x2 = (candidate->x2 + other->x2) / 2;
                other->y2 = (candidate->y2 + other->y2) / 2;
                add = 0;
                break;
            }
        }
        if (add) {
            out_lines[p++] = candidate;
            if (p == MAX_LINES) break;
        }
    }
    *out_len = p;
    return out_lines;
}

int LineIntersection(const Line* l1, const Line* l2, int *x, int *y) {
    double a = cosf(l1->theta);
    double b = sinf(l1->theta);
    double c = cosf(l2->theta);
    double d = sinf(l2->theta);
    double dt = a * d - b * c; //determinative (rearranged matrix for inverse)
    if(fabs(dt) > 0.1) { // account for error, dt != 0
        if (x != NULL && y != NULL)
        {
            *x = (int)((d * l1->rho - b * l2->rho) / dt);
            *y = (int)((-c * l1->rho + a * l2->rho) / dt);
        }
        return 1;
    }
    return 0;
}

Rect** FindRects(Image* img, const Line** lines, size_t len, int* found_count)
{
    int p1_x = 0;
    int p1_y = 0;
    for(size_t i = 0; i < len; i++)
    {
        const Line* candidate = lines[i];
        for(size_t j = 0; j < len; j++)
        {
            if (i == j) continue;
            if (LineIntersection(candidate, lines[j], &p1_x, &p1_y))
            {
                if (p1_y >= 0 && p1_y < img->height && p1_x >= 0 &&
                        p1_x < img->width)
                {
                    img->pixels[p1_y * img->width + p1_x] = 0xFFFF00;
                }
            }
        }
    }
    *found_count = 0;
    return NULL;
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

void DrawLine(unsigned int* pixels, unsigned int color, unsigned int w, 
        unsigned int h, int x0, int y0, int x1, int y1)
{
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int error = dx + dy;

    while (1)
    {
        if (y0 >= 0 && y0 < h && x0 >= 0 && x0 < w)
            pixels[y0 * w + x0] = color;
        if (x0 == x1 && y0 == y1) 
            break;
        int e2 = 2 * error;
        if (e2 >= dy)
        {
            if (x0 == x1) break;
            error += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            if (y0 == y1) break;
            error += dx;
            y0 += sy;
        }
    }
}

void RenderLines(Image* image, unsigned int color, const Line** lines, int len)
{
    size_t w = image->width;
    size_t h = image->height;
    unsigned int* pix = image->pixels;
    for(size_t i = 0; i < len; i++)
    {
        const Line* l = lines[i];
        DrawLine(pix, color, w, h, (int)l->x1, (int)l->y1, (int)l->x2,
                (int)l->y2);
    }
}
