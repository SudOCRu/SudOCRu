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
    u8 nbsquares;
} Sudoku;

/*  > CreateSudoku
 * Initializes a new Sudoku board.
 * > Returns a new Sudoku (cols*rows) from array
 *      - array : Array to create Sudoku
 *      - cols : Number of cols of Sudoku board
 *      - cols : Number of rows of Sudoku board
 */
Sudoku* CreateSudoku(const u8* array, u8 boardedge, u8 nbsquares);

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
Sudoku* ImportSudoku(const char* in_file);

/*  > SaveSudoku
 * Save a Sudoku board to a given file using the EPITA format (9x9). This
 * function returns
 * > Returns 0 (false) if the board couldn't be saved, else 1 (true)
 *      - sudoku : Sudoku to save into file
 *      - out_file : Name of saved file that contains Sudoku grid
 */
int SaveSudoku(const Sudoku* sudoku, const char* out_file);

/*  > IsSudokuValid
 * Check if "sudoku" grid is valid (if it's solvable)
 * > Returns 0 (false) if the board is not valid, else 1 (true)
 *      - sudoku : Sudoku grid to check
 */
int IsSudokuValid(const Sudoku* sudoku);

/*  > IsSudokuSolved
 * Check if "sudoku" grid is solved (if the grid is filled of numbers != 0)
 * > Returns 0 (false) if the board not solved, else 1 (true)
 *      - sudoku : Sudoku grid to check
 */
int IsSudokuSolved(const Sudoku* sudoku);

/*  > PossibleValues
 * Check all possible moves on a specific cell
 * > Returns a bitmask of values than can be played
 *      - sudoku : Sudoku grid to check
 *      - index : Cell to run checks on
 */
short PossibleValues(const Sudoku* sudoku, u8 index);

/*  > Backtracking
 * Recursion of solve using backtracking algo. For each box box in the sudoku
 * grid, it will test every solution possible until it tested all solutions.
 * Sudoku in ref is modified to return the sudoku solved
 * > Returns 0 (false) if it was not able to find a solution, else 1 (true)
 *      - sudoku : sudoku pointer to modify
 */
int Backtracking(Sudoku* sudoku, size_t i);

/*  > InteractiveSolveSudoku
 * Solve Sudoku using back-tracking algorithm and print each step
 * > Returns NULL if the board is not solved, else a new Sudoku grid solved
 *      - sudoku : Sudoku grid to solve
 */
int IntBacktracking(const Sudoku* orginal, Sudoku* sudoku, size_t i);

/*  > SolveSudoku
 * Solve Sudoku using back-tracking algorithm
 * > Returns NULL if the board is not solved, else a new Sudoku grid solved
 *      - sudoku : Sudoku grid to solve
 */
Sudoku* SolveSudoku(const Sudoku* sudoku, int interactive);

/*  > PrintSudoku
 * Print the sudoku board
 * > Returns *anything*
 *      - sudoku : Sudoku grid to print
 */
void PrintBoard(const Sudoku* sudoku);

/*  > PrintBoardCol
 * Print the sudoku board with colors
 * > Returns *anything*
 *      - org : Original sudoku grid
 *      - sudoku : Sudoku grid to print
 */
void PrintBoardCol(const Sudoku* org, const Sudoku* sudoku);
