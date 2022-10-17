#include "hough_lines.h"
#include <math.h>
#define DEBUG_VIEW

Line* LineFrom(unsigned int val, float theta, float rho, float x1,
        float y1, float x2, float y2)
{
    Line* l = (Line*) malloc(sizeof(Line));
    l->val = (int)val;
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
    size_t w = img->width, h = img->height;
    int hsp_height = (int)ceil(sqrt(w*w + h*h))*2;
    float dtheta = (4*M_PI)/(3*hsp_height/2);
    int hsp_width = M_PI / dtheta; // M_PI / dtheta

    // Pre-generate sin, cos, 1/sin tables for faster computation.
    float sin_t[hsp_width];
    float cos_t[hsp_width];
    size_t col = 0;
    float max_angle = M_PI;
    for(float th = 0; col < hsp_width; col++, th += dtheta)
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
                    float p = hsp_height/2 + x * cos_t[col] + y * sin_t[col];
                    size_t idx = hsp_width * (size_t)round(p) + col;
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
                float p = (float)y - hsp_height / 2;
                float x0 = cos_t[x] * p;
                float y0 = sin_t[x] * p;
                float a = -1000*sin_t[x];
                float b = 1000*cos_t[x];
                out_lines[lines++] = LineFrom(acc[i], x * dtheta, p,
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
    float deltaT = 3 * M_PI / 180; // 3° tolerance
    float deltaR = 3;
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
    float a = cosf(l1->theta);
    float b = sinf(l1->theta);
    float c = cosf(l2->theta);
    float d = sinf(l2->theta);
    float dt = a * d - b * c; //determinative (rearranged matrix for inverse)
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

Rect** FindRects(Image* img, Line** lines, size_t len, size_t* found_count)
{
    float a90 = M_PI / 2; // 90°
    float Ta = 8 * M_PI / 180; // max angle diff between two orhogonal lines
    float Tt = 3 * M_PI / 180; // max angle diff between two lines
    float Tp = 100; // Rho diff
    float Tl = 0.4; // threshold for line segements to have similar lengths
    float Dmin = 10 * 100;
    float Ts = 0; // sides length diff

    ExtPeak** pairs = malloc(sizeof(ExtPeak*) * len * len);
    size_t nb_pairs = 0;

    for (size_t i = 0; i < len; i++)
    {
        const Line* l1 = lines[i];
        for (size_t j = 0; j < len; j++)
        {
            if (i == j) continue;
            const Line* l2 = lines[j];
            float dT = fabs(l1->theta - l2->theta);
            int dC = abs(l1->val - l2->val);
            int mid = (l1->val + l2->val) / 2;

            if (dT < Tt && dC < Tl * mid)
            {
                ExtPeak* ep = malloc(sizeof(ExtPeak));
                ep->l1 = l1;
                ep->l2 = l2;
                ep->alpha = (l1->theta + l2->theta) / 2;
                ep->epsilon = fabs(l1->rho - l2->rho) / 2;
                pairs[nb_pairs++] = ep;
            }
        }
    }
    printf("nb_pairs = %lu\n", nb_pairs);

    //RenderPairs(img, pairs, nb_pairs);

    size_t alloc_size = nb_pairs * 2;
    if (nb_pairs < len)
    {
        alloc_size = len;
    }
    Rect** rects = malloc(sizeof(Rect*) * alloc_size);
    size_t rect_count = 0;

    for(size_t i = 0; i < nb_pairs; i++)
    {
        ExtPeak* ep1 = pairs[i];
        float a = 2 * ep1->epsilon;
        for(size_t j = 0; j < nb_pairs; j++)
        {
            if (i == j) continue;
            ExtPeak* ep2 = pairs[j];
            float b = 2 * ep2->epsilon;

            float dA = fabs(fabs(ep1->alpha - ep2->alpha) - a90);
            float dP = fabs((ep1->l1->rho + ep1->l2->rho / 2) - 
                    (ep2->l1->rho + ep2->l2->rho / 2));
            float squareness = a > b ? b / a : a / b;
            if (dA < Ta)
            {
                printf("dP = %f, squareness = %f\n", dP, squareness);
                if (dP < Tp && squareness > Ts &&
                        a * b >= Dmin)
                {
                    Rect* rect = malloc(sizeof(Rect));
                    rect->ep1 = ep1;
                    rect->ep2 = ep2;
                    rect->area = a * b;
                    rect->squareness = squareness;
                    rects[rect_count++] = rect;
                    if (rect_count == nb_pairs)
                    {
                        *found_count = rect_count;
                        return rects;
                    }
                }
            }
        }
    }

    printf("rect_count = %lu\n", rect_count);

    *found_count = rect_count;
    return rects;
}

Rect* FindSudokuBoard(Rect** rects, size_t rect_count)
{
    if (rect_count == 0) return NULL;

    Rect* max = rects[0];
    float m_score = max->squareness * max->area;
    for (size_t i = 1; i < rect_count; i++)
    {
        Rect* rect = rects[i];
        float score = rect->squareness * rect->area;
        if (score > m_score)
        {
            m_score = score;
            max = rect;
        }
    }
    return max;
}

void FreeRects(Rect** rects, size_t len)
{
    if (rects == NULL) return;
    for(size_t i = 0; i < len; i++)
    {
        free(rects[i]);
    }
    free(rects);
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

void RenderLines(Image* img, unsigned int color, Line** lines, size_t l)
{
    size_t w = img->width;
    size_t h = img->height;
    unsigned int* pix = img->pixels;
    for(size_t i = 0; i < l; i++)
    {
        const Line* l = lines[i];
        DrawLine(pix, color, w, h, (int)l->x1, (int)l->y1, (int)l->x2,
                (int)l->y2);
    }
}

void RenderPairs(Image* img, ExtPeak** pairs, size_t l)
{
    size_t w = img->width;
    size_t h = img->height;
    unsigned int* pix = img->pixels;
    unsigned int color;
    for(size_t i = 0; i < l; i++)
    {
        const ExtPeak* ep = pairs[i];

        const Line* l1 = ep->l1;
        const Line* l2 = ep->l2;
        color = (rand() % 255) << 16 | (rand() % 255) << 8 | (rand() % 255);

        DrawLine(pix, color, w, h, (int)l1->x1, (int)l1->y1, (int)l1->x2,
                (int)l1->y2);
        DrawLine(pix, color, w, h, (int)l2->x1, (int)l2->y1, (int)l2->x2,
                (int)l2->y2);
    }
}

void RenderRect(Image* img, unsigned int color, Rect* r)
{
    size_t w = img->width;
    size_t h = img->height;
    unsigned int* pix = img->pixels;

    const Line* l1 = r->ep1->l1;
    const Line* l2 = r->ep1->l2;
    const Line* l3 = r->ep2->l1;
    const Line* l4 = r->ep2->l2;

    int p1_x = 0;
    int p1_y = 0;
    LineIntersection(l1, l3, &p1_x, &p1_y);

    int p2_x = 0;
    int p2_y = 0;
    LineIntersection(l2, l3, &p2_x, &p2_y);

    int p3_x = 0;
    int p3_y = 0;
    LineIntersection(l1, l4, &p3_x, &p3_y);

    int p4_x = 0;
    int p4_y = 0;
    LineIntersection(l2, l4, &p4_x, &p4_y);

    DrawLine(pix, color, w, h, p1_x, p1_y, p2_x, p2_y);
    DrawLine(pix, color, w, h, p1_x, p1_y, p3_x, p3_y);
    DrawLine(pix, color, w, h, p2_x, p2_y, p4_x, p4_y);
    DrawLine(pix, color, w, h, p3_x, p3_y, p4_x, p4_y);
}

void RenderRects(Image* img, Rect** rects, size_t l)
{
    size_t w = img->width;
    size_t h = img->height;
    unsigned int* pix = img->pixels;
    unsigned int color;
    for(size_t i = 0; i < l; i++)
    {
        const Rect* r = rects[i];

        const Line* l1 = r->ep1->l1;
        const Line* l2 = r->ep1->l2;
        const Line* l3 = r->ep2->l1;
        const Line* l4 = r->ep2->l2;
        color = (rand() % 255) << 16 | (rand() % 255) << 8 | (rand() % 255);

        int p1_x = 0;
        int p1_y = 0;
        LineIntersection(l1, l3, &p1_x, &p1_y);

        int p2_x = 0;
        int p2_y = 0;
        LineIntersection(l2, l3, &p2_x, &p2_y);

        int p3_x = 0;
        int p3_y = 0;
        LineIntersection(l1, l4, &p3_x, &p3_y);

        int p4_x = 0;
        int p4_y = 0;
        LineIntersection(l2, l4, &p4_x, &p4_y);

        DrawLine(pix, color, w, h, p1_x, p1_y, p2_x, p2_y);
        DrawLine(pix, color, w, h, p1_x, p1_y, p3_x, p3_y);
        DrawLine(pix, color, w, h, p2_x, p2_y, p4_x, p4_y);
        DrawLine(pix, color, w, h, p3_x, p3_y, p4_x, p4_y);
    }
}
