#include "windows.h"
#include <solver.h>
#include "../utils.h"

struct CellDetails {
    SudOCRu* app;
    unsigned char id;
    GtkButton* button;
};

gboolean CloseEditCell(GtkWidget *widget, GdkEvent *event, gpointer user_data);

gboolean SaveCell(GtkButton* button, gpointer user_data)
{
    UNUSED(button);
    struct CellDetails* details = user_data;
    SudOCRu* app = details->app;

    GtkEntry* num_input = GTK_ENTRY(gtk_builder_get_object(app->ui,
                "CellTextBox"));
    GtkStyleContext* ctx = gtk_widget_get_style_context(
            GTK_WIDGET(details->button));
    const gchar *text = gtk_entry_get_text(num_input);
    if (text[0] >= '0' && text[0] <= '9')
    {
        SudokuCell* cell = app->cells[details->id];
        cell->value = text[0] - '0';
        char digit[3] = { 0, };
        if (cell->value != 0) {
            snprintf(digit, sizeof(digit), "%hhu", cell->value);
            gtk_style_context_add_class(ctx, "colored");
        } else {
            gtk_style_context_remove_class(ctx, "colored");
        }
        gtk_style_context_remove_class(ctx, "error");
        gtk_button_set_label(details->button, digit);
        gtk_widget_queue_draw(GTK_WIDGET(details->button));
    }

    GtkWindow* dialog = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "CellModifPopup"));
    GtkButton* save = GTK_BUTTON(gtk_builder_get_object(app->ui,
                "SaveCellButton"));
    g_signal_handlers_disconnect_by_func(save, G_CALLBACK(SaveCell), details);
    g_signal_handlers_disconnect_by_func(G_OBJECT(dialog),
            G_CALLBACK(CloseEditCell), details);
    HideWindow(app, "CellModifPopup");
    return TRUE;
}

gboolean CloseEditCell(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    UNUSED(widget);
    UNUSED(event);
    struct CellDetails* details = user_data;
    SudOCRu* app = details->app;
    GtkWindow* dialog = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "CellModifPopup"));
    GtkButton* save = GTK_BUTTON(gtk_builder_get_object(app->ui,
                "SaveCellButton"));

    g_signal_handlers_disconnect_by_func(save, G_CALLBACK(SaveCell), details);
    g_signal_handlers_disconnect_by_func(G_OBJECT(dialog),
            G_CALLBACK(CloseEditCell), details);

    GtkStyleContext* ctx = gtk_widget_get_style_context(
            GTK_WIDGET(details->button));
    gtk_style_context_remove_class(ctx, "error");
    gtk_widget_queue_draw(GTK_WIDGET(details->button));

    HideWindow(app, "CellModifPopup");
    return TRUE;
}

gboolean EditCell(GtkButton* button, gpointer user_data)
{
    UNUSED(button);
    struct CellDetails* details = user_data;
    SudOCRu* app = details->app;

    GtkWindow* dialog = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "CellModifPopup"));
    GtkButton* save = GTK_BUTTON(gtk_builder_get_object(app->ui,
                "SaveCellButton"));
    GtkImage* display = GTK_IMAGE(gtk_builder_get_object(app->ui,
                "CellImage"));
    GtkEntry* num_input = GTK_ENTRY(gtk_builder_get_object(app->ui,
                "CellTextBox"));

    SudokuCell* cell = app->cells[details->id];
    DrawImage(cell->data, display);
    char digit[4] = { 0, };
    if (cell->value != 0)
        snprintf(digit, sizeof(digit), "%hhu", cell->value);
    gtk_entry_set_text(num_input, digit);
    gtk_entry_set_alignment(num_input, 0.5);

    char name[14];
    snprintf(name, sizeof(name), "Edit Cell %02hhu", details->id);

    gtk_window_set_title(dialog, name);
    gtk_window_set_destroy_with_parent(dialog, TRUE);
    gtk_window_set_modal(dialog, TRUE);
    g_signal_connect(G_OBJECT(save), "clicked", G_CALLBACK(SaveCell), details);
    g_signal_connect(G_OBJECT(dialog),
        "delete-event", G_CALLBACK(CloseEditCell), details);
    gtk_widget_show(GTK_WIDGET(dialog));
    gtk_window_set_keep_above(dialog, TRUE);
    return TRUE;
}

struct SolveTask {
    SudOCRu* app;
    int result;
    InvalidSudokuError* error;
};

gboolean DoneSolve(gpointer user_data)
{
    struct SolveTask* task = user_data;
    SudOCRu* app = task->app;
    HideWindow(app, "SolvingPopup");
    if (!task->result)
    {
        HideWindow(app, "OCRCorrection");
        ShowSolveResults(app);
    } else if (task->error != NULL) {
        InvalidSudokuError* error = task->error;
        PrintError(app->sudoku, error);

        GList *children, *iter;
        GtkStyleContext *ctx;

        GtkContainer* container = GTK_CONTAINER(gtk_builder_get_object(app->ui,
                "CellGrid"));
        children = gtk_container_get_children(container);
        unsigned char i = 80;
        for(iter = children; iter != NULL; iter = g_list_next(iter))
        {
            ctx = gtk_widget_get_style_context(iter->data);

            if (i-- == error->error_pos)
            {
                gtk_style_context_add_class(ctx, "error");
                break;
            }
        }
        g_list_free(children);

        char* type = error->type == 0 ? "Group" :
            (error->type == 1 ? "Horizontal" :
             (error->type == 2 ? "Vertical" : "IsSolved"));
        size_t error_pos = error->error_pos;
        short flag = error->flag;
        char msg[100] = { 0, };

        snprintf(msg, sizeof(msg), "%s check failed:\nInvalid cell at index %lu"
                ", cell %hi is already placed (state: %hi)", type, error_pos,
                app->sudoku->board[error_pos], flag);
        ShowErrorMessage(app, "OCRCorrection", "Invalid Sudoku", msg);
        free(task->error);
    } else {
        ShowErrorMessage(app, "OCRCorrection", "Unable to solve sudoku",
                "No solution were found :(");
    }
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(app->ui,
                 "OCRNextButton")), TRUE);
    free(task);
    return G_SOURCE_REMOVE;
}

gpointer ThreadSolveSudoku(gpointer thr_data)
{
    struct SolveTask* task = thr_data;
    SudOCRu* app = task->app;

    PrintStage(1, 2, "Preparing sudoku", 0);
    u8 cells[81];
    for (size_t i = 0; i < sizeof(cells); i++)
    {
        cells[i] = app->cells[i]->value;
    }
    app->sudoku = CreateSudoku((const u8*)&cells, 9, 3);
    InvalidSudokuError* error;
    int isSolvable = IsSudokuValid(app->sudoku, &error);
    if (isSolvable != 1)
    {
        printf("\n");
        task->result = isSolvable;
        task->error = error;
        gdk_threads_add_idle(DoneSolve, task);
        return NULL;
    }
    PrintStage(1, 2, "Preparing sudoku", 1);

    PrintStage(2, 2, "Solving sudoku", 0);
    int solved = Backtracking(app->sudoku, 0);
    if (solved != 1)
    {
        printf("\n");
        task->result = solved;
        gdk_threads_add_idle(DoneSolve, task);
        return NULL;
    }
    PrintStage(2, 2, "Solving sudoku", 1);
    PrintBoard(app->sudoku);

    task->result = 0;
    gdk_threads_add_idle(DoneSolve, task);
    return NULL;
}

gboolean RunSudokuSolver(GtkButton* button, gpointer user_data)
{
    SudOCRu* app = user_data;
    UNUSED(button);

    struct SolveTask* task = calloc(1, sizeof(struct SolveTask));
    task->app = app;

    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(app->ui,
                 "OCRNextButton")), FALSE);
    ShowLoadingDialog(app, "OCRCorrection", "SolvingPopup");

    PrintProcedure("Sudoku Solver");
    g_thread_new("sudoku_solver", ThreadSolveSudoku, task);
    return TRUE;
}

gboolean SaveOCRResults(GtkButton* button, gpointer user_data)
{
    SudOCRu* app = user_data;
    UNUSED(button);

    GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "OCRCorrection"));
    GtkFileChooserNative *dialog = gtk_file_chooser_native_new(
            "Export OCR results",
            GTK_WINDOW(win),
            GTK_FILE_CHOOSER_ACTION_SAVE,
            "Save", "Cancel");
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "sudoku");

    GtkFileFilter *filter = gtk_file_filter_new();
    filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "All files");
    gtk_file_filter_add_pattern(filter, "*");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    gint res = gtk_native_dialog_run(GTK_NATIVE_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char *file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        u8 cells[81];
        for (size_t i = 0; i < sizeof(cells); i++)
        {
            cells[i] = app->cells[i]->value;
        }
        Sudoku* sudoku = CreateSudoku((const u8*)&cells, 9, 3);
        if (SaveSudoku(sudoku, file))
            printf("Successfully export sudoku as `%s`\n", file);
        DestroySudoku(sudoku);
    }

    g_object_unref(dialog);
    return TRUE;
}

void SetupOCRResults(SudOCRu* app)
{
    GtkWindow* main = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "MainWindow"));
    GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "OCRCorrection"));
    GtkButton* next = GTK_BUTTON(gtk_builder_get_object(app->ui,
                "OCRNextButton"));
    GtkButton* save = GTK_BUTTON(gtk_builder_get_object(app->ui,
                "OCRSaveAsText"));

    GtkWindowGroup* grp = gtk_window_get_group(main);
    gtk_window_group_add_window(grp, win);
    gtk_window_set_screen(win, gdk_screen_get_default());

    GtkContainer* container = GTK_CONTAINER(gtk_builder_get_object(app->ui,
                "CellGrid"));
    GList *children, *iter;
    GtkStyleContext *ctx;

    children = gtk_container_get_children(container);
    unsigned char i = 80;
    for(iter = children; iter != NULL; iter = g_list_next(iter))
    {
        struct CellDetails* details = malloc(sizeof(struct CellDetails));
        details->app = app;
        details->id = i--;
        details->button = GTK_BUTTON(iter->data);

        ctx = gtk_widget_get_style_context(iter->data);
        gtk_style_context_add_class(ctx, "cell");
        gtk_style_context_remove_class(ctx, "colored");
        gtk_button_set_relief(details->button, GTK_RELIEF_NONE);

        g_signal_connect(GTK_WIDGET(iter->data),
                "clicked", G_CALLBACK(EditCell), details);
    }
    g_list_free(children);

    g_signal_connect(G_OBJECT(win),
        "delete-event", G_CALLBACK(hide_window), NULL);
    g_signal_connect(win, "destroy", G_CALLBACK(hide_window), NULL);
    g_signal_connect(save, "clicked", G_CALLBACK(SaveOCRResults), app);
    g_signal_connect(next, "clicked", G_CALLBACK(RunSudokuSolver), app);

    gtk_window_set_transient_for(win, main);
    gtk_window_set_destroy_with_parent(win, TRUE);
    gtk_window_set_modal(win, TRUE);
}

void ShowOCRResults(SudOCRu* app)
{
    GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "OCRCorrection"));

    GtkImage* img = GTK_IMAGE(gtk_builder_get_object(app->ui,
                "OCRImage"));
    DrawImage(app->cropped_grid, img);

    GtkContainer* container = GTK_CONTAINER(gtk_builder_get_object(app->ui,
                "CellGrid"));
    GtkButton* but;
    GList *children, *iter;
    GtkStyleContext *ctx;

    children = gtk_container_get_children(container);
    unsigned char i = 80;
    char digit[4] = { 0, };
    for(iter = children; iter != NULL; iter = g_list_next(iter))
    {
        ctx = gtk_widget_get_style_context(iter->data);
        but = GTK_BUTTON(iter->data);

        SudokuCell* cell = app->cells[i--];
        if (cell->value != 0) {
            snprintf(digit, sizeof(digit), "%hhu", cell->value);
            gtk_button_set_label(but, digit);
            gtk_style_context_add_class(ctx, "colored");
        } else {
            gtk_button_set_label(but, "");
            gtk_style_context_remove_class(ctx, "colored");
        }
        gtk_style_context_remove_class(ctx, "error");
    }
    g_list_free(children);

    gtk_widget_show(GTK_WIDGET(win));
}
