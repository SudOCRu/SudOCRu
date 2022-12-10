#include "windows.h"
#include "../utils.h"
#include <filtering/image_filter.h>

struct OCRTask {
    SudOCRu* app;
    int result;
};

gboolean DoneOCR(gpointer user_data)
{
    struct OCRTask* task = user_data;
    SudOCRu* app = task->app;
    HideWindow(app, "OCRPopup");
    if (!task->result)
    {
        HideWindow(app, "ResizingWindow");
        ShowOCRResults(app);
    } else {
        char code[4];
        snprintf(code, sizeof(code), "E%02i", task->result);
        ShowErrorMessage(app, (const char*)&code, "Unable to extract digits");
    }
    free(task);
    return G_SOURCE_REMOVE;
}

gpointer ThreadRunOCR(gpointer thr_data) {
    struct OCRTask* task = thr_data;
    SudOCRu* app = task->app;

    PrintStage(1, 3, "Extracting cells", 0);
    app->cells = ExtractSudokuCells(app->thresholded_image, app->grid, 0,
            &app->cells_len, &app->cropped_grid);
    PrintStage(1, 3, "Extracting cells", 1);

    PrintStage(2, 3, "Preparing Neural Network", 0);
    if (app->nn == NULL)
    {
        app->nn = ReadNetwork("bin/ocr_weights.bin");
        if (app->nn == NULL)
        {
            task->result = 1;
            gdk_threads_add_idle(DoneOCR, task);
            return NULL;
        }
    }
    PrintStage(2, 3, "Preparing Neural Network", 1);

    PrintStage(3, 3, "Reading digits", 0);
    size_t len = app->cells[0]->width * app->cells[0]->height * sizeof(u8);
    u8* tmp = malloc(len);
    for (size_t i = 0; i < app->cells_len; i++)
    {
        SudokuCell* cell = app->cells[i];
        if (CleanCell(cell->data, tmp))
        {
            cell->value = ReadDigit(PrepareCell(cell->data, tmp), app->nn);
        } else {
            cell->value = 0;
        }
    }
    PrintStage(3, 3, "Reading digits...", 1);
    free(tmp);
    task->result = 0;
    gdk_threads_add_idle(DoneOCR, task);
    return NULL;
}

gboolean RunOCR(GtkButton* button, gpointer user_data)
{
    UNUSED(button);
    SudOCRu* app = user_data;

    struct OCRTask* task = malloc(sizeof(struct OCRTask));
    task->app = app;

    ShowLoadingDialog(task->app, "OCRPopup");

    PrintProcedure("OCR");
    g_thread_new("run_ocr", ThreadRunOCR, task);
    return TRUE;
}

static inline void RenderPoint(cairo_t* cr, int x, int y)
{
    cairo_set_source_rgb(cr, 254.0f / 255.0f, 97.0f / 255.0f, 23.0f / 255.0f);
    cairo_arc(cr, x, y, 9, 0.0, 2.0 * M_PI);
    cairo_fill(cr);

    cairo_set_source_rgba(cr, 254.0f / 255.0f, 142.0f / 255.0f, 6.0f / 255.0f,
            0.64f);
    cairo_set_line_width (cr, 2);
    cairo_arc(cr, x, y, 10, 0.0, 2.0 * M_PI);
    cairo_stroke(cr);
}

struct DraggableBB {
    int x1;
    int y1;
    int x2;
    int y2;
    int x3;
    int y3;
    int x4;
    int y4;
    int scaledDown;
    SudOCRu* app;
};

gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    UNUSED(widget);
    struct DraggableBB* dbb = user_data;
    SudOCRu* app = dbb->app;
    if (app->grid == NULL)
        return FALSE;

    // Sets the background to white.
    cairo_set_source_rgba(cr, 1, 1, 1, 0);
    cairo_paint(cr);

    cairo_set_source_rgb(cr, 254.0f / 255.0f, 97.0f / 255.0f, 23.0f / 255.0f);
    cairo_set_line_width(cr, 4);
    cairo_move_to(cr, dbb->x1, dbb->y1);
    cairo_line_to(cr, dbb->x2, dbb->y2);
    cairo_line_to(cr, dbb->x3, dbb->y3);
    cairo_line_to(cr, dbb->x4, dbb->y4);
    cairo_line_to(cr, dbb->x1, dbb->y1);
    cairo_stroke(cr);

    RenderPoint(cr, dbb->x1, dbb->y1);
    RenderPoint(cr, dbb->x2, dbb->y2);
    RenderPoint(cr, dbb->x3, dbb->y3);
    RenderPoint(cr, dbb->x4, dbb->y4);

    return TRUE;
}

void SetupGridDetection(SudOCRu* app)
{
    GtkWindow* main = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "MainWindow"));
    GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "ResizingWindow"));
    GtkButton* next = GTK_BUTTON(gtk_builder_get_object(app->ui,
                "ResizingNextButton"));

    GtkWindowGroup* grp = gtk_window_get_group(main);
    gtk_window_group_add_window(grp, win);
    gtk_window_set_screen(win, gdk_screen_get_default());

    g_signal_connect(next, "clicked", G_CALLBACK(RunOCR), app);

    gtk_window_set_destroy_with_parent(win, TRUE);
    gtk_window_set_modal(win, TRUE);
}

gboolean DestroyDraggableBB(GtkWidget* widget, gpointer user_data)
{
    UNUSED(widget);
    free(user_data);
    return TRUE;
}

gboolean StopDraggableBB(GtkWidget* widget, GdkEvent *e, gpointer user_data)
{
    UNUSED(e);
    struct DraggableBB* dbb = user_data;
    SudOCRu* app = dbb->app;
    GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "ResizingWindow"));
    GtkDrawingArea* area =
        GTK_DRAWING_AREA(gtk_builder_get_object(app->ui, "ResizeArea"));

    g_signal_handlers_disconnect_by_func(G_OBJECT(win),
            G_CALLBACK(StopDraggableBB), dbb);
    g_signal_handlers_disconnect_by_func(win,
            G_CALLBACK(DestroyDraggableBB), dbb);
    g_signal_handlers_disconnect_by_func(area, G_CALLBACK(on_draw), dbb);
    gtk_widget_hide(widget);
    free(user_data);
    return TRUE;
}

void ShowGridDetection(SudOCRu* app)
{
    GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "ResizingWindow"));
    GtkImage* img = GTK_IMAGE(gtk_builder_get_object(app->ui,
                "ResizingImage"));
    GtkDrawingArea* area =
        GTK_DRAWING_AREA(gtk_builder_get_object(app->ui, "ResizeArea"));

    DrawImage(app->original_image, img);

    BBox* bb = app->grid->bounds;
    BBox sorted = {
        bb->x1, bb->y1,
        bb->x2, bb->y2,
        bb->x3, bb->y3,
        bb->x4, bb->y4,
    };
    SortBB(&sorted);

    struct DraggableBB* dbb = malloc(sizeof(struct DraggableBB));
    dbb->app = app;
    dbb->scaledDown = 0;
    if (app->original_image->height > 800)
    {
        dbb->scaledDown = 1;
        int* arr = (int*) &sorted;
        for (size_t i = 0; i < 8; i++)
        {
            arr[i] /= 2;
        }
    }
    memcpy(dbb, &sorted, 8 * sizeof(int));

    g_signal_connect(G_OBJECT(win),
        "delete-event", G_CALLBACK(StopDraggableBB), dbb);
    g_signal_connect(win, "destroy", G_CALLBACK(DestroyDraggableBB), dbb);
    g_signal_connect(area, "draw", G_CALLBACK(on_draw), dbb);

    gtk_widget_show(GTK_WIDGET(win));
    gtk_window_set_keep_above(win, TRUE);
}
