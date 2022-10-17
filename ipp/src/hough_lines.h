#pragma once
#include "image.h"

#define MAX_LINES 4096
#define THETA_STEPS 480
#define BLACK_EDGE 0
#define WHITE_EDGE 1

typedef struct Line {
    int val; // peak value C(p_i, theta_i)
    float rho;
    float theta;

    float x1;
    float y1;
    float x2;
    float y2;
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
    double squareness;
    double area;
} Rect;

typedef struct SudokuGrid {
    Rect bounds;
    double angle;
} SudokuGrid;

void Dilate(Image* img);

Line** HoughLines(const Image* img, size_t* found_count, int white_edge,
        size_t theta_steps, int threshold);
void FreeRects(Rect** rects, size_t len);
void FreeLines(Line** lines, size_t len);

Line** AverageLines(Line** lines, size_t len, size_t* out_len);

void RenderLines(Image* image, unsigned int color, Line** lines, size_t len);
void RenderRect(Image* image, unsigned int color, Rect* rect);
void RenderRects(Image* image, Rect** rects, size_t len);
void RenderPairs(Image* img, ExtPeak** pairs, size_t l);

Rect** FindRects(Image* image, Line** lines, size_t len, size_t* found_count);
Rect* FindSudokuBoard(Rect** rects, size_t rect_count);
