#include "homography.h"

Matrix* GetHomographyMatrix(const BBox* from, const BBox* to)
{
    float a_vals[8 * 8] =
    {
        from->x1, from->y1, 1, 0, 0, 0, -to->x1 * from->x1, -to->x1*from->y1,
        0, 0, 0, from->x1, from->y1, 1, -to->y1 * from->x1, -to->y1*from->y1,

        from->x2, from->y2, 1, 0, 0, 0, -to->x2 * from->x2, -to->x2*from->y2,
        0, 0, 0, from->x2, from->y2, 1, -to->y2 * from->x2, -to->y2*from->y2,

        from->x3, from->y3, 1, 0, 0, 0, -to->x3 * from->x3, -to->x3*from->y3,
        0, 0, 0, from->x3, from->y3, 1, -to->y3 * from->x3, -to->y3*from->y3,

        from->x4, from->y4, 1, 0, 0, 0, -to->x4 * from->x4, -to->x4*from->y4,
        0, 0, 0, from->x4, from->y4, 1, -to->y4 * from->x4, -to->y4*from->y4,
    };
    float b_vals[8] = {
        to->x1, to->y1,
        to->x2, to->y2,
        to->x3, to->y3,
        to->x4, to->y4,
    };
    Matrix* a = NewMatrix(8, 8, a_vals);
    Matrix* b = NewMatrix(8, 1, b_vals);
    Matrix* at = MatTranspose(a);

    // We need to compute H = (at*a)^-1 * at * b
    //                <=> H = iv^-1 * atb

    Matrix* atb = MatMultiply(at, b);
    Matrix* iv = MatMultiply(at, a);
    if (!MatInvert(iv))
        return NULL;

    Matrix* h = MatMultiply(iv, atb);

    DestroyMatrix(iv);
    DestroyMatrix(atb);
    DestroyMatrix(at);
    DestroyMatrix(b);
    DestroyMatrix(a);
    return h;
}
