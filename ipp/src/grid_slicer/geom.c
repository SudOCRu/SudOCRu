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

int min4(int a, int b, int c, int d) {
    int m1 = a < b ? a : b;
    int m2 = c < d ? c : d;
    return m1 < m2 ? m1 : m2;
}

int max4(int a, int b, int c, int d) {
    int m1 = a > b ? a : b;
    int m2 = c > d ? c : d;
    return m1 > m2 ? m1 : m2;
}

BBox* NewBB(Rect* r)
{
    const Line* l1 = r->ep1->l1;
    const Line* l2 = r->ep1->l2;
    const Line* l3 = r->ep2->l1;
    const Line* l4 = r->ep2->l2;

    int p1_x = 0, p1_y = 0;
    LineIntersection(l1, l3, &p1_x, &p1_y);

    int p2_x = 0, p2_y = 0;
    LineIntersection(l2, l3, &p2_x, &p2_y);

    int p3_x = 0, p3_y = 0;
    LineIntersection(l1, l4, &p3_x, &p3_y);

    int p4_x = 0, p4_y = 0;
    LineIntersection(l2, l4, &p4_x, &p4_y);

    BBox* bb = malloc(sizeof(BBox*));
    bb->x1 = min4(p1_x, p2_x, p3_x, p4_x);
    bb->y1 = min4(p1_y, p2_y, p3_y, p4_y);
    bb->x2 = max4(p1_x, p2_x, p3_x, p4_x);
    bb->y2 = max4(p1_y, p2_y, p3_y, p4_y);
    return bb;
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
