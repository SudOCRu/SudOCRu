#include <err.h>
#include <math.h>
#include "hough_lines.h"
#include "renderer.h"

Line* LineFrom(unsigned int val, float theta, float rho, float x1,
        float y1, float x2, float y2)
{
    Line* l = (Line*) malloc(sizeof(Line));
    l->val = val;
    l->theta = theta;
    l->rho = rho;
    l->x1 = x1;
    l->y1 = y1;
    l->x2 = x2;
    l->y2 = y2;
    return l;
}

Line** HoughLines(const Image* img, size_t* found_count, int white_edge,
        size_t theta_steps, int threshold, int save)
{
    size_t w = img->width, h = img->height;
    size_t hsp_height = (int)ceil(sqrt(w*w + h*h));
    size_t h_d2 = hsp_height / 2;
    float dtheta = M_PI / theta_steps;
    size_t hsp_width = M_PI / dtheta; // M_PI / dtheta
    float max_angle = 0;

    // Pre-generate sin, cos tables for faster computation.
    float sin_t[hsp_width];
    float cos_t[hsp_width];
    size_t col = 0;
    for (float th = -max_angle; col < hsp_width; col++, th += dtheta)
    {
        sin_t[col] = sin(th);
        cos_t[col] = cos(th);
    }

    Image* accumulator = CreateImage(0, hsp_width, hsp_height, NULL);
    if (accumulator == NULL)
    {
        errx(EXIT_FAILURE, "Not enough memory for Hough Space (%lux%lu) \n",
                hsp_width, hsp_height);
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
                    float r = x * cos_t[col] + y * sin_t[col];
                    int rs = round((r * h_d2) / hsp_height) + h_d2;
                    size_t idx = hsp_width * rs + col;
                    acc[idx] += 1;
                    if (acc[idx] > max) max = acc[idx];
                }
            }
        }
    }

    Line** out_lines = (Line**)malloc(MAX_LINES * sizeof(Line*));
    unsigned int t = threshold < 0 ? (max * -threshold / 100) :
	    (unsigned int) threshold;
    for(size_t y = 0; y < hsp_height; y++)
    {
        for(size_t x = 0; x < hsp_width; x++)
        {
            size_t i = y * hsp_width + x;
            if (acc[i] >= t)
            {
                float theta = x * dtheta - max_angle;
                float p = ((float)y - h_d2) * hsp_height / h_d2;
                float x0 = cos_t[x] * p;
                float y0 = sin_t[x] * p;
                float a = -10000*sin_t[x];
                float b = 10000*cos_t[x];
                out_lines[lines++] = LineFrom(acc[i], theta, p,
                        x0 + a, y0 + b,
                        x0 - a, y0 - b);
                if (lines == MAX_LINES)
                    break;
            }
            if (save)
            {
                // better visualization in outpout image
                unsigned int col = ((0xFF * acc[i]) / max) & 0xFF;
                acc[i] = col << 16 | col << 8 | col;
            }
        }
        if (lines == MAX_LINES)
            break;
    }

    if (save && SaveImageFile(accumulator, "acc.png"))
        printf("Successfully wrote acc.png (Hough Lines accumulator)\n");

    DestroyImage(accumulator);

    *found_count = lines;
    return out_lines;
}

Line** AverageLines(Line** lines, size_t len, size_t* out_len)
{
    float deltaT = 5 * M_PI / 180;
    float deltaR = 15;
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
                other->val   = (candidate->val   + other->val)   / 2;
                other->rho   = (candidate->rho   + other->rho)   / 2;
                other->theta = (candidate->theta + other->theta) / 2;
                other->x1    = (candidate->x1    + other->x1)    / 2;
                other->y1    = (candidate->y1    + other->y1)    / 2;
                other->x2    = (candidate->x2    + other->x2)    / 2;
                other->y2    = (candidate->y2    + other->y2)    / 2;
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

PSet** GroupParallelLines(Line** lines, size_t len, size_t* out_len)
{
    float Tt = 8 * M_PI / 180; // max angle diff between two lines
    float Tl = 0.4; // threshold for line segements to have similar vote counts

    PSet** pairs = malloc(sizeof(PSet*) * len * len);
    size_t nb_pairs = 0;

    for (size_t i = 0; i < len; i++)
    {
        const Line* l1 = lines[i];
        for (size_t j = 0; j < len; j++)
        {
            if (i == j) continue;
            const Line* l2 = lines[j];
            float dT = fabs(l1->theta - l2->theta);
            float dT2 = fabs(l1->theta - l2->theta - M_PI);
            float dC = fabs((float)l1->val - (float)l2->val);
            float mid = (l1->val + l2->val) / 2;

            if ((dT < Tt || dT2 < Tt) && dC < Tl * mid)
            {
                PSet* ep = malloc(sizeof(PSet));
                ep->l1 = l1;
                ep->l2 = l2;
                if (dT2 < Tt)
                {
                    ep->alpha = (l1->theta + l2->theta - M_PI) / 2;
                    ep->epsilon = fabs(l1->rho + l2->rho) / 2;
                }
                else
                {
                    ep->alpha = fabs(l1->theta + l2->theta) / 2;
                    ep->epsilon = fabs(l1->rho - l2->rho) / 2;
                }
                pairs[nb_pairs++] = ep;
            }
        }
    }

    *out_len = nb_pairs;
    return pairs;
}

unsigned int LongestCut(const unsigned int* pixels, int w, int h,
        int x0, int y0, int x1, int y1)
{
    size_t max_cut = 0;
    size_t cut = 0;
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int error = dx + dy;

    while (1)
    {
        if (y0 >= 0 && y0 < h && x0 >= 0 && x0 < w)
        {
            if (pixels[y0 * w + x0] > 0)
            {
                if (cut > max_cut) max_cut = cut;
                cut = 0;
            } else {
                cut += 1;
            }
        }
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
    if (cut > max_cut) max_cut = cut;
    return max_cut;
}

unsigned int RectOccupency(const Image* img, Rect* r)
{
    const size_t w = img->width;
    const size_t h = img->height;
    const unsigned int* p = img->pixels;

    const Line* l1 = r->ep1->l1;
    const Line* l2 = r->ep1->l2;
    const Line* l3 = r->ep2->l1;
    const Line* l4 = r->ep2->l2;
    int x1, y1, x2, y2, x3, y3, x4, y4;

    LineIntersection(l1, l3, &x1, &y1);
    LineIntersection(l2, l3, &x2, &y2);
    LineIntersection(l1, l4, &x3, &y3);
    LineIntersection(l2, l4, &x4, &y4);

   return LongestCut(p, w, h, x1, y1, x2, y2) + 
       LongestCut(p, w, h, x1, y1, x3, y3) +
       LongestCut(p, w, h, x4, y4, x2, y2) +
       LongestCut(p, w, h, x4, y4, x2, y2);
}

Rect** FindRects(const Image* img, PSet** pairs, size_t nb_pairs,
        size_t* found_count)
{
    float a90 = M_PI / 2; // 90°
    float Ta = 8 * M_PI / 180; // max angle diff between two orhogonal lines

    float Dmin = 10 * 81; // 10pix per cell minimum
    float Ts = 1 - 0.075; // sides length diff
    size_t alloc_size = nb_pairs * nb_pairs;
    size_t rect_count = 0;
    Rect** rects = malloc(sizeof(Rect*) * alloc_size);

    for(size_t i = 0; i < nb_pairs; i++)
    {
        PSet* ep1 = pairs[i];
        float a = 2 * ep1->epsilon;
        for(size_t j = 0; j < nb_pairs; j++)
        {
            if (i == j) continue;
            PSet* ep2 = pairs[j];
            float b = 2 * ep2->epsilon;

            float dA = fabs(fabs(ep1->alpha - ep2->alpha) - a90);
            float squareness = a > b ? b / a : a / b;

            if (dA < Ta && squareness > Ts && a * b >= Dmin)
            {
                Rect* rect = malloc(sizeof(Rect));
                rect->ep1 = ep1;
                rect->ep2 = ep2;
                rect->area = round(a * b);
                rect->squareness = squareness;
                rect->occ = RectOccupency(img, rect);

                rects[rect_count++] = rect;
                if (rect_count == alloc_size)
                {
                    size_t size = alloc_size + alloc_size/2;
                    rects = realloc(rects, sizeof(Rect*) * size);
                    alloc_size = size;
                }
            }
        }
    }

    *found_count = rect_count;
    return rects;
}

Rect** GetBestRects(Rect** rects, size_t len, size_t keep)
{
    Rect** top = calloc(keep, sizeof(Rect*));
    for(size_t i = 0; i < len; i++)
    {
        Rect* cand = rects[i];
        size_t j = 0;
        while(j < keep && top[j] != NULL && top[j]->area > cand->area)
            j++;
        if (j < keep)
        {
            if (top[j] != NULL && top[j]->area == cand->area)
                continue;
            for(size_t k = keep - 1; k > j; k--)
                top[k] = top[k - 1];
            top[j] = cand;
        }
    }
    return top;
}

static inline float CalcScore(Rect* r)
{
    return pow(r->squareness, 3) * sqrt(r->area) * (1 - r->occ / r->area);
}

Rect* FindSudokuBoard(Rect** rects, size_t rect_count)
{
    if (rect_count == 0) return NULL;

    Rect* max = rects[0];
    float m_score = CalcScore(max);
    for (size_t i = 1; i < rect_count; i++)
    {
        Rect* rect = rects[i];
        if (rect == NULL) continue;
        float score = CalcScore(rect);
        if (score > m_score)
        {
            m_score = score;
            max = rect;
        }
    }
    return max;
}
