#pragma once
#include "image.h"

#define MAX_LINES 256

typedef struct Point {
    unsigned int x;
    unsigned int y;
} Point;

typedef struct Line {
    double rho;
    double theta;
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

Line** HoughLines(const Image* image, size_t* found_count);
void FreeLines(Line** lines, size_t len);
Rect** FindRects(const Image* image, int* found_count);
void RenderLines(Image* image, unsigned int color, Line** lines, int len);

void DetectGrid(const Image* image, SudokuGrid* grid);
double DetectAngle(const Rect* rect);
