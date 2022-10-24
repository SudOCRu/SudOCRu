#include "grid_slicer.h"
#include "hough_lines.h"
#include "renderer.h"

Image** ExtractSudokuCells(Image* img, size_t* out_count, int threshold,
        int flags)
{
    // Find all lines
    size_t len = 0;
    printf("Running edge detection (HT)...\n");
    Line** lines = HoughLines(img, &len, WHITE_EDGE, THETA_STEPS, threshold);
    if((flags & SC_FLG_ALINES) != 0)
        RenderLines(img, 0x0000FF, lines, len);

    // Merge similar lines
    size_t fil_len = 0;
    Line** filtered = AverageLines(lines, len, &fil_len);
    printf("Filtered lines, %lu -> %lu\n", len, fil_len);
    if((flags & SC_FLG_FLINES) != 0)
        RenderLines(img, 0xFF0000, filtered, fil_len);

    // Group parallel lines
    size_t nb_psets = 0;
    PSet** psets = GroupParallelLines(filtered, fil_len, &nb_psets);

    size_t rect_count = 0;
    Rect** rects = FindRects(img, psets, nb_psets, &rect_count);
    printf("Detected %lu rects\n", rect_count);
    if((flags & SC_FLG_ARECTS) != 0)
        RenderRects(img, rects, rect_count);
    Rect** best = GetBestRects(rects, rect_count, 5);

    if ((flags & SC_FLG_PRESTL) != 0)
    {
        printf("Results:\n");
        printf("-------------------------------------------------\n");
        printf("|  i |  color |     area | squareness |   angle |\n");
        printf("-------------------------------------------------\n");
    }
    int colors[5] = { 0x00FF00, 0xFFAA00, 0xAA00FF, 0xFF00AA, 0x00FFFF };
    char* names[5] = { "green", "orange", "purple", "pink", "cyan" };
    for(size_t i = 5; i > 0; i--)
    {
        Rect* r = best[i - 1];
        if (r == NULL) continue;
        if ((flags & SC_FLG_PRESTL) != 0)
        {
            printf("| %lu. | %6s | %8u | %10f | %3.2f° |\n", i, names[i - 1],
                    r->area, r->squareness, (r->ep1->alpha) * 180 / M_PI);
            printf("-------------------------------------------------\n");
        }
        if((flags & SC_FLG_FRECTS) != 0)
            RenderRect(img, colors[i - 1], r);
    }

    Rect* candidate = FindSudokuBoard(img, best, 5);
    if (candidate != NULL)
    {
        printf("=> Found rect:\n");
        printf("   > angle = %f°\n", (candidate->ep1->alpha) * 180 / M_PI);
        printf("   > squareness = %f\n", candidate->squareness);
        printf("   > area = %u pix*pix\n", candidate->area);
        RenderRect(img, 0x00FF00, candidate);
    }

    FreeRects(rects, rect_count);
    FreePSets(psets, nb_psets);
    FreeLines(lines, len);
    *out_count = 0;
    return NULL;
}