#pragma once
#include "geom.h"

/*
 * RenderLine(Image* image, unsigned int color, Line* line) -> void
 *
 * Renders the line line in the specified `color` using the Bresenham's
 * drawing line algorithm. Any part of the line outside of the image's bounds,
 * is omitted.
 *
 * The color field is a bit field: RRGGBB where RR, GG and BB are the byte
 * values for the brightness of each component. Alpha is not supported.
*/
void RenderLine(Image* image, unsigned int color, Line* line);

/*
 * RenderLines(Image* image, unsigned int color, Line** lines, size_t len);
 *
 * Renders `len` lines in the specified `color` from the array `lines`. Each
 * line is rendered using the RenderLine function, please check its
 * documentation.
 *
 * The color field is a bit field: RRGGBB where RR, GG and BB are the byte
 * values for the brightness of each component. Alpha is not supported.
*/
void RenderLines(Image* image, unsigned int color, Line** lines, size_t len);

/*
 * RenderPairs(Image* img, Pset** psets, size_t len) -> void
 *
 * Renders `len` parallel sets in random colors (each set is rendered in the
 * same color) from the array `pairs`. Each line of the extended peaks are
 * rendered using the RenderLine function, please check its documentation.
*/
void RenderPSets(Image* img, PSet** pairs, size_t len);

/*
 * RenderRect(Image* image, unsigned int color, Rect* rect) -> void
 *
 * Renders the rectangle by renderings its individual segments. The segments of
 * the rectangle are calculated using the coordinates of the intersections of
 * its corresponding lines in the specified `color`. Each segment of the
 * rectangle is rendered using the RenderLine function, please check its
 * documentation.
 *
 * The color field is a bit field: RRGGBB where RR, GG and BB are the byte
 * values for the brightness of each component. Alpha is not supported.
*/
void RenderRect(Image* image, unsigned int color, Rect* rect);

/*
 * RenderRects(Image* image, Rect** rects, size_t len) -> void
 *
 * Renders `len` rectangles in distinct colors from the array `rects`. Each
 * rectangle is rendered using the RenderRect function, please check its
 * documentation.
*/
void RenderRects(Image* image, Rect** rects, size_t len);
