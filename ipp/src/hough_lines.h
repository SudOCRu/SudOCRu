#pragma once
#include "image.h"

#define MAX_LINES 4096
#define MAX_RECTS 256

typedef struct Point {
    unsigned int x;
    unsigned int y;
} Point;

typedef struct Line {
    Point p1;
    Point p2;
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
} Board;

Line** HoughLines(const Image* image, int* found_count);
Rect** FindRects(const Image* image, int* found_count);
void RenderLines(Image* image, unsigned int color, Line** lines, int len);

void DetectGrid(const Image* image, SudokuGrid* grid);
double DetectAngle(const Rect* rect);
