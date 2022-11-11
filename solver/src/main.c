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
    // --- Check nb arguments ---
    if (argc > 4)
        errx(EXIT_FAILURE, "Error: Too many arguments (check man by \
using './solver'");

    // ------- Sudoku Man -------

    else if (argc == 1){
        printf("\n\n   ---> SudokuSolver v1.0 - commands:\n\n\n\
    -> $./solver \n\
            - Load man page (this page)\n\n\
    -> $./solver inputfile \n\
            - Load a file named 'inputfile' and save solution in \
file named 'inputfile'.result (if a solution exists) \n\n\
    -> $./solver inputfile -d\n\
            - Same as previous but in debugging mode (show steps)\n\n\
    -> $./solver inputfile -s\n\
            - Same as previous but in step by step mode (show solver \
possibilities)\n\n\
    -> $./solver inputfile -i\n\
            - Same as previous but in (interactive) live mode (show solver \
possibilities)\n\n\
    -> $./solver inputfile -p \n\
            - Same as previous but print the sudoku board before solving it \
(if a solution exists), solve the sudoku and print the sudoku board solved\
\n\n\
    -> $./solver inputfile -o output \n\
            - Load a file named 'inputfile' and save solution in \
file named 'outputfile' (if a solution exists), if no output file mentionned \
returns an error\n\n\n\
       Note that all -options can be used at the same time (ex : -oi)\
\n\n");
    }
    //---------------------------

    else {
        // --- OPTIONS ---
        int p = 0;
        int i = 0;
        int o = 0;
        int d = 0;

        Sudoku* sudoku = NULL;

        // --- Check options mentionned in args ---

        if (argc > 2){
            if (argv[2][0] == '-'){
                for (int index = 1; argv[index] != 0; index++){
                    if (argv[2][index] == 'p') p = 1;
                    else if (argv[2][index] == 'i') i = 1;
                    else if (argv[2][index] == 'o') o = 1;
                    else if (argv[2][index] == 'd') d = 1;
                }
            }
            else errx(EXIT_FAILURE, "Parser: Bad arguments (check man by \
using './solver')");
        }
        if (argc == 3 && o == 1)
            errx(EXIT_FAILURE, "Parser: Bad arguments, missing output file \
name (check man by using './solver')");

        // ---------------------------------------


        // --- Import sudoku ---

        if (d == 1) printf("\n-> Importing Sudoku...\n\n");
        sudoku = ImportSudoku(argv[1]);
        if (d == 1) printf("===== SUDOKU IMPORTED =====\n\n");

        // ---------------------

        // Solve and print sudoku

        if (p == 1) PrintBoard(sudoku);
        if (d == 1) printf("-> Try to Solve Sudoku...\n\n");
        Sudoku* solved = SolveSudoku(sudoku, i);
        if (d == 1) printf("=====  SUDOKU SOLVED  =====\n\n");
        if (p == 1) PrintBoardCol(sudoku, solved);

        // ---------------------

        // ---- Save sudoku ----

        if (d == 1) printf("-> Saving Sudoku...\n\n");

        if (o == 1) {
            SaveSudoku(solved, argv[3]);
        }
        else {
            size_t len = mystrlen(argv[1]) + 8;
            char finalstring[len];

            snprintf(finalstring, len, "%s%s", argv[1], ".result");
            SaveSudoku(solved, finalstring);
        }
        if (d == 1) printf("=====   SUDOKU SAVED   =====\n\n");
        DestroySudoku(sudoku);
        DestroySudoku(solved);
    }

    return 0;
}
