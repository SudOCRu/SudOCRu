#pragma once

typedef unsigned char u8;

/*  > Sudoku
 *      - board : Sudoku board filled of 0 (empty box) and numbers from 1 to 9
 *      - cols : Number of columns in grid
 *      - rows : Number of rows in grid
 */
typedef struct Sudoku {
    u8* board;
    u8 boardedge;
    u8 boardsize;
} Sudoku;

/*  > CreateSudoku
 * Initializes a new Sudoku board. 
 * > Returns a new Sudoku (cols*rows) from array
 *      - array : Array to create Sudoku
 *      - cols : Number of cols of Sudoku board
 *      - cols : Number of rows of Sudoku board
 */
Sudoku* CreateSudoku(u8* array, u8 boardedge, u8 boardsize);

/*  > DestroySudoku
 * Destroys of the provided Sudoku board.
 *      - board : Board to destroy
 */
void DestroySudoku(Sudoku* board);

/*  > ImportSudoku
 * Import a Sudoku board from a given file using the EPITA format (9x9). This
 * function returns NULL if the board couldn't be loaded or the file does not
 * exist.
 * > Returns a Sudoku board loaded from file "in_file"
 *      - in_file : Path of file to create the new Sudoku grid
 */
Sudoku* ImportSudoku(char* in_file);

/*  > SaveSudoku
 * Save a Sudoku board to a given file using the EPITA format (9x9). This 
 * function returns 
 * > Returns 0 (false) if the board couldn't be saved, else 1 (true)
 *      - sudoku : Sudoku to save into file
 *      - out_file : Name of saved file that contains Sudoku grid
 */
int SaveSudoku(const Sudoku* sudoku, char* out_file);

/*  > IsSudokuValid
 * Check if "sudoku" grid is valid (if it's solvable)
 * > Returns 0 (false) if the board is not valid, else 1 (true)
 *      - sudoku : Sudoku grid to check
 */
int IsSudokuValid(Sudoku* sudoku);

/*  > IsSudokuSolved
 * Check if "sudoku" grid is solved (if the grid is filled of numbers != 0)
 * > Returns 0 (false) if the board not solved, else 1 (true)
 *      - sudoku : Sudoku grid to check
 */
int IsSudokuSolved(const Sudoku* sudoku);

/*  > SolveSudoku
 * Solve Sudoku using back-tracking algorithm
 * > Returns NULL if the board is not solved, else a new Sudoku grid solved
 *      - sudoku : Sudoku grid to solve
 */
Sudoku* SolveSudoku(Sudoku* sudoku);

/*  > PrintSudoku
 * Print the sudoku board
 * > Returns *anything*
 *      - sudoku : Sudoku grid to print
 */
void PrintBoard(const Sudoku* sudoku);
