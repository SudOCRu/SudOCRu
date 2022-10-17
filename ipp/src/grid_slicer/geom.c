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
