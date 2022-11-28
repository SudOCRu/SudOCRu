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

Matrix* GetIdMatrix(size_t n)
{
    Matrix* mat = malloc(sizeof(Matrix));
    mat->cols = n;
    mat->rows = n;
    mat->m = calloc(n * n, sizeof(float));
    for (size_t i = 0; i < n; i++)
        mat->m[i * n + i] = 1;
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
    if (a->cols != b->rows)
        return NULL;
    Matrix* c = malloc(sizeof(Matrix));
    c->rows = a->rows;
    c->cols = b->cols;
    c->m = calloc(c->cols * c->rows, sizeof(float));

    for(size_t i = 0; i < a->rows; ++i)
    {
        for(size_t k = 0; k < a->cols; ++k)
        {
            for(size_t j = 0; j < b->cols; ++j)
            {
                c->m[i * c->cols + j] += a->m[i * a->cols + k]
                    * b->m[k * b->cols + j];
            }
        }
    }

    return c;
}

int MatInvert(Matrix* a)
{
    if (a->rows != a->cols)
        return 0;
    size_t n = a->cols;

    float* id = calloc(n * n, sizeof(float));
    for (size_t i = 0; i < n; i++)
        id[i * n + i] = 1;

    // Gauss-Jordan Elimination
    for (size_t i = 0; i < n; i++)
    {
        if (a->m[i * n + i] == 0) // division by 0
            return 0;
        for (size_t j = 0; j < n; j++)
        {
            if (i == j) continue;
            float r = a->m[j * n + i] / a->m[i * n + i];
            for (size_t k = 0; k < n; k++)
            {
                id[j * n + k] = id[j * n + k] -
                    r * id[i * n + k];
            }
        }
    }

    // Normalisation to have ones on the diagonal of a
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < n; j++)
        {
            id[i * n + j] /= a->m[i * n + i];
        }
    }
    a->m = id;
    return 1;
}
