/*
 * ===========================================================================
 *
 *       Filename:  solver.c
 *
 *    Description: Functions used to solve Sudokus 
 *
 *        Version:  0.5.2
 *        Created:  10/01/22 16:12:22
 *       Revision:  problems in sudoku saving debugged
 *       Compiler:  gcc
 *
 *         Author:  Kevin JAMET, 
 *   Organization:  SudOCRu
 *
 * ===========================================================================
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <err.h>
#include "solver.h"
#include <stdio.h>
#include <unistd.h>

// ===========================================================================
// -----------------------      Sudoku Treatment        ---------------------- 
// ===========================================================================


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

    if (sudoku == NULL) return;

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

    //in_file++;
    //Sudoku* sudoku;

    // INDEXES
    size_t index = 0;

    // VARIABLES TO READ LINES
    FILE* file = NULL;
    char* line = NULL;
    size_t length = 0;
    ssize_t line_reader;

    // CHECK IF FILE EXISTS
    file = fopen(in_file, "r");
    if (file == NULL) return NULL;

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
            len = (len <= 12? 9 : 16); 
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
                else{

                    //  --- DEBUG  ---
                    //printf("invalid char at index: %lu => '%hhu'\n", i, line[i]);
                    fclose(file);
                    if (line) free(line);
                    free(array);

                    //  --- DEBUG  ---
                    //printf("invalid char at index: %lu => '%c'\n", i, line[i]);
                    return NULL;
                }
            }
        }    
    }

    fclose(file);
    if (line) free(line);

    if (array == NULL) return NULL;
    Sudoku* imported_sodoku = CreateSudoku(array, len, len*len, len/3);

    return imported_sodoku;
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
    if (file == NULL) return 0;

    while (index < sudoku->boardsize){
        if (index != 0 && index % sudoku->boardedge == 0) fprintf(file, "\n");
        else if (index != 0 && index % 3 == 0) fprintf(file, " ");
        if (sudoku->board[index] != 0){
            fprintf(file, "%hhu", sudoku->board[index]);
        }
        else fprintf(file, ".");

        index++;
    }
    //fprintf(file, "\n ");

    fclose(file);

    return 1;
}



// ===========================================================================
// ----------------------------      Checks        --------------------------- 
// ===========================================================================

short PossibleValues(const Sudoku* sudoku, u8 index);
int clear (short n, short* flag);

/*  > IsSudokuValid
 * Check if "sudoku" grid is valid (if it's solvable)
 * > Returns 0 (false) if the board is not valid, else 1 (true)
 *      - sudoku : Sudoku grid to check
 */
int IsSudokuValid(Sudoku* sudoku){
    Sudoku* issolvable = SolveSudoku(sudoku);
    if (issolvable == NULL) return 0;
    return 1;
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
    
    while (i < sudoku->boardsize && sudoku->board[i] != 0) i++;
    if (i >= sudoku->boardsize)
    {
        return 1;
    }

    //  --- DEBUG  ---
    //printf("index: %lu", i);

    short possibilities = PossibleValues(sudoku, i);
    for (u8 n = 1; n <= 9; n++)
    {
        //  ---  DEBUG  ---
        //printf("is %hhu valid -> %i\n", possible_values, 
        //        (verify(possible_values, possibilities)));
        
        //printf("[%lu] Testing: %hhu -> %i\n", i, n, f & 1);
        if (verify(n, possibilities)){
            sudoku->board[i] = n;

            //   ---  DEBUG  ---
            //printf("\e[1;1H\e[2J");           
            //PrintBoard(sudoku);
            //sleep(1);

            if(Backtracking(sudoku, i))
            { 
                return 1;
            }
            sudoku->board[i] = 0;
        }   
        
    }    

    return 0;
}

/*  > SolveSudoku
 * Solve Sudoku using back-tracking algorithm
 * > Returns NULL if the board is not solved, else a new Sudoku grid solved
 *      - sudoku : Sudoku grid to solve
 */
Sudoku* SolveSudoku(Sudoku* sudoku){ 
    int is_solved = Backtracking(sudoku,0);
    if (is_solved)
        return sudoku;
    return NULL;
}

/*  > PrintSudoku
 * Print the sudoku board
 * > Returns *anything*
 *      - sudoku : Sudoku grid to print
 */
void PrintBoard(const Sudoku* sudoku){
    if (sudoku == NULL) return;
    
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
