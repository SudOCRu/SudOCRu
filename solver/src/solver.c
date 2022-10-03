/*
 * ===========================================================================
 *
 *       Filename:  solver.c
 *
 *    Description: Functions used to solve Sudokus 
 *
 *        Version:  0.0.2
 *        Created:  10/01/22 16:12:22
 *       Revision:  began basic functions
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

    // TODO : tests
    u8 len = rows*cols;
    u8 index = 0;
    Sudoku* sudoku[rows*cols] = {};
    while (index < len){
        sudoku[index] = array[index];
        index++;
    }

    return sudoku;
}

/*  > DestroySudoku
 * Destroys of the provided Sudoku board.
 *      - board : Board to destroy
 */
void DestroySudoku(Sudoku* board){

    // TODO

}

/*  > ImportSudoku
 * Import a Sudoku board from a given file using the EPITA format (9x9). This
 * function returns NULL if the board couldn't be loaded or the file does not
 * exist.
 * > Returns a Sudoku board loaded from file "in_file"
 *      - in_file : Path of file to create the new Sudoku grid
 */
Sudoku* ImportSudoku(char* in_file){

    // TODO
    
    Sudoku* sudoku;
    return sudoku;
}

/*  > SaveSudoku
 * Save a Sudoku board to a given file using the EPITA format (9x9). This 
 * function returns 
 * > Returns 0 (false) if the board couldn't be saved, else 1 (true)
 *      - sudoku : Sudoku to save into file
 *      - out_file : Name of saved file that contains Sudoku grid
 */
u8 SaveSudoku(const Sudoku* sudoku, char* out_file){

    // TODO
    
    return 1;
}



// ===========================================================================
// ----------------------------      Checks        --------------------------- 
// ===========================================================================

// TODO : checkcolumn
// TODO : checkrow
// TODO : checksquare

/*  > SudokuCheck
 * Check if one number of the "sudoku" grid is valid (if it's in the right 
 * place)
 * > Returns 0 (false) if the number is not at a valid position, else 1 (true)
 *      - sudoku : Sudoku grid to check
 */
u8 SudokuCheck(const Sudoku* sudoku){

    // TODO
    
    return 1;
}

/*  > IsSudokuValid
 * Check if "sudoku" grid is valid (if it's solvable)
 * > Returns 0 (false) if the board is not valid, else 1 (true)
 *      - sudoku : Sudoku grid to check
 */
u8 IsSudokuValid(const Sudoku* sudoku){

    // TODO : check if correct
    //
    // for i in range len sudoku
    //      if (!(check column && check row && check square)) return 0;
    
    u8 len = rows*cols;
    u8 index = 0;
    while (index < len){
        if (!SudokuCheck(sudoku)) return 0;
    }

    return 1;
}

/*  > IsSudokuSolved
 * Check if "sudoku" grid is solved (if the grid is filled of numbers != 0)
 * > Returns 0 (false) if the board not solved, else 1 (true)
 *      - sudoku : Sudoku grid to check
 */
u8 IsSudokuSolved(const Sudoku* sudoku){

    // TODO
    
    if (!IsSudokuValid) return 0;
    short len = sudoku.cols * sudoku.rows;
    short index = 0;

    while(index < len && sudoku[i] != 0) i++;

    return index == len;
}



// ===========================================================================
// ----------------------------      SOLVE        ---------------------------- 
// ===========================================================================

/*  > PossibleValues
 * Returns the numbers that are possible to put in the gris at (x, y) 
 * positions (represented in binary representation)
 * > Returns the possibilites in power of two added to the number
 *      - sudoku : Sudoku grid to check
 */
short PossibleValues(const Sudoku* sudoku){

    // possibilities are in binary representation and are power of 2 numbers
    //  ex : if 9 is possible : 2^9 is added to possibilities
    short possibilities = 0;
    
    return possibilities;
}

/*  > SolveSudoku
 * Solve Sudoku using back-tracking algorithm
 * > Returns NULL if the board is not solved, else a new Sudoku grid solved
 *      - sudoku : Sudoku grid to solve
 */
Sudoku* SolveSudoku(const Sudoku* sudoku){

    // TODO
    
    Sudoku* sudoku_solved;
    return sudoku_solved;
}
