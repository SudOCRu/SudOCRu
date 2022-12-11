/*
 * ===========================================================================
 *
 *       Filename:  solver.c
 *
 *    Description: Functions used to solve Sudokus
 *
 *        Version:  1.0.0
 *        Created:  10/01/22 16:12:22
 *       Revision:  Official Release of Sudoku Solver from SudOCRu
 *       Compiler:  gcc
 *
 *         Author:  Kevin JAMET,
 *   Organization:  SudOCRu
 *
 * ===========================================================================
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <err.h>
#include <time.h>
#include <string.h>

#include "solver.h"

// ===========================================================================
// -----------------------      Sudoku Treatment        ----------------------
// ===========================================================================

enum SudokuError {
    // Sudoku = 0
    Exit_Success = 0,
    Operation_Succeeded = 1,

    //SolverError = 10
    Sudoku_Not_Solvable = 11,
    Sudoku_Not_Solved = 12,

    //CreateSudoku = 10

    //ImportSudoku = 20
    File_Do_Not_Exists = 21,
    Empty_File = 22,
    Sudoku_Format_Invalid = 23,
    Invalid_Character = 24,

    //SaveSudoku = 30
    Cant_Save_Sudoku = 31,

    //DestroyError = 40
    Sudoku_Null = 41,
};


/*  > CreateSudoku
 * Initializes a new Sudoku board.
 * > Returns a new Sudoku (cols*rows) from array
 *      - array : Array to create Sudoku
 *      - cols : Number of cols of Sudoku board
 *      - cols : Number of rows of Sudoku board
 */
Sudoku* CreateSudoku(const u8* array, u8 side, u8 nbsquares){
    Sudoku* sudoku = malloc(sizeof(Sudoku));

    size_t size = side * side * sizeof(u8);
    u8* board = malloc(size);
    memcpy(board, array, size);

    sudoku->board = board;
    sudoku->boardedge = side;
    sudoku->boardsize = side * side;
    sudoku->nbsquares = nbsquares;

    return sudoku;
}

/*  > DestroySudoku
 * Destroys of the provided Sudoku board.
 *      - board : Board to destroy
 */
void DestroySudoku(Sudoku* sudoku){
    if (sudoku == NULL)
        return;

    free(sudoku->board);
    free(sudoku);
}

/*  > ImportSudoku
 * Import a Sudoku board from a given file using the EPITA format (9x9). This
 * function returns NULL if the board couldn't be loaded or the file does not
 * exist.
 * > Returns a Sudoku board loaded from file "in_file"
 *      - in_file : Path of file to create the new Sudoku grid
 */
Sudoku* ImportSudoku(const char* in_file){
    // INDEXES
    size_t index = 0;

    // VARIABLES TO READ LINES
    FILE* file = NULL;
    char* line = NULL;
    size_t length = 0;
    ssize_t line_reader;

    // CHECK IF FILE EXISTS
    file = fopen(in_file, "r");
    if (file == NULL)
        errx(File_Do_Not_Exists,
                "ImportSudoku: File do not exists named as %s", in_file);

    // COUNT THE NUMBER OF ROWS/COLS
    // FILL THE SUDOKU
    u8* array = NULL;
    size_t len = 0;
    while ((line_reader = getline(&line, &length, file)) != -1){
        //printf("%lu", length);
        // SUDOKU FILLING
        if (array == NULL) {
            while (line[len] != '\n') len++;
            // ---  DEBUG  ---
            //printf("%u\n", (unsigned char) len);
            if (len == 11 || len == 12) // Sudoku
                len = 9;
            else if (len == 19 || len == 20) // HexaSudoku
                len = 16;
            else
                errx(Sudoku_Format_Invalid,
                        "ImportSudoku: Sudoku Format Invalid \
                        (length of line should be 11 or 19 but was %u",
                        (unsigned char) len);
            array = calloc(len * len, sizeof(u8));
        }
        for (ssize_t i = 0; i < line_reader; i++){
            if (line[i] != '\n' && line[i] != '\r'
                    && line[i] != 0 && line[i] != ' ' ){
                if (line[i] == '.' || line[i] == '0')
                    index++;
                else if (line[i] > '0' && line[i] <= '9'){
                    //printf("%u\n", (unsigned char) len);
                    // TODO change this for hexa tables
                    //printf("placed at index %lu, %c\n", index, line[i]);
                    array[index] = line[i] - '0';
                    //printf("placed at index %lu, %c\n", index, line[i]);
                    index++;
                }
                else {
                    // invalid char at index: %lu => '%hhu'\n", i, line[i]
                    fclose(file);
                    if (line) free(line);
                    free(array);

                    errx(Invalid_Character,
                            "ImportSudoku: Invalid char at \
                            index: %lu => '%c'\n", i, line[i]);
                }
            }
        }
    }

    if (line) free(line);
    fclose(file);

    if (array == NULL)
        errx(Empty_File, "ImportSudoku: Empty file named %s", in_file);

    return CreateSudoku(array, len, len/3);
}

/*  > SaveSudoku
 * Save a Sudoku board to a given file using the EPITA format (9x9). This
 * function returns
 * > Returns 0 (false) if the board couldn't be saved, else 1 (true)
 *      - sudoku : Sudoku to save into file
 *      - out_file : Name of saved file that contains Sudoku grid
 */
int SaveSudoku(const Sudoku* sudoku, const char* out_file){

    // INDEXES
    size_t index = 0;

    // VARIABLES TO READ LINES
    FILE* file = NULL;

    // CHECK IF FILE EXISTS
    file = fopen(out_file, "w");
    if (file == NULL) {
        errx(Cant_Save_Sudoku,
                "SaveSudoku: Can't save Sudoku at location %s", out_file);
        return Cant_Save_Sudoku;
    }

    while (index < sudoku->boardsize){
        if (index != 0 && index % sudoku->boardedge == 0)
        {
            if ((index / sudoku->boardedge) % sudoku->nbsquares == 0)
                fprintf(file, "\n");
            fprintf(file, "\n");
        }
        else if (index != 0 && (index % sudoku->nbsquares) == 0)
            fprintf(file, " ");

        if (sudoku->board[index] != 0){
            fprintf(file, "%hhu", sudoku->board[index]);
        }
        else fprintf(file, ".");

        index++;
    }
    fprintf(file, "\n");

    fclose(file);
    return Operation_Succeeded;
}



// ===========================================================================
// ----------------------------      Checks        ---------------------------
// ===========================================================================

/*  > IsSudokuValid > vset
 * Sets the bit `n` to one. If the bit was already set, this function returns 1,
 * 0 otherwise.
 *      - n : Cell value
 *      - flag : possibilities bit mask
 */
int vset(short n, short* flag){
    short mark = 1 << (n-1);
    if ((*flag & mark) != 0) return 1;
    *flag = *flag | mark;
    return 0;
}

/*  > IsSudokuValid > PrintError
 * Prints the error description and position to stdout
 *      - org : Original Sudoku grid
 *      - type : Check identifier
 *      - i : Error position
 *      - flag : possibilities bit mask
 */
void PrintError(const Sudoku* sudoku, const InvalidSudokuError* error)
{
    char* type = error->type == 0 ? "Group" :
        (error->type == 1 ? "Horizontal" :
         (error->type == 2 ? "Vertical" : "IsSolved"));
    size_t error_pos = error->error_pos;
    short flag = error->flag;
    fprintf(stderr, "%s check failed: Invalid cell at index %lu, cell %hi is "
            "already placed (state: %hi)\n", type, error_pos,
            sudoku->board[error_pos], flag);
    for (size_t col = 0; col < sudoku->boardedge; col++){
        fprintf(stderr, " ---");
    }
    fprintf(stderr, "\n");

    for (size_t row = 0; row < sudoku->boardedge; row++){
        fprintf(stderr, "|");
        for (size_t col = 0; col < sudoku->boardedge; col++){
            size_t i = row*sudoku->boardedge + col;
            if (i == error_pos)
                fprintf(stderr, " \x1b[31m%hhu\x1b[0m |", sudoku->board[i]);
            else if (sudoku->board[i] != 0)
                fprintf(stderr, " %hhu |", sudoku->board[i]);
            else
                fprintf(stderr, "   |");
        }
        fprintf(stderr, "\n");
        for (size_t col = 0; col < sudoku->boardedge; col++){
            fprintf(stderr, " ---");
        }
        fprintf(stderr, "\n");
    }
}

/*  > IsSudokuValid
 * Check if "sudoku" grid is valid (if it's solvable)
 * > Returns 0 (false) if the board is not valid, else 1 (true)
 *      - sudoku : Sudoku grid to check
 */
int IsSudokuValid(const Sudoku* sudoku, InvalidSudokuError** out_error){
    u8 idx = 0, cell, row, col, init_row, init_col;
    short possibilities;

    for(u8 gy = 0; gy < sudoku->nbsquares; gy++)
    {
        init_row = gy * sudoku->nbsquares;

        for(u8 gx = 0; gx < sudoku->nbsquares; gx++)
        {
            init_col = gx * sudoku->nbsquares;

            possibilities = 0b000000000;
            for(u8 i = 0; i < sudoku->boardedge; i++)
            {
                idx = (init_row + (i / sudoku->nbsquares)) * sudoku->boardedge
                    + ((i % sudoku->nbsquares) + init_col);
                cell = sudoku->board[idx];
                if (cell != 0 && vset(cell, &possibilities)) {
                    InvalidSudokuError* error =
                        malloc(sizeof(InvalidSudokuError));
                    error->type = 0;
                    error->error_pos = idx;
                    error->flag = possibilities;
                    *out_error = error;
                    return Sudoku_Not_Solvable;
                }
            }
        }
    }

    for (u8 index = 0; index < sudoku->boardsize; index++)
    {
        row = index / sudoku->boardedge;
        col = index % sudoku->boardedge;

        // horizontal
        possibilities = 0b000000000;
        for(u8 i = 0; i < sudoku->boardedge; i++)
        {
            idx = row * sudoku->boardedge + i;
            cell = sudoku->board[idx];
            if (cell != 0 && vset(cell, &possibilities)) {
                InvalidSudokuError* error = malloc(sizeof(InvalidSudokuError));
                error->type = 1;
                error->error_pos = idx;
                error->flag = possibilities;
                *out_error = error;
                return Sudoku_Not_Solvable;
            }
        }

        // vertical
        possibilities = 0b000000000;
        for(u8 i = col; i < sudoku->boardsize; i += sudoku->boardedge)
        {
            cell = sudoku->board[i];
            if (cell != 0 && vset(cell, &possibilities)) {
                InvalidSudokuError* error = malloc(sizeof(InvalidSudokuError));
                error->type = 2;
                error->error_pos = idx;
                error->flag = possibilities;
                *out_error = error;
                return Sudoku_Not_Solvable;
            }
        }
    }

    return Operation_Succeeded;
}

const short SET_MASKS[10] = {
    0,
    1 << 0,
    1 << 1,
    1 << 2,
    1 << 3,
    1 << 4,
    1 << 5,
    1 << 6,
    1 << 7,
    1 << 8,
};

const short CLEAR_MASKS[10] = {
    0b111111111,
    ~(1 << 0),
    ~(1 << 1),
    ~(1 << 2),
    ~(1 << 3),
    ~(1 << 4),
    ~(1 << 5),
    ~(1 << 6),
    ~(1 << 7),
    ~(1 << 8),
};

/*  > IsSudokuSolved
 * Check if "sudoku" grid is solved (if the grid is filled of numbers != 0)
 * > Returns 0 (false) if the board not solved, else 1 (true)
 *      - sudoku : Sudoku grid to check
 */
int IsSudokuSolved(const Sudoku* sudoku){
    u8 i = 0;

    while(i < sudoku->boardsize && PossibleValues(sudoku, i) == 0) i++;
    if (i != sudoku->boardsize)
    {
        InvalidSudokuError err = { 3, i, PossibleValues(sudoku, i) };
        PrintError(sudoku, &err);
    }

    return i == sudoku->boardsize;
}

// ===========================================================================
// ----------------------------      SOLVE        ----------------------------
// ===========================================================================

/*  > PossibleValues
 * Returns the numbers that are possible to put in the gris at (x, y)
 * positions (represented in binary representation)
 * > Returns the possibilites in power of two added to the number
 *      - sudoku : Sudoku grid to check
 *      - index : index of box to find possibilities
 */
short PossibleValues(const Sudoku* sudoku, u8 index){

    // possibilities are in binary representation and are power of 2 numbers
    //  ex : if 9 is possible : 2^9 is added to possibilities

    u8 row = index / sudoku->boardedge;
    u8 col = index % sudoku->boardedge;
    short possibilities = 0b111111111;
    u8 cell;

    u8 init_row = (row/sudoku->nbsquares)*sudoku->nbsquares;
    u8 init_col = (col/sudoku->nbsquares)*sudoku->nbsquares;

    for (size_t i = 0, idx = 0; i < sudoku->boardedge; i++)
    {
        /// horizontal
        idx = row * sudoku->boardedge + i;
        cell = sudoku->board[idx];
        if ((possibilities &= CLEAR_MASKS[cell]) == 0) {
            return 0;
        }

        // vertical
        idx = i * sudoku->boardedge + col;
        cell = sudoku->board[idx];
        if ((possibilities &= CLEAR_MASKS[cell]) == 0) {
            return 0;
        }

        // groups
        idx = (init_row + (i / sudoku->nbsquares)) * sudoku->boardedge
            + ((i % sudoku->nbsquares) + init_col);
        cell = sudoku->board[idx];
        if ((possibilities &= CLEAR_MASKS[cell]) == 0) {
            return 0;
        }
    }

    return possibilities;
}

/*  > Backtracking
 * Recursion of solve using backtracking algo. For each box box in the sudoku
 * grid, it will test every solution possible until it tested all solutions.
 * Sudoku in ref is modified to return the sudoku solved
 * > Returns 0 (false) if it was not able to find a solution, else 1 (true)
 *      - sudoku : sudoku pointer to modify
 */
int Backtracking(Sudoku* sudoku, size_t i){
    // Get next position
    while (i < sudoku->boardsize && sudoku->board[i] != 0) i++;
    if (i >= sudoku->boardsize)
        return Operation_Succeeded;

    short possibilities = PossibleValues(sudoku, i);
    for (u8 n = 1; n <= 9; n++)
    {
        if ((possibilities & SET_MASKS[n]) != 0)
        {
            sudoku->board[i] = n;

            if (Backtracking(sudoku, i + 1) == Operation_Succeeded)
            {
                return Operation_Succeeded;
            }

        }
    }
    sudoku->board[i] = 0;

    return Sudoku_Not_Solved;
}

int msleep(unsigned long msec)
{
    struct timespec period;
    period.tv_sec = msec / 1000;
    period.tv_nsec = (msec % 1000) * 1000000;

    int res;
    do {
        res = nanosleep(&period, &period);
    } while (res && errno == EINTR);

    return res;
}

int IntBacktracking(const Sudoku* org, Sudoku* sudoku, size_t i){
    // Get next position
    while (i < sudoku->boardsize && sudoku->board[i] != 0) i++;
    if (i >= sudoku->boardsize)
        return Operation_Succeeded;

    u8 len = 2 * sudoku->boardedge + 1;
    short possibilities = PossibleValues(sudoku, i);
    for (u8 n = 1; n <= 9; n++)
    {
        if ((possibilities & SET_MASKS[n]) != 0)
        {
            sudoku->board[i] = n;

            PrintBoardCol(org, sudoku);
            printf("\033[%hhuA", len);
            fflush(stdout);
            msleep(15);

            if(IntBacktracking(org, sudoku, i + 1) == Operation_Succeeded)
            {
                return Operation_Succeeded;
            }
        }
    }
    sudoku->board[i] = 0;

    return Sudoku_Not_Solved;
}

/*  > SolveSudoku
 * Solve Sudoku using back-tracking algorithm
 * > Returns NULL if the board is not solved, else a new Sudoku grid solved
 *      - sudoku : Sudoku grid to solve
 */
Sudoku* SolveSudoku(const Sudoku* sudoku, int interactive){
    InvalidSudokuError* err;
    if (IsSudokuValid(sudoku, &err) != Operation_Succeeded)
    {
        PrintError(sudoku, err);
        free(err);
        errx(Sudoku_Not_Solved, "SolveSudoku: The board is not a valid sudoku");
        return NULL;
    }

    Sudoku* copy = CreateSudoku(sudoku->board, sudoku->boardedge,
            sudoku->nbsquares);

    int is_solved = interactive ? IntBacktracking(sudoku, copy, 0) :
        Backtracking(copy, 0);

    if (is_solved == Operation_Succeeded)
        return copy;

    DestroySudoku(copy);
    errx(Sudoku_Not_Solved, "SolveSudoku: No solutions were found");
    return NULL;
}

/*  > PrintSudoku
 * Print the sudoku board
 * > Returns *anything*
 *      - sudoku : Sudoku grid to print
 */
void PrintBoard(const Sudoku* sudoku){
    if (sudoku == NULL)
        return;

    for (size_t col = 0; col < sudoku->boardedge; col++){
        printf(" ---");
    }
    printf("\n");

    for (size_t row = 0; row < sudoku->boardedge; row++){
        printf("|");
        for (size_t col = 0; col < sudoku->boardedge; col++){
            if (sudoku->board[row*sudoku->boardedge + col] != 0)
                printf(" %hhu |", sudoku->board[row*sudoku->boardedge + col]);
            else
                printf("   |");
        }
        printf("\n");
        for (size_t col = 0; col < sudoku->boardedge; col++){
            printf(" ---");
        }
        printf("\n");
    }
}

void PrintBoardCol(const Sudoku* org, const Sudoku* sudoku){
    if (sudoku == NULL)
        return;

    for (size_t col = 0; col < sudoku->boardedge; col++){
        printf(" ---");
    }
    printf("\n");

    for (size_t row = 0; row < sudoku->boardedge; row++){
        printf("|");
        for (size_t col = 0; col < sudoku->boardedge; col++){
            size_t i = row*sudoku->boardedge + col;
            if (sudoku->board[i] != org->board[i])
                printf(" \x1b[31m%hhu\x1b[0m |", sudoku->board[i]);
            else if (sudoku->board[i] != 0)
                printf(" %hhu |", sudoku->board[i]);
            else
                printf("   |");
        }
        printf("\n");
        for (size_t col = 0; col < sudoku->boardedge; col++){
            printf(" ---");
        }
        printf("\n");
    }
}
