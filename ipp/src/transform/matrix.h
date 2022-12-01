#pragma once
#include <stdlib.h>
#include <string.h>

/*
 *  This struct represents a Matrix (n x p) with n rows and p cols
 */
typedef struct Matrix
{
    float* m;
    size_t cols, rows;
} Matrix;

/*
 * NewMatrix(size_t rows, size_t cols, const float* vals) -> Matrix*
 *
 * Create a new matrix of n rows and p cols and vals as the array of coefficents
 * in row-major order (vals is copied to the internal array if Matrix)
 */
Matrix* NewMatrix(size_t rows, size_t cols, const float* vals);

/*
 * GetIdMatrix(size_t n) -> Matrix*
 *
 * Returns the Identity matrix of order n, for example with n=2:
 * M = |---|---|
 *     | 1 | 0 |
 *     | 0 | 1 |
 *     |---|---|
 */
Matrix* GetIdMatrix(size_t n);

/*
 * PrintMatrix(const Matrix* a)
 *
 * Print the matrix a in stdout
 */
void PrintMatrix(const Matrix* a);

/*
 * DestroyMatrix(Matrix* matrix)
 *
 * Frees the memory allocated by the matrix. The pointer to matrix must not be
 * used after a call to this function. Note: It is safe to pass NULL to
 * this function.
 */
void DestroyMatrix(Matrix* mat);

/*
 * MatEqual(const Matrix* a, const Matrix* b) -> bool
 *
 * Returns true if the two matrix are equal by checking if the dimensions are
 * the same and if values of the internal arrays are equal (float comparaison
 * between each coeffcient with M_EP epsilion). Note: It is safe to pass NULL as
 * the matrices a or b, it will return true only if a and b are NULL.
 */
int MatEqual(const Matrix* a, const Matrix* b);

/*
 * MatTranspose(const Matrix* a) -> Matrix*
 *
 * Returns the transposed matrix of a, for example:
 * A = |---|---|---|    =>    transposed(A) = |---|---|
 *     | a | b | c |                          | a | d |
 *     | d | e | f |                          | b | e |
 *     |---|---|---|                          | c | f |
 *                                            |---|---|
 * Note: It is safe to pass NULL to this function, it will return NULL.
 */
Matrix* MatTranspose(const Matrix* a);


/*
 * MatMultiply(const Matrix* a, const Matrix* b) -> Matrix*
 *
 * Let A(m, n) and B(b, p) two matrices. This function will return the product
 * of the two matrices only if n = p, as A(m, n) and B(n, p), the result will be
 * C(m, p). Returns NULL otherwise. It is safe to pass NULL to a and/or b.
 */
Matrix* MatMultiply(const Matrix* a, const Matrix* b);


/*
 * MatMultiplyN(const Matrix* a, const Matrix* b, Matrix* c) -> bool
 *
 * No allocation version of MatMultiply, this function will return true if the
 * matrix c has been modified, false otherwise. It is safe to pass NULL to a,
 * b or c.
 */
int MatMultiplyN(const Matrix* a, const Matrix* b, Matrix* c);


/*
 * MatInvert(Matrix* a) -> bool
 *
 * Returns true (1) if the matrix A is a square matrix and is invertible.
 * Otherwise, this function returns false (0). Note that, whether or not the
 * matrix A is invertible, the matrix will be modified: in the case of A
 * invertible, A will contain A^-1, otherwise, the original values of A will be
 * lost.
 */
int MatInvert(Matrix* a);
