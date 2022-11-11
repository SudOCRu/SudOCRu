/*
 * ============================================================================
 *
 *       Filename:  tests_solver.c
 *
 *    Description: Tests for solver.c files
 *
 *        Version:  1.0
 *        Created:  10/08/22 08:34:11
 *       Revision:  added file to repo
 *       Compiler:  gcc
 *
 *         Author:  Kevin JAMET
 *   Organization:  SudOCRu
 *
 * ============================================================================
 */
#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include "solver.h"
#include <stddef.h>

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

unsigned char board1_false[] =  {
    1, 0, 0, 4, 8, 9, 0, 0, 6,
    0, 3, 7, 0, 0, 0, 0, 4, 0,
    7, 0, 0, 0, 0, 1, 2, 9, 5,
    0, 0, 7, 1, 2, 0, 6, 0, 0,
    5, 0, 0, 7, 0, 3, 0, 0, 8,
    0, 0, 6, 0, 9, 5, 7, 0, 0,
    9, 1, 4, 6, 0, 0, 0, 0, 0,
    0, 2, 0, 0, 0, 0, 0, 3, 7,
    8, 0, 0, 5, 1, 2, 0, 0, 4,
    };

unsigned char board2[] =  {
    5, 3, 0, 0, 7, 0, 0, 0, 0,
    6, 0, 0, 1, 9, 5, 0, 0, 0,
    0, 9, 8, 0, 0, 0, 0, 6, 0,
    8, 0, 0, 0, 6, 0, 0, 0, 3,
    4, 0, 0, 8, 0, 3, 0, 0, 1,
    7, 0, 0, 0, 2, 0, 0, 0, 6,
    0, 6, 0, 0, 0, 0, 2, 8, 0,
    0, 0, 0, 4, 1, 9, 0, 0, 5,
    0, 0, 0, 0, 8, 0, 0, 7, 9,
};

unsigned char board3[] =  {
    0, 2, 0, 6, 0, 8, 0, 0, 0,
    5, 8, 0, 0, 0, 9, 7, 0, 0,
    0, 0, 0, 0, 4, 0, 0, 0, 0,
    3, 7, 0, 0, 0, 0, 5, 0, 0,
    6, 0, 0, 0, 0, 0, 0, 0, 4,
    0, 0, 8, 0, 0, 0, 0, 1, 3,
    0, 0, 0, 0, 2, 0, 0, 0, 0,
    0, 0, 9, 8, 0, 0, 0, 3, 6,
    0, 0, 0, 3, 0, 6, 0, 9, 0,
};

unsigned char board4[] =  {
    4, 9, 0, 0, 0, 0, 0, 2, 0,
    0, 0, 8, 0, 0, 6, 0, 0, 4,
    0, 0, 0, 0, 0, 0, 1, 6, 9,
    0, 0, 0, 4, 0, 5, 0, 0, 7,
    0, 8, 5, 0, 6, 0, 4, 0, 0,
    3, 0, 0, 0, 2, 0, 0, 1, 0,
    0, 0, 0, 6, 7, 0, 9, 0, 0,
    0, 0, 9, 0, 3, 1, 0, 7, 0,
    8, 0, 0, 0, 0, 0, 0, 0, 1,
};

unsigned char board5[] =  {
    8, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 3, 6, 0, 0, 0, 0, 0,
    0, 7, 0, 0, 9, 0, 2, 0, 0,
    0, 5, 0, 0, 0, 7, 0, 0, 0,
    0, 0, 0, 0, 4, 5, 7, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 3, 0,
    0, 0, 1, 0, 0, 0, 0, 6, 8,
    0, 0, 8, 5, 0, 0, 0, 1, 0,
    0, 9, 0, 0, 0, 0, 4, 0, 0,
};

unsigned char board6[] =  {
    8, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 6, 0, 0, 0, 0, 3,
    0, 7, 4, 0, 8, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 3, 0, 0, 2,
    0, 8, 0, 0, 4, 0, 0, 1, 0,
    6, 0, 0, 5, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 7, 8, 0,
    5, 0, 0, 0, 0, 9, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 4, 0,
};

unsigned char hardest_board[] =
{
    9, 0, 0, 8, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 0, 0, 1, 0, 0, 0, 3,
    0, 1, 0, 0, 0, 0, 0, 6, 0,
    0, 0, 0, 4, 0, 0, 0, 7, 0,
    7, 0, 8, 6, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 3, 0, 1, 0, 0,
    4, 0, 0, 0, 0, 0, 2, 0, 0
};

// ================= SOLUTIONS =====================

unsigned char board1_s[] =  {
    1, 5, 2, 4, 8, 9, 3, 7, 6,
    7, 3, 9, 2, 5, 6, 8, 4, 1,
    4, 6, 8, 3, 7, 1, 2, 9, 5,
    3, 8, 7, 1, 2, 5, 6, 5, 9,
    5, 9, 1, 7, 6, 3, 4, 2, 8,
    2, 4, 6, 8, 9, 5, 7, 1, 3,
    9, 1, 4, 6, 3, 7, 5, 8, 2,
    6, 2, 5, 9, 4, 8, 1, 3, 7,
    8, 7, 3, 5, 1, 2, 9, 6, 4,
};

unsigned char board2_s[] =  {
    5, 3, 4, 6, 7, 8, 9, 1, 2,
    6, 7, 2, 1, 9, 5, 3, 4, 8,
    1, 9, 8, 3, 4, 2, 5, 6, 7,
    8, 5, 9, 7, 6, 1, 4, 2, 3,
    4, 2, 6, 8, 5, 3, 7, 9, 1,
    7, 1, 3, 9, 2, 4, 8, 5, 6,
    9, 6, 1, 5, 3, 7, 2, 8, 4,
    2, 8, 7, 4, 1, 9, 6, 3, 5,
    3, 4, 5, 2, 8, 6, 1, 7, 9,
};

unsigned char board3_s[] =  {
    1, 2, 3, 6, 7, 8, 9, 4, 5,
    5, 8, 4, 2, 3, 9, 7, 6, 1,
    9, 6, 7, 1, 4, 5, 3, 2, 8,
    3, 7, 2, 4, 6, 1, 5, 8, 9,
    6, 9, 1, 5, 8, 3, 2, 7, 4,
    4, 5, 8, 7, 9, 2, 6, 1, 3,
    8, 3, 6, 9, 2, 4, 1, 5, 7,
    2, 1, 9, 8, 5, 7, 4, 3, 6,
    7, 4, 5, 3, 1, 6, 8, 9, 2,
};

unsigned char board4_s[] =  {
    4, 9, 6, 1, 5, 7, 8, 2, 3,
    2, 1, 8, 3, 9, 6, 7, 5, 4,
    7, 5, 3, 2, 8, 4, 1, 6, 9,
    9, 6, 2, 4, 1, 5, 3, 8, 7,
    1, 8, 5, 7, 6, 3, 4, 9, 2,
    3, 7, 4, 9, 2, 8, 5, 1, 6,
    5, 3, 1, 6, 7, 2, 9, 4, 8,
    6, 4, 9, 8, 3, 1, 2, 7, 5,
    8, 2, 7, 5, 4, 9, 6, 3, 1,
};

unsigned char board5_s[] =  {
    8, 1, 2, 7, 5, 3, 6, 4, 9,
    9, 4, 3, 6, 8, 2, 1, 7, 5,
    6, 7, 5, 4, 9, 1, 2, 8, 3,
    1, 5, 4, 2, 3, 7, 8, 9, 6,
    3, 6, 9, 8, 4, 5, 7, 2, 1,
    2, 8, 7, 1, 6, 9, 5, 3, 4,
    5, 2, 1, 9, 7, 4, 3, 6, 8,
    4, 3, 8, 5, 2, 6, 9, 1, 7,
    7, 9, 6, 3, 1, 8, 4, 5, 2,
};

unsigned char board6_s[] =  {
    1, 2, 6, 4, 3, 7, 9, 5, 8,
    8, 9, 5, 6, 2, 1, 4, 7, 3,
    3, 7, 4, 9, 8, 5, 1, 2, 6,
    4, 5, 7, 1, 9, 3, 8, 6, 2,
    9, 8, 3, 2, 4, 6, 5, 1, 7,
    6, 1, 2, 5, 7, 8, 3, 9, 4,
    2, 6, 9, 3, 1, 4, 7, 8, 5,
    5, 4, 8, 7, 6, 9, 2, 3, 1,
    7, 3, 1, 8, 5, 2, 6, 4, 9,
};

int is_solution(const unsigned char* board, const Sudoku* sudoku){
    u8 i = 0;
    while (board[i] != 0){
        if (board[i] != sudoku->board[i]) return 1;
    }
    return 0;
}

int main2()
{
    /*
    Sudoku* hardest = CreateSudoku(hardest_board, 9, 81, 3);
    SaveSudoku(hardest, "grid_xx");
    */
    /*
    Sudoku* sudoku5_save = CreateSudoku(board6, 9, 81, 3);
    SaveSudoku(sudoku5_save, "grid_05");

    Sudoku* sudoku4_save = CreateSudoku(board4, 9, 81, 3);
    SaveSudoku(sudoku4_save, "grid_04");

    Sudoku* sudoku3_save = CreateSudoku(board3, 9, 81, 3);
    SaveSudoku(sudoku3_save, "grid_03");

    Sudoku* sudoku2_save = CreateSudoku(board2, 9, 81, 3);
    SaveSudoku(sudoku2_save, "grid_02");

    Sudoku* sudoku1_save = CreateSudoku(board1, 9, 81, 3);
    SaveSudoku(sudoku1_save, "grid_01");
    */

    /*
    Sudoku* sudoku6 = ImportSudoku("grid_06");

    //printf("IsSudokuValid : %i\n", IsSudokuValid(imported));
    printf("IsSudokuSolved : %i\n", IsSudokuSolved(sudoku6));
    PrintBoard(sudoku6);
    printf("\n");

    SolveSudoku(sudoku6);

    //printf("IsSudokuValid : %i\n", IsSudokuValid(imported));
    printf("IsSudokuSolved : %i\n", IsSudokuSolved(sudoku6));
    PrintBoard(sudoku6);
    printf("\n");

    SaveSudoku(sudoku6, "lasolution.result");
    DestroySudoku(sudoku6);
    */


    /*
    Sudoku* imported = ImportSudoku("grid_00");

    //printf("IsSudokuValid : %i\n", IsSudokuValid(imported));
    printf("IsSudokuSolved : %i\n", IsSudokuSolved(imported));
    PrintBoard(imported);
    printf("\n");

    SolveSudoku(imported);

    //printf("IsSudokuValid : %i\n", IsSudokuValid(imported));
    printf("IsSudokuSolved : %i\n", IsSudokuSolved(imported));
    PrintBoard(imported);
    printf("\n");

    SaveSudoku(imported, "lasolution.result");
    DestroySudoku(imported);
    */


    // HARDEST SUDOKU

    /*
    Sudoku* hardest = CreateSudoku(hardest_board, 9, 81, 3);
    PrintBoard(hardest);
    printf("\n");

    SolveSudoku(hardest);

    printf("IsSudokuSolved : %i\n", IsSudokuSolved(hardest));
    PrintBoard(hardest);
    printf("\n");

    SaveSudoku(hardest, "leplusdur.result");
    //DestroySudoku(hardest);
    */


    //Sudoku* false_sudoku = CreateSudoku(board1_false, 9, 9);
    //printf("IsSudokuValid (false) : %i\n", IsSudokuValid(false_sudoku));
    //DestroySudoku(false_sudoku);


    /*
    Sudoku* imported_solved = ImportSudoku("grid_00.result");
    PrintBoard(imported_solved);
    DestroySudoku(imported_solved);
    */

    /*
    Sudoku* sudoku = CreateSudoku(board1, 9, 9);
    PrintBoard(sudoku);

    Sudoku* sudoku_solved = CreateSudoku(board1, 9, 9);
    PrintBoard(sudoku_solved);

    Sudoku* sudoku_1_false = CreateSudoku(board1_false, 9, 9);
    PrintBoard(sudoku_1_false);

    // -------------------------------------------------------------


    int issolved = IsSudokuSolved(sudoku);
    printf("IsSolved(sudoku) = %i\n", issolved);

    int issolved_solved = IsSudokuSolved(sudoku_solved);
    printf("IsSolved(sudoku_solved) = %i\n", issolved_solved);

    int issolved_1 = IsSudokuSolved(sudoku_solved);
    printf("IsSolved(sudoku_false) = %i\n", issolved_1);

    // -------------------------------------------------------------

    int isvalid = IsSudokuValid(sudoku);
    printf("IsValid(sudoku) = %i\n", isvalid);

    int isvalid_solved = IsSudokuValid(sudoku_solved);
    printf("IsValid(sudoku_solved) = %i\n", isvalid_solved);

    int isvalid_false = IsSudokuValid(sudoku_1_false);
    printf("IsValid(sudoku_false) = %i\n", isvalid_false);

    // -------------------------------------------------------------

    printf("======================================\n");


    Sudoku* sudoku_2 = CreateSudoku(board6, 9, 9);
    PrintBoard(sudoku_2);

    Sudoku* sudoku_s = SolveSudoku(sudoku_2);

    if (sudoku_s == NULL) // TODO in SolveSudoku
        errx(EXIT_FAILURE, "SudokuSolver : NOT SOLVED\n");
    else
    {
        PrintBoard(sudoku_s);
        printf("SudokuSolver : Successfully Solved\n");
    }
    return 0;



    // Import Sudoku from file
    //Sodoku* sudoku = ImportSudoku(argv[1]);

    */

    /*
    if (sudoku->board == NULL)
        errx(EXIT_FAILURE, "Error: Sudoku can't be loaded");

    for (int i = 0; i < sudoku->cols*sudoku->rows; i++){
        if (sudoku->board[i] != board1_s[i])
            printf("index : %i is different : should be %u8 but it is %u8\n",
                i, board1_s[i], sudoku->board[i]);
    }
    */

    //if (!IsSudokuValid(sudoku))
    //    errx(EXIT_FAILURE, "Error: Sudoku can't be loaded (not solvable)");

    //Sudoku* sudoku_solved =
    //    (!IsSudokuSolved(sudoku) ? SolveSudoku(sudoku) : sudoku);

    // Check if Sudoku is solved
    /*
    if (sudoku_solved == NULL) // TODO in SolveSudoku
        errx(EXIT_FAILURE, "SudokuSolver : NOT SOLVED\n");
    else
        printf("SudokuSolver : Successfully Solved\n");
    return 0;
    */
    return 0;
}
