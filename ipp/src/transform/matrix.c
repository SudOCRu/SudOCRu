#include "matrix.h"
#include <stdio.h>

Matrix* NewMatrix(size_t rows, size_t cols, const float* vals)
{
    size_t size = cols * rows * sizeof(float);
    Matrix* mat = malloc(sizeof(Matrix));
    mat->cols = cols;
    mat->rows = rows;
    mat->m = malloc(size);
    memcpy(mat->m, vals, size);
    return mat;
}

void PrintMatrix(const Matrix* a)
{
    for (size_t i = 0; i < a->rows; i++)
    {
        printf("|");
        for (size_t j = 0; j < a->cols; j++)
        {
            printf(" %-8f |", a->m[i * a->cols + j]);
        }
        printf("\n");
    }
}

int MatEqual(const Matrix* a, const Matrix* b)
{
    if (a->cols != b->cols || a->rows != b-> rows)
        return 0;
    for (size_t i = 0; i < a->rows; i++)
    {
        for (size_t j = 0; j < a->cols; j++)
        {
            if (a->m[i * a->cols + j] != b->m[i * a->cols + j])
                return 0;
        }
    }
    return 1;
}

void DestroyMatrix(Matrix* mat)
{
    if (mat == NULL)
        return;
    free(mat->m);
    free(mat);
}

Matrix* MatTranspose(const Matrix* a)
{
    Matrix* mat = malloc(sizeof(Matrix));
    mat->cols = a->rows;
    mat->rows = a->cols;
    mat->m = malloc(a->cols * a->rows * sizeof(float));

    for (size_t j = 0; j < a->cols; j++)
    {
        for (size_t i = 0; i < a->rows; i++)
        {
            mat->m[j * mat->cols + i] = a->m[i * a->cols + j];
        }
    }
    return mat;
}

// Right associative matrix multiplication
Matrix* MatMultiply(const Matrix* a, const Matrix* b)
{
    return NULL;
}

int MatInvert(Matrix* a)
{
    if (a->rows != a->cols)
        return 0;
    return 1;
}
