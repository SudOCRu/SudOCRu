#pragma once
#include "image.h"
#define GEOM_INTS_ERROR 0.01

/* Line is a represetnation of a local maximum in the hough space, it is defined
 * by its coordinates (rho, theta) in polar coordinates and by ((x1, y1), (x2,
 * y2)) in cartesian coordinates and by its total number of votes `val`.
 */
typedef struct Line {
    int val; // number of votes at position (rho, theta) in the hough image
    float rho;
    float theta;

    // Cartesian coordinates of the line's representation
    float x1;
    float y1;
    float x2;
    float y2;
} Line;

/*
 *  PSet or ParallelSet stores information about a pair of parallel lines
 *  generated by the analysis of the hough space parameters. Alpha is the
 *  average rho of the two lines and epsilon is the distance betweens these
 *  lines.
*/
typedef struct PSet {
    const Line* l1;
    const Line* l2;
    float alpha;
    float epsilon;
} PSet;

/*
 * Rect or Rectangle is a rectangle formed by two pairs of parallel lines
 * seperated by a 90° angle in hough space (PI). Squareness is a unitless metric
 * to compare rects to squares by dividing the length of the longest side by its
 * shortest.
*/
typedef struct Rect {
    PSet* ep1;
    PSet* ep2;
    unsigned int area;
    float squareness;
} Rect;

/*
 * BBox or Bounding Box stores the intersection points of the lines that form
 * the corresponding quad.
*/
typedef struct BBox {
    int x1;
    int y1;
    int x2;
    int y2;
    int x3;
    int y3;
    int x4;
    int y4;
} BBox;

/*
 * BBox* NewBB(Rect* rectangle) -> BBox*
 *
 * Returns the bounding box of the corresponding rectangle by calculating the
 * intersections of the its four lines. Note that the input rectangle must not
 * be NULL.
*/
BBox* NewBB(Rect* r);

void RotateBB(BBox* bb, float angle, float centerX, float centerY);
void GetCenterBB(BBox* bb, float* centerX, float* centerY);
void GetRectFromBB(BBox* bb, size_t* l, size_t* t, size_t* r, size_t* b);

/*
 * LineIntersection(const Line* l1, const Line* l2, float *x, float *y) -> bool
 *
 * Returns true if the two lines intersects, x and y and updated according to
 * the coordinates of the intersection. Returns false otherwise.
 *
 * Note that it is safe to pass NULl to x and/or y.
*/
int LineIntersection(const Line* l1, const Line* l2, int *x, int *y);

/* ---------- MEMORY OPERATIONS ---------- */

/*
 * FreeLine(Line* line) -> void
 * Frees the memory allocated by the Line struct. Note that is is safe to pass
 * NULL to this function.
 *
 * The `line` pointer must not be used after calling this function.
*/
void FreeLine(Line* line);

/*
 * FreeLines(Line** lines, size_t len) -> void
 * Frees the memory allocated by `len` line in the `lines` array using the
 * FreeLine function  and frees the array pointer. Note that is is safe to pass
 * NULL to this function.
 *
 * The `lines` pointer must not be used after calling this function.
*/
void FreeLines(Line** lines, size_t len);

/*
 * FreePSet(PSet* pset) -> void
 * Frees the memory allocated by the ParallelSet struct. This does not include
 * the inner line pointers, these pointers must also be freed by FreeLine
 * afterwards. Note that is is safe to pass NULL to this function.
 *
 * The `pset` pointer must not be used after calling this function.
*/
void FreePSet(PSet* pset);

/*
 * FreePSets(ParallelSet** psets, size_t len) -> void
 * Frees the memory allocated by `len` ParallelSets in the `psets` array using
 * the FreeParallelSet function and frees the array pointer. This does not
 * include the inner line pointers, these pointers must also be freed by
 * FreeLines afterwards. Note that is is safe to pass NULL to this function.
 *
 * The `psets` pointer must not be used after calling this function.
*/
void FreePSets(PSet** psets, size_t len);

/*
 * FreeRect(Rect pset) -> void
 * Frees the memory allocated by the Rect struct. This does not include
 * the inner parallel sets pointers nether line pointers, these pointers must
 * also be freed by FreeParallelSet and FreeLine afterwards. Note that is is
 * safe to pass NULL to this function.
 *
 * The `rect` pointer must not be used after calling this function.
*/
void FreeRect(Rect* rect);

/*
 * FreeRects(Rect** rects, size_t len) -> void
 * Frees the memory allocated by `len` Rects in the `rects` array using
 * the FreeRect function and frees the array pointer. This does not include
 * the inner parallel sets pointers nether line pointers, these pointers must
 * also be freed by FreeParallelSet and FreeLine afterwards. Note that is is
 * safe to pass NULL to this function.
 *
 * The `rects` pointer must not be used after calling this function.
*/
void FreeRects(Rect** rects, size_t len);
