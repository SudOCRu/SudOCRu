#pragma once
#include "image.h"

#define MAX_LINES 512
#define THETA_STEPS 180
#define BLACK_EDGE 0
#define WHITE_EDGE 1

typedef struct Point {
    unsigned int x;
    unsigned int y;
} Point;

typedef struct Line {
    double rho;
    double theta;
    double a;
    double b;
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

Line** HoughLines(const Image* img, size_t* found_count, int white_edge,
        size_t theta_steps, int threshold);
void FreeLines(Line** lines, size_t len);
Rect** FindRects(const Image* image, int* found_count);
void RenderLines(Image* image, unsigned int color, Line** lines, int len);

void DetectGrid(const Image* image, SudokuGrid* grid);
double DetectAngle(const Rect* rect);
