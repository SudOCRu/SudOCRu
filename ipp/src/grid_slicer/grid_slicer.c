#include "grid_slicer.h"
#include "hough_lines.h"
#include "renderer.h"
#include "../utils.h"
#include "../transform/transform.h"

Image* ExtractSudoku(Image* original, Image* img, int threshold, int flags)
{
    // Find all lines
    size_t len = 0;
    PrintStage(1, 4, "Hough Transform", 0);
    Line** lines = HoughLines(img, &len, WHITE_EDGE, THETA_STEPS, threshold,
            (flags & SC_FLG_DACC) != 0);
    if((flags & SC_FLG_ALINES) != 0)
        RenderLines(img, 0x0000FF, lines, len);
    PrintStage(1, 4, "Hough Transform", 1);

    // Merge similar lines
    PrintStage(2, 4, "Merge similar lines", 0);
    size_t fil_len = 0;
    Line** filtered = AverageLines(lines, len, &fil_len);
    printf(" --> %lu -> %lu", len, fil_len);
    PrintStage(2, 4, "Merge similar lines", 1);
    if((flags & SC_FLG_FLINES) != 0)
        RenderLines(img, 0xFF0000, filtered, fil_len);

    // Group parallel lines
    PrintStage(3, 4, "Group Parallel Lines", 0);
    size_t nb_psets = 0;
    PSet** psets = GroupParallelLines(filtered, fil_len, &nb_psets);
    printf(" --> %lu", nb_psets);
    PrintStage(3, 4, "Group Parallel Lines", 1);

    PrintStage(4, 4, "Find Rectangles", 0);
    size_t rect_count = 0;
    Rect** rects = FindRects(psets, nb_psets, &rect_count);
    printf(" --> Detected %lu rects", rect_count);
    PrintStage(4, 4, "Find Rectangles", 1);

    if (rect_count == 0)
        return NULL;

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
    int colors[5] = { 0x008800, 0xFFAA00, 0xAA00FF, 0xFF00AA, 0x00FFFF };
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

    Image* sudoku = NULL;
    Rect* candidate = FindSudokuBoard(best, 5);
    if (candidate != NULL)
    {
        float angle = fmod(candidate->ep1->alpha, M_PI/2);

        printf("=> Found matching Bounding Box:\n");
        printf("   > angle = %f°\n", angle * 180 / M_PI);
        printf("   > squareness = %f\n", candidate->squareness);
        printf("   > area = %u pix*pix\n", candidate->area);

        if ((flags & SC_FLG_PRESTL) != 0)
            RenderRect(img, 0x00ff00, candidate);

        BBox* bb = NewBB(candidate);
        printf("[(%i, %i), (%i, %i), (%i, %i), (%i, %i)]\n",
                bb->x1, bb->y1, bb->x2, bb->y2, bb->x3, bb->y3, bb->x4, bb->y4);
        sudoku = WarpPerspective(original, bb);

        if (sudoku == NULL)
        {
            printf("<!> Something went wrong when trying to warp perspective."
                    "Trying again with fallback method (auto-rotate).\n");

            float midX = 0, midY = 0;
            GetCenterBB(bb, &midX, &midY);
            RotateBB(bb, -angle, midX, midY);
            int l = 0, t = 0, r = original->width - 1, b = original->height - 1;
            GetRectFromBB(bb, &l, &t, &r, &b);
            printf("   > left=%i, top=%i, right=%i, bottom=%i\n", l, t, r, b);

            if((flags & SC_FLG_DROT) != 0)
            {
                Image* out = LoadBufImage(original->pixels, original->width,
                        original->height, NULL);
                RotateImage(out, -angle, midX, midY);
                if (SaveImageFile(out, "rotated.png"))
                    printf("Successfully wrote rotated.png\n");
                DestroyImage(out);
            }

            sudoku = CropRotateImage(original, -angle, midX, midY, l, t, r, b);
            if (sudoku == NULL)
                printf("<!> Something went wrong when cropping sudoku\n");
        }

        if (sudoku != NULL && SaveImageFile(sudoku, "sudoku.png"))
        {
            printf("Successfully wrote sudoku.png\n");
        }

        FreeBB(bb);
    }

    FreeRects(rects, rect_count);
    FreePSets(psets, nb_psets);
    FreeLines(lines, len);
    return sudoku;
}

Image** ExtractSudokuCells(Image* original, Image* img, size_t* out_count,
        int threshold, int flags)
{
    *out_count = 0;
    Image* sudoku = ExtractSudoku(original, img, threshold, flags);
    if (sudoku == NULL)
        return NULL;

    size_t vcells = 9, hcells = 9;
    Image** cells = malloc(vcells * hcells * sizeof(Image*));
    if (cells == NULL)
        return NULL;

    size_t stepX = sudoku->width / hcells;
    size_t stepY = sudoku->height / vcells;

    for (size_t i = 0; i < vcells; i++)
    {
        for (size_t j = 0; j < hcells; j++)
        {
            size_t r = (j + 1) * stepX;
            if (r >= sudoku->width) r = sudoku->width - 1;
            size_t b = (i + 1) * stepY;
            if (b >= sudoku->height) b = sudoku->height - 1;
            cells[i * hcells + j] =
                CropImage(sudoku, j * stepX, i * stepY, r, b);
        }
    }
    *out_count = vcells * hcells;

    return cells;
}
