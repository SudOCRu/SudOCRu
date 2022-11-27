#pragma once
#include <stdlib.h>
#include <string.h>

typedef struct Matrix
{
    float* m;
    size_t cols, rows;
} Matrix;

Matrix* NewMatrix(size_t rows, size_t cols, const float* vals);
void PrintMatrix(const Matrix* a);
void DestroyMatrix(Matrix* mat);

int MatEqual(const Matrix* a, const Matrix* b);
Matrix* MatTranspose(const Matrix* a);
// Right associative matrix multiplication
Matrix* MatMultiply(const Matrix* a, const Matrix* b);
int MatInvert(Matrix* a);
