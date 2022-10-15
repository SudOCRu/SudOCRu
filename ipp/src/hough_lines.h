#pragma once
#include "image.h"

#define MAX_LINES 512
#define THETA_STEPS 360
#define BLACK_EDGE 0
#define WHITE_EDGE 1

typedef struct Line {
    int val; // peak value C(p_i, theta_i)
    double rho;
    double theta;

    double x1;
    double y1;
    double x2;
    double y2;
} Line;

typedef struct ExtPeak {
    const Line* l1;
    const Line* l2;
    double alpha;
    double epsilon;
} ExtPeak;

typedef struct Rect {
    ExtPeak* ep1;
    ExtPeak* ep2;
} Rect;

typedef struct SudokuGrid {
    Rect bounds;
    double angle;
} SudokuGrid;

void Dilate(Image* img);

Line** HoughLines(const Image* img, size_t* found_count, int white_edge,
        size_t theta_steps, int threshold);
void FreeLines(Line** lines, size_t len);

Line** AverageLines(Line** lines, size_t len, size_t* out_len);

void RenderLines(Image* image, unsigned int color, Line** lines, size_t len);
void RenderRects(Image* image, unsigned int color, Rect** rects, size_t len);
void RenderPairs(Image* img, unsigned int color, ExtPeak** pairs, size_t l);

Rect** FindRects(Image* image, Line** lines, size_t len, size_t* found_count);
void DetectGrid(const Image* image, SudokuGrid* grid);
double DetectAngle(const Rect* rect);
