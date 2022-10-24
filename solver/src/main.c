#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include "solver.h"
#include <stddef.h>

size_t mystrlen(char *s)
{
    size_t len = 0;
    for (char* p = s; *p != 0; p++) len++;
    return len;
}

int main(int argc, char** argv)
{

    printf("\
                        ===========================\n\
                        =====  SUDOKU SOLVER  =====\n\
                        ===========================\n");
    // check nb arguments
    if (argc > 4) 
        errx(EXIT_FAILURE, 
                "Error: Too many arguments (check man by \
using './solver'");
    
    else if (argc == 1){
        printf("\n\n   ---> SudokuSolver v1.0 - commands:\n\n\n\
    -> $./solver \n\
            - Load man page (this page)\n\n\
    -> $./solver inputfile \n\
            - Load a file named 'inputfile' and save solution in \
file named 'inputfile'.result (if a solution exists) \n\n\
    -> $./solver inputfile -i\n\
            - Same as previous but in interactive mode (show solver \
possibilities)\n\n\
     -> $./solver inputfile -p \n\
            - Same as previous but print the sudoku board before solving it \
(if a solution exists), solve the sudoku and print the sudoku board solved\
\n\n\n\
   -> $./solver inputfile -o output \n\
            - Load a file named 'inputfile' and save solution in \
file named 'outputfile' (if a solution exists)\n\n\n\
       Note that all -options can be used at the same time (ex : -oi)\
\n\n");
    }
    else{
        // OPTIONS
        int p = 0;
        int i = 0;
        int o = 0;

        size_t ind = 1;
        Sudoku* sudoku = NULL; 
        short options;
        for (; ind < argc; ind++){
            if (ind == 1){
                printf("\n-> Importing Sudoku...\n\n");
                sudoku = ImportSudoku(argv[1]);
                printf("===== SUDOKU IMPORTED =====\n\n");
            }
            else if (ind == 2){
                // shifting for options
                // do actions 1 by 1
                if (argv[2][0] == '-'){
                    for (int index = 1; argv[index] != 0; index++){
                        if (argv[2][index] == 'p') p = 1;
                        else if (argv[2][index] == 'i') i = 1;
                        else if (argv[2][index] == 'o') o = 1;
                    }
                    if (p == 1) PrintBoard(sudoku);
                    printf("-> Try to Solve Sudoku...\n\n");
                    SolveSudoku(sudoku);
                    printf("=====  SUDOKU SOLVED  =====\n\n");
                    if (p == 1) PrintBoard(sudoku);
                }
                else errx(EXIT_FAILURE, "Parser: Bad arguments (check man by \
using './solver')");
            }
            else if (ind == 3 && o == 1){
                printf("-> Saving Sudoku...\n\n");
                SaveSudoku(sudoku, argv[3]);
                printf("=====   SUDOKU SAVED   =====\n\n");
            }
            else 
              errx(EXIT_FAILURE, 
                "Error: Too many and/or bad arguments (check man by \
using './solver'");
        }
        
        if (ind == 2){
            if (p == 1) PrintBoard(sudoku);
            printf("-> Try to Solve Sudoku...\n\n");
            SolveSudoku(sudoku);
            printf("=====  SUDOKU SOLVED  =====\n\n");
            if (p == 1) PrintBoard(sudoku);
            printf("-> Saving Sudoku...\n\n");

            size_t len = mystrlen(argv[1]) + 8;
            char finalstring[len];

            snprintf(finalstring, len, "%s%s", argv[1], ".result");
            SaveSudoku(sudoku, finalstring);
            printf("=====   SUDOKU SAVED   =====\n\n");
        }
        //  ---  DEBUG  ---
        //PrintBoard(sudoku);

        /*  
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
        */
    }

    return 0;
}
