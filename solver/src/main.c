#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include "solver.h"
#include <stddef.h>

int main(int argc, char** argv)
{
    // check nb arguments
    if (argc > 2) 
        errx(EXIT_FAILURE, "Error: Too many arguments (2 arguments only)");
    
    // Sudoku pre-loaded for tests
    unsigned char board1[] =  {
    1, 0, 0, 4, 8, 9, 0, 0, 6,
    7, 3, 0, 0, 0, 0, 0, 4, 0,
    0, 0, 0, 0, 0, 1, 2, 9, 5,
    0, 0, 7, 1, 2, 0, 6, 0, 0,
    5, 0, 0, 7, 0, 3, 0, 0, 8,
    0, 0, 6, 0, 9, 5, 7, 0, 0,
    9, 1, 4, 6, 0, 0, 0, 0, 0,
    0, 2, 0, 0, 0, 0, 0, 3, 7,
    8, 0, 0, 5, 1, 2, 0, 0, 4,
};

    // CreateSudoku from test char[] previsouly added
    Sudoku* sudoku = CreateSudoku(board1, 9, 9);

    // Import Sudoku from file
    //Sodoku* sudoku = ImportSudoku(argv[1]);

    if (sudoku == NULL) 
        errx(EXIT_FAILURE, "Error: Sudoku can't be loaded");
    if (!IsSudokuValid(sudoku)) 
        errx(EXIT_FAILURE, "Error: Sudoku can't be loaded (not solvable)");

    Sudoku* sudoku_solved = 
        (!IsSudokuSolved(sudoku) ? SolveSudoku(sudoku) : sudoku);
    
    // Check if Sudoku is solved
    if (sudoku_solved == NULL) // TODO in SolveSudoku
        errx(EXIT_FAILURE, "SudokuSolver : NOT SOLVED\n");
    else
        printf("SudokuSolver : Successfully Solved\n");
        
    // Save Sudoku into file
    if (!SaveSudoku(sudoku, "SudokuSolved"))
        errx(EXIT_FAILURE, "Error: Sudoku can't be saved");

    // destroy -> struct YT (how to free memory)

    return 0;
}
