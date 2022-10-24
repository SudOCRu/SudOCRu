#include "renderer.h"

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

void RenderLine(Image* img, unsigned int color, Line* l)
{
    DrawLine(img->pixels, color, img->width, img->height,
            (int)l->x1, (int)l->y1, (int)l->x2, (int)l->y2);
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

void RenderPSets(Image* img, PSet** psets, size_t l)
{
    size_t w = img->width;
    size_t h = img->height;
    unsigned int* pix = img->pixels;
    unsigned int color;
    for(size_t i = 0; i < l; i++)
    {
        const PSet* ep = psets[i];

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
