#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include "solver.h"
#include <stddef.h>
#include <libgen.h>

size_t mystrlen(char *s)
{
    size_t len = 0;
    for (char* p = s; *p != 0; p++) len++;
    return len;
}

int main(int argc, char** argv)
{
    puts("Squid game\n\n");
    // ------- Sudoku Man -------

    if (argc <= 1){
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

        unsigned int file_arg = 0;
        if (argc > 2){
            for (int k = 2; k < argc; k++){
                if (argv[k][0] == '-'){
                    for (int j = 1; argv[k][j] != 0; j++){
                             if (argv[k][j] == 'p') p = 1;
                        else if (argv[k][j] == 'i') i = 1;
                        else if (argv[k][j] == 'o') o = 1;
                        else if (argv[k][j] == 'd') d = 1;
                    }
                }
                else
                {
                    if (o && file_arg == 0)
                    {
                        file_arg = k;
                        continue;
                    }
                    errx(EXIT_FAILURE, "Parser: Unknown argument `%s` (check "
                        "man by using './solver')", argv[k]);
                }
            }
            if (o == 1 && file_arg == 0)
                errx(EXIT_FAILURE, "Parser: Missing output file "
                                   "(check man by using './solver')");
        }

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
            SaveSudoku(solved, argv[file_arg]);
        }
        else {
            char* file_name = basename(argv[1]);
            size_t len = mystrlen(file_name) + 8;
            char finalstring[len];

            snprintf(finalstring, len, "%s%s", file_name, ".result");
            SaveSudoku(solved, finalstring);
        }
        if (d == 1) printf("=====   SUDOKU SAVED   =====\n\n");
        DestroySudoku(sudoku);
        DestroySudoku(solved);
    }

    return 0;
}
