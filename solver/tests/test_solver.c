#ifdef CRIT_TEST
#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/parameterized.h>
#include <criterion/redirect.h>
#include "solver.h"
#include "boards.h"

void redirect_io()
{
    cr_redirect_stdout();
    cr_redirect_stderr();
}

void cleanup_params(struct criterion_test_params *crp) {
    for (size_t i = 0; i < crp->length; i++) {
        cr_free(((u8**) crp->params)[i]);
    }
    cr_free(crp->params);
}

u8* make_sd_param(const u8* org)
{
    u8* m = cr_calloc(81, sizeof(u8));
    memcpy(m, org, 81 * sizeof(u8));
    return m;
}

Test(solver, create) {
    u8* arr = calloc(81, sizeof(u8));
    Sudoku* sd = CreateSudoku(arr, 9, 3);
    cr_assert(eq(u8, sd->boardedge, 9),
            "The number of cells per side must be 9");
    cr_assert(eq(u8, sd->boardsize, 9 * 9),
            "The total number of cells must be 81");
    cr_assert(eq(u8, sd->nbsquares, 3), "The number of sub-groups must be 3");
    cr_assert(ne(ptr, sd->board, arr), 
            "The sudoku board should not have any ref to the original array");
    DestroySudoku(sd);
}

Test(solver, boardNotValid, .init = redirect_io) {
    Sudoku* sd = CreateSudoku(TEST_INVALID, 9, 3);
    cr_assert(ne(i32, IsSudokuValid(sd), 1),
            "An invalid sudoku was marked valid");
    DestroySudoku(sd);
}

ParameterizedTestParameters(solver, boardValid) {
    const size_t size = 4;
    u8** params = cr_calloc(size, sizeof(u8*));

    params[0] = make_sd_param(TEST_BOARD1);
    params[1] = make_sd_param(TEST_BOARD2);
    params[2] = make_sd_param(TEST_BOARD5_S);
    params[3] = make_sd_param(TEST_BOARD6_S);

    return cr_make_param_array(u8*, params, size, cleanup_params);
}

ParameterizedTest(u8** board, solver, boardValid) {
    Sudoku* sd = CreateSudoku(*board, 9, 3);
    cr_assert(eq(i32, IsSudokuValid(sd), 1),
            "A valid board was marked as invalid");
    DestroySudoku(sd);
}

Test(solver, boardNotSolved, .init = redirect_io) {
    Sudoku* sd = CreateSudoku(TEST_BOARD3, 9, 3);
    cr_assert(ne(i32, IsSudokuSolved(sd), 1),
            "An non-solved sudoku was marked solved");
    DestroySudoku(sd);
}

ParameterizedTestParameters(solver, isSolved) {
    const size_t size = 6;
    u8** params = cr_calloc(size, sizeof(u8*));

    params[0] = make_sd_param(TEST_BOARD1_S);
    params[1] = make_sd_param(TEST_BOARD2_S);
    params[2] = make_sd_param(TEST_BOARD3_S);
    params[3] = make_sd_param(TEST_BOARD4_S);
    params[4] = make_sd_param(TEST_BOARD5_S);
    params[5] = make_sd_param(TEST_BOARD6_S);

    return cr_make_param_array(u8*, params, size, cleanup_params);
}

ParameterizedTest(u8** board, solver, isSolved) {
    Sudoku* sd = CreateSudoku(*board, 9, 3);
    int status = IsSudokuSolved(sd);
    cr_assert(eq(i32, status, 1),
            "A solved board was marked as not solved");
    DestroySudoku(sd);
}

ParameterizedTestParameters(solver, solve) {
    const size_t size = 5;
    u8** params = cr_calloc(size, sizeof(u8*));

    params[0] = make_sd_param(TEST_BOARD1);
    params[1] = make_sd_param(TEST_BOARD2);
    params[2] = make_sd_param(TEST_BOARD3);
    params[3] = make_sd_param(TEST_BOARD4);
    params[4] = make_sd_param(TEST_BOARD5);
    //params[5] = make_sd_param(TEST_BOARD6);

    return cr_make_param_array(u8*, params, size, cleanup_params);
}

ParameterizedTest(u8** board, solver, solve, .timeout = 5.0) {
    Sudoku* sd = CreateSudoku(*board, 9, 3);
    Sudoku* solved = SolveSudoku(sd, 0);
    cr_assert(ne(ptr, solved, NULL), "The board was not solved");
    cr_assert(eq(i32, IsSudokuSolved(solved), 1), "The board is incorrect");
    DestroySudoku(solved);
    DestroySudoku(sd);
}

#endif
