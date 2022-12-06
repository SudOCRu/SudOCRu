#include "geom.h"

int LineIntersection(const Line* l1, const Line* l2, int *x, int *y) {
    float a = cosf(l1->theta);
    float b = sinf(l1->theta);
    float c = cosf(l2->theta);
    float d = sinf(l2->theta);
    float dt = a * d - b * c; //determinative (rearranged matrix for inverse)
    if(fabs(dt) > GEOM_INTS_ERROR) { // account for error, dt != 0
        if (x != NULL) *x = (int)((d * l1->rho - b * l2->rho) / dt);
        if (y != NULL) *y = (int)((-c * l1->rho + a * l2->rho) / dt);
        return 1;
    }
    return 0;
}

static inline int min4(int a, int b, int c, int d) {
    int m1 = a < b ? a : b;
    int m2 = c < d ? c : d;
    return m1 < m2 ? m1 : m2;
}

static inline int max4(int a, int b, int c, int d) {
    int m1 = a > b ? a : b;
    int m2 = c > d ? c : d;
    return m1 > m2 ? m1 : m2;
}

BBox* NewBB(Rect* r)
{
    BBox* bb = malloc(sizeof(BBox));
    if (bb == NULL)
        return NULL;

    const Line* l1 = r->ep1->l1;
    const Line* l2 = r->ep1->l2;
    const Line* l3 = r->ep2->l1;
    const Line* l4 = r->ep2->l2;

    /*
     (x1, y1) --- [l1] --- (x3, y3)
        |                     |
        |                     |
       [l3]                 [l4]
        |                     |
        |                     |
     (x2, y2) --- [l2] --- (x4, y4)
     */

    LineIntersection(l1, l3, &bb->x1, &bb->y1);
    LineIntersection(l2, l3, &bb->x2, &bb->y2);
    LineIntersection(l1, l4, &bb->x3, &bb->y3);
    LineIntersection(l2, l4, &bb->x4, &bb->y4);

    return bb;
}

void GetCenterBB(BBox* bb, float* centerX, float* centerY)
{
    *centerX = (bb->x1 + bb->x2 + bb->x3 + bb->x4) / 4.0f;
    *centerY = (bb->y1 + bb->y2 + bb->y3 + bb->y4) / 4.0f;
}

static inline void RotatePoint(int* x, int* y, float cx, float cy, float cost,
        float sint)
{
    float nx = cost * ((float)*x - cx) - sint * ((float)*y - cy) + cx;
    float ny = sint * ((float)*x - cx) + cost * ((float)*y - cy) + cy;
    *x = nx;
    *y = ny;
}

void RotateBB(BBox* bb, float angle, float centerX, float centerY)
{
    if (fabs(angle) < (M_PI/180)) return; // Less than 1°
    float cost = cos(angle), sint = sin(angle);
    RotatePoint(&bb->x1, &bb->y1, centerX, centerY, cost, sint);
    RotatePoint(&bb->x2, &bb->y2, centerX, centerY, cost, sint);
    RotatePoint(&bb->x3, &bb->y3, centerX, centerY, cost, sint);
    RotatePoint(&bb->x4, &bb->y4, centerX, centerY, cost, sint);
}

void GetRectFromBB(BBox* bb, int* l, int* t, int* r, int* b)
{
    *l = min4(bb->x1, bb->x2, bb->x3, bb->x4);
    *t = min4(bb->y1, bb->y2, bb->y3, bb->y4);
    *r = max4(bb->x1, bb->x2, bb->x3, bb->x4);
    *b = max4(bb->y1, bb->y2, bb->y3, bb->y4);
}

void SortBB(BBox* bb)
{
    if (bb == NULL)
        return;

    int* points = (int*) bb;

    int sum_minX = points[0], sum_minY = points[1],
        sum_maxX = points[0], sum_maxY = points[1];

    int sub_minX = points[0], sub_minY = points[1],
        sub_maxX = points[0], sub_maxY = points[1];
    for (size_t i = 2; i < 8; i += 2)
    {
        int x = points[i];
        int y = points[i + 1];
        if (x + y > sum_maxX + sum_maxY)
        {
            sum_maxX = x;
            sum_maxY = y;
        }
        if (x + y < sum_minX + sum_minY)
        {
            sum_minX = x;
            sum_minY = y;
        }
        if (x - y > sub_maxX - sub_maxY)
        {
            sub_maxX = x;
            sub_maxY = y;
        }
        if (x - y < sub_minX - sub_minY)
        {
            sub_minX = x;
            sub_minY = y;
        }
    }

    /*
     The points are sorted like this:
     (x1, y1) ------------ (x2, y2)
        |                     |
        |                     |
        |                     |
        |                     |
        |                     |
     (x4, y4) ------------ (x3, y3)
    */

    bb->x1 = sum_minX;
    bb->y1 = sum_minY;
    bb->x3 = sum_maxX;
    bb->y3 = sum_maxY;

    bb->x2 = sub_minX;
    bb->y2 = sub_minY;
    bb->x4 = sub_maxX;
    bb->y4 = sub_maxY;
}

void FreeBB(BBox* bb)
{
    free(bb);
}

void FreeLine(Line* line)
{
    free(line);
}

void FreeLines(Line** lines, size_t len)
{
    if (lines == NULL) return;
    for(size_t i = 0; i < len; i++)
    {
        FreeLine(lines[i]);
    }
    free(lines);
}

void FreePSet(PSet* pset)
{
    free(pset);
}

void FreePSets(PSet** psets, size_t len)
{
    if (psets == NULL) return;
    for(size_t i = 0; i < len; i++)
    {
        FreePSet(psets[i]);
    }
    free(psets);
}

void FreeRect(Rect* rect)
{
    free(rect);
}

void FreeRects(Rect** rects, size_t len)
{
    if (rects == NULL) return;
    for(size_t i = 0; i < len; i++)
    {
        FreeRect(rects[i]);
    }
    free(rects);
}
