#pragma once

struct Sudoku {
    char* board;
    size_t cols;
    size_t rows;
}

/*
 * Initializes a new Sudoku board.
 */
Sudoku* CreateSudoku(char* array, size_t cols, size_t rows);

/*
 * Destroys of the provided Sudoku board.
 */
void DestroySudoku(Sudoku* board);

/*
 * Import a Sudoku board from a given file using the EPITA format (9x9). This
 * function returns NULL if the board couldn't be loaded or the file does not
 * exist.
 */
Sodoku* ImportSudoku(char* in_file);
/*
 * Save a Sudoku board to a given file using the EPITA format (9x9). This 
 * function returns 0 (false) if the board couldn't be saved.
 */
int SaveSudoku(const Sudoku* sudoku, char* out_file);

int IsSudokuValid(const Sudoku* sudoku);
int IsSudokuSolved(const Sudoku* sudoku);
Sudoku* SolveSudoku(const Sudoku* sudoku);
