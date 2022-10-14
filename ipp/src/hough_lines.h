#pragma once
#include "image.h"

#define MAX_LINES 512
#define THETA_STEPS 360
#define BLACK_EDGE 0
#define WHITE_EDGE 1

typedef struct Point {
    unsigned int x;
    unsigned int y;
} Point;

typedef struct Line {
    double rho;
    double theta;
    double x1;
    double y1;
    double x2;
    double y2;
} Line;

typedef struct Rect {
    Point p1;
    Point p2;
    Point p3;
    Point p4;
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

void RenderLines(Image* img, unsigned int color, const Line** lines, int len);

Rect** FindRects(Image* img, const Line** lines, size_t len, int* found_count);
void DetectGrid(const Image* image, SudokuGrid* grid);
double DetectAngle(const Rect* rect);
