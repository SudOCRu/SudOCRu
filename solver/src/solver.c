/*
 * ===========================================================================
 *
 *       Filename:  solver.c
 *
 *    Description: Functions used to solve Sudokus 
 *
 *        Version:  0.0.4
 *        Created:  10/01/22 16:12:22
 *       Revision:  almost finished solve functions
 *       Compiler:  gcc
 *
 *         Author:  Kevin JAMET, 
 *   Organization:  SudOCRu
 *
 * ===========================================================================
 */

#include <stdlib.h>
#include <err.h>
#include "solver.h"
#include "stdio.h"
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
Sudoku* CreateSudoku(u8* array, u8 cols, u8 rows){

    Sudoku* sudoku = malloc(sizeof(Sudoku));
    sudoku->board = array; 
    sudoku->cols = cols;
    sudoku->rows = rows;
    
    u8 len = rows*cols;
    u8 index = 0;

    while (index < len){
        sudoku->board[index] = array[index];
        index++;
    }

    return sudoku;
}

/*  > DestroySudoku
 * Destroys of the provided Sudoku board.
 *      - board : Board to destroy
 */
void DestroySudoku(Sudoku* board){

    if (board == NULL) return;

    free(board->board);
    free(board);        
}

/*  > ImportSudoku
 * Import a Sudoku board from a given file using the EPITA format (9x9). This
 * function returns NULL if the board couldn't be loaded or the file does not
 * exist.
 * > Returns a Sudoku board loaded from file "in_file"
 *      - in_file : Path of file to create the new Sudoku grid
 */
Sudoku* ImportSudoku(char* in_file){

    in_file++;
    // TODO 
    //Sudoku* sudoku;

    return NULL;
}

/*  > SaveSudoku
 * Save a Sudoku board to a given file using the EPITA format (9x9). This 
 * function returns 
 * > Returns 0 (false) if the board couldn't be saved, else 1 (true)
 *      - sudoku : Sudoku to save into file
 *      - out_file : Name of saved file that contains Sudoku grid
 */
int SaveSudoku(const Sudoku* sudoku, char* out_file){

    // TODO
    sudoku++;
    out_file++;
    
    return 1;
}



// ===========================================================================
// ----------------------------      Checks        --------------------------- 
// ===========================================================================

/*  > BoxCheck
 * Check if one number of the "sudoku" grid is valid (if it's in the right 
 * place)
 * > Returns 0 (false) if the number is not at a valid position, else 1 (true)
 *      - sudoku : Sudoku grid to check
 *      - index : 
 */
int BoxCheck(const Sudoku* sudoku, u8 index){

    // TODO
    
    for (u8 i = index%sudoku->rows; i <= index+3*(sudoku->cols); 
            i += sudoku->cols){
        //for (u8 j = index/cols; // TODO finsih function using indexes
    }

    return 1;
}

/*  > IsSudokuValid
 * Check if "sudoku" grid is valid (if it's solvable)
 * > Returns 0 (false) if the board is not valid, else 1 (true)
 *      - sudoku : Sudoku grid to check
 */
int IsSudokuValid(const Sudoku* sudoku){

    // TODO : check if correct
    //
    // for i in range len sudoku
    //      if (!(check column && check row && check square)) return 0;
    
    u8 len = sudoku->rows*sudoku->cols;
    u8 index = 0;
    while (index < len){
        if (!BoxCheck(sudoku, index)) return 0;
        index++;
    }

    return 1;
}

/*  > IsSudokuSolved
 * Check if "sudoku" grid is solved (if the grid is filled of numbers != 0)
 * > Returns 0 (false) if the board not solved, else 1 (true)
 *      - sudoku : Sudoku grid to check
 */
int IsSudokuSolved(const Sudoku* sudoku){

    // TODO
    
    //if (!IsSudokuValid) return 0;
    short len = sudoku->cols * sudoku->rows;
    short index = 0;

    while(index < len && sudoku->board[index] != 0) index++;

    return index == len;
}

int verify(short n, short flag){
    short mark = 1 << (n-1);
    return ((flag & mark) != 0) ? 1 : 0;
}

short set (short n, short flag){
    short mark = 1 << (n-1);
    return flag | mark;
}

void clear (short n, short* flag){
    short mark = 1 << (n-1);
    *flag = *flag & ~mark;
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
    u8 row = index / 9;
    u8 col = index % 9;
    short possibilities = 0b111111111;
    size_t idx;
    u8 cell;

    for(size_t i = 0; i < 9; i++)
    {
        idx = row * 9 + i;
        cell = sudoku->board[idx];
        if (cell != 0)
        {
            clear(cell, &possibilities);
        }
    }
    if (possibilities == 0) return possibilities;

    for(size_t i = 0; i < 9; i++)
    {
        idx = i * 9 + col;
        cell = sudoku->board[idx];
        if (cell != 0)
        {
            clear(cell, &possibilities);
        }
    }

    if (possibilities == 0) return possibilities;

     
    size_t vgroups = 9 / 3;
    size_t hgroups = 9 / 3;
    size_t init_row = (row/hgroups)*hgroups;
    size_t init_col = (col/vgroups)*vgroups;
    for (size_t i = 0; i < 3; i++){
        for (size_t j = 0; j < 3; j++){
            idx = (init_row + i)  * 9 + (j + init_col) ;
            cell = sudoku->board[idx];
            if (cell != 0){
                clear(cell, &possibilities);
            }
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
    //u8 len = sudoku->cols * sudoku->rows; 
    u8 len = 81;
    
    // TODO: boucle while pour chercher
    while (i < len && sudoku->board[i] != 0) i++;
    if (i >= len)
    {
        return 1;
    }


    //printf("index: %lu", i);
    short possibilities = PossibleValues(sudoku, i);
    for (u8 possible_values = 1; possible_values <= 9; 
            possible_values++)
    {
        //printf("is %hhu valid -> %i\n", possible_values, 
        //        (verify(possible_values, possibilities)));
        
        if (verify(possible_values, possibilities)){
            sudoku->board[i] = possible_values; 

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

    /*
    while (index < len){
        // TODO if 0 :
        if (sudoku->board[index] != 0){
            possibilities = PossibleValues(sudoku);
            while (possibilities < 1 && !found_solution){
                
                sudoku->board[index] = possibilities >> 1; // TODO check if correct
                found_solution = Backtracking(sudoku);
                                
                if (found_solution) return 1;
                else{
                    sudoku->board[index] = 0; // TODO check if necessary
                }
            }
        }
        if (found_solution) {
            index--;
        }
        else index++;
        // if another number
    }
    */
}

/*  > SolveSudoku
 * Solve Sudoku using back-tracking algorithm
 * > Returns NULL if the board is not solved, else a new Sudoku grid solved
 *      - sudoku : Sudoku grid to solve
 */
Sudoku* SolveSudoku(Sudoku* sudoku){ // TODO return int ?? const ????
    //Sudoku* sudoku_solved; // TODO : use createsudoku function

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
    // print first edge of square
    for (size_t col = 0; col < sudoku->cols; col++){
            printf(" ---");
    }
    printf("\n");

    for (size_t row = 0; row < sudoku->rows; row++){
        printf("|");
        for (size_t col = 0; col < sudoku->cols; col++){
            printf(" %hhu |", sudoku->board[row*sudoku->rows + col]);
        }
        printf("\n");
        for (size_t col = 0; col < sudoku->cols; col++){
            printf(" ---");
        }
        printf("\n");
    }
}
