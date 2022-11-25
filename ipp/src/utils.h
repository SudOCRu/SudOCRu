#pragma once
#include <stdio.h>

#define SC_NO_FLG     0
#define SC_FLG_ALINES 0x001 // All lines
#define SC_FLG_FLINES 0x002 // Filtered lines
#define SC_FLG_ARECTS 0x004 // All rects
#define SC_FLG_FRECTS 0x008 // Filtered rects
#define SC_FLG_PRESTL 0x010 // Print result table

#define SC_FLG_DGRS   0x020 // Save grayscale.png debug image
#define SC_FLG_DMED   0x040 // Save median.png debug image
#define SC_FLG_DFIL   0x080 // Save filtered.png debug image
#define SC_FLG_DGRD   0x100 // Save detected.png debug image
#define SC_FLG_DEDG   0x200 // Save edges.png debug image
#define SC_FLG_DROT   0x400 // Save rotated.png debug image
#define SC_FLG_DACC   0x800 // Save acc.png debug image

#define SC_FLG_IPP ((SC_FLG_DFIL) | (SC_FLG_DEDG) | (SC_FLG_FIL))
#define SC_FLG_FIL ((SC_FLG_DGRD) | (SC_FLG_FLINES) | (SC_FLG_FRECTS) | (SC_FLG_PRESTL))

void PrintStage(unsigned char id, unsigned char total, char* stage, int ok);
