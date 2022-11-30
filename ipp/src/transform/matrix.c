#include "matrix.h"
#include <stdio.h>
#include <math.h>
#include <float.h>
#define M_EP 1e-5

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

static inline int feq(float a, float b, float epsilon) {
    return a == b || (fabs(a) == 0 && fabs(b) == 0) || fabs(a - b) < epsilon;
}

int MatEqual(const Matrix* a, const Matrix* b)
{
    if (a == NULL || b == NULL)
        return a == b;
    if (a->cols != b->cols || a->rows != b-> rows)
        return 0;
    for (size_t i = 0; i < a->rows; i++)
    {
        for (size_t j = 0; j < a->cols; j++)
        {
            if (!feq(a->m[i * a->cols + j], b->m[i * a->cols + j], M_EP))
            {
                /*
                printf("<a == b> %f != %f\n", a->m[i * a->cols + j] + 0.0,
                        b->m[i * a->cols + j] + 0.0);
                        */
                return 0;
            }
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
    if (a == NULL)
        return NULL;
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
    if (a == NULL || b == NULL || a->cols != b->rows)
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

static inline void SwapLines(float* m, size_t a, size_t b, size_t cols)
{
    if (a == b) return;
    for (size_t j = 0; j < cols; j++)
    {
        float tmp = m[a * cols + j];
        m[a * cols + j] = m[b * cols + j];
        m[b * cols + j] = tmp;
    }
}

int MatInvert(Matrix* a)
{
    if (a == NULL || a->rows != a->cols) // Not a square matrix
        return 0;
    size_t n = a->cols;

    float* id = malloc(n * n * sizeof(float));
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < n; j++)
        {
            id[i * n + j] = j == i;
        }
    }

    // Gauss-Jordan Elimination
    for (size_t i = 0; i < n; i++)
    {
        if (feq(a->m[i * n + i], 0, M_EP)) // Found a 0 in the diagonal
        {
            // Find a line with the coef at (position i) != 0 and swap them
            size_t l = 0;
            while (l < n && feq(a->m[l * n + i], 0, M_EP))
                l++;

            // Could not find any line with a value != 0 at pos i
            // Not invertible
            if (l == i)
                return 0;
            SwapLines(a->m, i, l, n);
            SwapLines(id, i, l, n);
        }
        for (size_t j = 0; j < n; j++)
        {
            if (i == j) continue;
            float r = a->m[j * n + i] / a->m[i * n + i];
            for (size_t k = 0; k < n; k++)
            {
                a->m[j * n + k] = a->m[j * n + k] - r * a->m[i * n + k];
                id[j * n + k]   =   id[j * n + k] - r * id[i * n + k];
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
    float* m = a->m;
    a->m = id;
    free(m);
    return 1;
}
