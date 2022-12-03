#include "transform.h"

void RotateImage(Image* img, float angle, float midX, float midY)
{
    if (fabs(angle) < (M_PI/180)) return; // Less than 1°
    size_t w = img->width, h = img->height;
    unsigned int* dst = calloc(w * h, sizeof(unsigned int));
    float sint = sin(-angle), cost = cos(-angle);

    for (size_t i = 0; i < h; i++){
        float ny = (float)i - midY;
        for (size_t j = 0; j < w; j++){
            float nx = (float)j - midX;
            float x = nx * cost - ny * sint + midX;
            float y = nx * sint + ny * cost + midY;
            if (x >= 0 && x < w && y >= 0 && y < h)
                dst[i * w + j] = img->pixels[(size_t)y * w + (size_t)x];
        }
    }

    memcpy(img->pixels, dst, w * h * sizeof(unsigned int));
    free(dst);
}

Image* CropImage(const Image* src, size_t l, size_t t, size_t r, size_t b)
{
    if (r <= l || b <= t || r >= src->width || b >= src->height)
    {
        printf("CropImage: out of bounds, l=%lu,t=%lu,r=%lu,b=%lu\n", l,t,r,b);
        return NULL;
    }

    size_t w = r - l, h = b - t;
    Image* dst = CreateImage(0, w, h, NULL);
    if (dst == NULL)
    {
        printf("CropImage: Not enough memory, width=%lu, height=%lu\n", w, h);
        return NULL;
    }

    for (size_t i = 0; i < h; i++)
    {
        for (size_t j = 0; j < w; j++)
        {
            dst->pixels[i * w + j] = src->pixels[(i + t) * src->width + j + l];
        }
    }

    return dst;
}

Image* CropRotateImage(const Image* src, float angle, float midX, float midY,
        int l, int t, int r, int b)
{
    if (fabs(angle) < (M_PI/180)) // Less than 1°
    {
        return CropImage(src, l, t, r, b);
    }

    if (r <= l || b <= t || r >= (int)src->width || b >= (int)src->height)
        return NULL;
    size_t nw = r - l, nh = b - t;

    Image* dst = CreateImage(0, nw, nh, NULL);
    if (dst == NULL)
        return NULL;

    size_t w = src->width, h = src->height;
    float sint = sin(-angle), cost = cos(-angle);

    for (size_t i = 0; i < nh; i++){
        float ny = (float)(i + t) - midY;
        for (size_t j = 0; j < nw; j++){
            float nx = (float)(j + l) - midX;
            float x = nx * cost - ny * sint + midX;
            float y = nx * sint + ny * cost + midY;
            if (x >= 0 && x < w && y >= 0 && y < h)
                dst->pixels[i * nw + j] = src->pixels[(size_t)y*w+(size_t)x];
            else
                dst->pixels[i * nw + j] = 0;
        }
    }

    return dst;
}

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

    // We need to compute h = (at*a)^-1 * at * b
    //                <=> h = iv^-1 * atb

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

// Calculate the distance squared between two points (x1, y1), (x2, y2)
static inline float DistanceSqrd(int x1, int y1, int x2, int y2)
{
    float dx = x1 - x2;
    float dy = y1 - y2;
    return dx * dx + dy * dy;
}

// Transforms a specific point stored in out_coords (the coordinates of the
// pixel in the output image to transform) to the new coordinates converted back
// from homogenous coordinates as (a, b). Note in_coords is used only to avoid
// allocating memory.
static inline int TransformPoint(const Matrix* h, const Matrix* out_coords,
        Matrix* in_coords, size_t* a, size_t* b)
{
    if(!MatMultiplyN(h, out_coords, in_coords))
        return 0;

    float norm = in_coords->m[2];
    *a = in_coords->m[0] / norm;
    *b = in_coords->m[1] / norm;
    return 1;
}

Image* WarpPerspective(const Image* img, BBox* from)
{
    // TODO: Sort bounding's box points to be always in the same order
    // Calculate the size of the effective square
    float ab = DistanceSqrd(from->x1, from->y1, from->x2, from->y2);
    float cb = DistanceSqrd(from->x2, from->y2, from->x4, from->y4);
    float ad = DistanceSqrd(from->x1, from->y1, from->x3, from->y3);
    float cd = DistanceSqrd(from->x3, from->y3, from->x4, from->y4);
    size_t l = sqrt(fmax(fmax(ab, cb), fmax(ad, cd)));
    printf("max length = %lu\n", l);

    // Find the transformation associated to transform the bounding box in a 
    // square of length l.
    BBox to = { 0, 0, 0, l, l, 0, l, l };
    Matrix* h = GetHomographyMatrix(from, &to);
    // Convert the homography matrix into a transformation matrix. The
    // conversion is in-place: the 8x1 matrix becomes a 3*3 matrix.
    h->m = realloc(h->m, 3 * 3 * sizeof(float));
    h->m[8] = 1;
    h->rows = h->cols = 3;

    // Calculate the inverse transformation
    if(!MatInvert(h))
        return NULL;

    // Prepare the matrix corresponding to the coordinates (x, y) in the output
    // image. Note: homogenous coordinates are used (last 1 in the matrix).
    float vals[] = { 0, 0, 1 };
    Matrix* out_coords = NewMatrix(3, 1, vals);
    Matrix* in_coords = NewMatrix(3, 1, vals);

    Image* out = CreateImage(0, l, l, NULL);
    if (out == NULL)
        return NULL;
    size_t a, b;

    // Apply the inverse transformation on the input image.
    for (size_t y = 0; y < out->height; y++)
    {
        out_coords->m[1] = y;
        for (size_t x = 0; x < out->width; x++)
        {
            out_coords->m[0] = x;

            // Calculate the homogenous coordinates (x', y') as (a, b) in
            // the input image
            if(!TransformPoint(h, out_coords, in_coords, &a, &b))
                continue;

            out->pixels[y * out->width + x] = img->pixels[b * img->width + a];
        }
    }

    DestroyMatrix(in_coords);
    DestroyMatrix(out_coords);
    DestroyMatrix(h);
    return out;
}