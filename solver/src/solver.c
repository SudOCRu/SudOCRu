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
    Operation_Successed = 1,

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

    //Print = 
    Cant_Print_Null_Sudoku
};


/*  > CreateSudoku
 * Initializes a new Sudoku board. 
 * > Returns a new Sudoku (cols*rows) from array
 *      - array : Array to create Sudoku
 *      - cols : Number of cols of Sudoku board
 *      - cols : Number of rows of Sudoku board
 */
Sudoku* CreateSudoku(u8* array, u8 boardedge, u8 boardsize, u8 nbsquares){

    Sudoku* sudoku = malloc(sizeof(Sudoku));
    sudoku->board = array; 
    sudoku->boardedge = boardedge;
    sudoku->boardsize = boardsize;
    sudoku->nbsquares = nbsquares;

    u8 index = 0;

    while (index < sudoku->boardsize){
        sudoku->board[index] = array[index];
        index++;
    }

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
Sudoku* ImportSudoku(char* in_file){
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

    return CreateSudoku(array, len, len*len, len/3);
}

/*  > SaveSudoku
 * Save a Sudoku board to a given file using the EPITA format (9x9). This 
 * function returns 
 * > Returns 0 (false) if the board couldn't be saved, else 1 (true)
 *      - sudoku : Sudoku to save into file
 *      - out_file : Name of saved file that contains Sudoku grid
 */
int SaveSudoku(const Sudoku* sudoku, char* out_file){

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
        if (index != 0 && index % sudoku->boardedge == 0) fprintf(file, "\n");
        else if (index != 0 && index % 3 == 0) fprintf(file, " ");
        if (sudoku->board[index] != 0){
            fprintf(file, "%hhu", sudoku->board[index]);
        }
        else fprintf(file, ".");

        index++;
    }
    fprintf(file, "\n");

    fclose(file);
    return Operation_Successed;
}



// ===========================================================================
// ----------------------------      Checks        --------------------------- 
// ===========================================================================

short PossibleValues(const Sudoku* sudoku, u8 index);
int clear(short n, short* flag);

/*  > IsSudokuValid
 * Check if "sudoku" grid is valid (if it's solvable)
 * > Returns 0 (false) if the board is not valid, else 1 (true)
 *      - sudoku : Sudoku grid to check
 */
int IsSudokuValid(Sudoku* sudoku){
    Sudoku* issolvable = SolveSudoku(sudoku, 0);
    if (issolvable == NULL){
        errx(Sudoku_Not_Solvable, 
                "IsSudokuValid: Sudoku not solvable (no solutions available)");
        return Sudoku_Not_Solvable;
    }
    return Operation_Successed;
}

/*  > IsSudokuSolved
 * Check if "sudoku" grid is solved (if the grid is filled of numbers != 0)
 * > Returns 0 (false) if the board not solved, else 1 (true)
 *      - sudoku : Sudoku grid to check
 */
int IsSudokuSolved(const Sudoku* sudoku){
    u8 index = 0;

    while(index < sudoku->boardsize && PossibleValues(sudoku, index) == 0) 
        index++;

    return index == sudoku->boardsize;
}

/*  > verify
 * Verify if a number is playable in the sudoku (check in PossibleValues if 
 * it is playable)
 * > Returns 0 (false) if the number is playable, else 1 (true)
 *      - n : number to test
 *      - flag : all possibilities
 */
int verify(short n, short flag){
    short mark = 1 << (n-1);
    return ((flag & mark) != 0) ? 1 : 0;
}

/*  > set
 * Set a number as playable in the sudoku (set in PossibleValues if 
 * it is playable)
 * > Returns the flag (possibilties) updated with the new possibility
 *      - n : number to set
 *      - flag : all possibilities
 */
short set (short n, short flag){
    short mark = 1 << (n-1);
    return flag | mark;
}

/*  > clear
 * Clear a number as unplayable in the sudoku (clear in PossibleValues if 
 * it is unplayable)
 * > Returns *nothing*
 *      - n : number to set
 *      - flag : all possibilities
 */
int clear (short n, short* flag){
    short mark = 1 << (n-1);
    *flag = *flag & ~mark;
    return *flag == 0;
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
    size_t idx;
    u8 cell;

    u8 init_row = (row/sudoku->nbsquares)*sudoku->nbsquares;
    u8 init_col = (col/sudoku->nbsquares)*sudoku->nbsquares;

    for(size_t i = 0; i < sudoku->boardedge; i++)
    {
        /// horizontal
        idx = row * sudoku->boardedge + i;
        cell = sudoku->board[idx];
        if (cell != 0 && clear(cell, &possibilities)) {
            return 0;
        }

        // vertical
        idx = i * sudoku->boardedge + col;
        cell = sudoku->board[idx];
        if (cell != 0 && clear(cell, &possibilities)) {
            return 0;
        }

        // groups
        idx = (init_row + (i / sudoku->nbsquares)) * sudoku->boardedge
            + ((i % sudoku->nbsquares) + init_col);
        cell = sudoku->board[idx];
        if (cell != 0 && clear(cell, &possibilities)) {
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
        return Operation_Successed;

    short possibilities = PossibleValues(sudoku, i);
    for (u8 n = 1; n <= 9; n++)
    {
        if (verify(n, possibilities))
        {
            sudoku->board[i] = n;

            if(Backtracking(sudoku, i) == Operation_Successed)
            { 
                return Operation_Successed;
            }

            sudoku->board[i] = 0;
        }
    }

    return Sudoku_Not_Solved;
}


int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

int IntBacktracking(Sudoku* org, Sudoku* sudoku, size_t i){
    // Get next position
    while (i < sudoku->boardsize && sudoku->board[i] != 0) i++;
    if (i >= sudoku->boardsize)
        return Operation_Successed;

    short possibilities = PossibleValues(sudoku, i);
    for (u8 n = 1; n <= 9; n++)
    {
        if (verify(n, possibilities))
        {
            sudoku->board[i] = n;

            printf("\e[1;1H\e[2J");
            PrintBoardCol(org, sudoku);
            msleep(15);

            if(IntBacktracking(org, sudoku, i) == Operation_Successed)
            { 
                return Operation_Successed;
            }
            sudoku->board[i] = 0;
        }
    }

    return Sudoku_Not_Solved;
}

/*  > SolveSudoku
 * Solve Sudoku using back-tracking algorithm
 * > Returns NULL if the board is not solved, else a new Sudoku grid solved
 *      - sudoku : Sudoku grid to solve
 */
Sudoku* SolveSudoku(Sudoku* sudoku, int interactive){ 
    size_t size = sudoku->boardsize * sizeof(u8);
    u8* arr = malloc(size);
    memcpy(arr, sudoku->board, size);
    Sudoku* copy = CreateSudoku(arr,
            sudoku->boardedge, sudoku->boardsize, sudoku->nbsquares);

    int is_solved = interactive ? IntBacktracking(sudoku, copy, 0) :
        Backtracking(copy, 0);

    if (is_solved == Operation_Successed)
        return copy;
    errx(Sudoku_Not_Solved, "Sudoku not solved, no solutions found");
    return NULL;
}

/*  > PrintSudoku
 * Print the sudoku board
 * > Returns *anything*
 *      - sudoku : Sudoku grid to print
 */
void PrintBoard(const Sudoku* sudoku){
    if (sudoku == NULL) 
        errx(Cant_Print_Null_Sudoku, "PrintBoard: Can't print null Sudoku");

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
        errx(Cant_Print_Null_Sudoku, "PrintBoard: Can't print null Sudoku");

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
