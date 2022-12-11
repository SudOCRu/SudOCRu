#ifdef CRIT_TEST
#include "mat_tests_utils.h"

Matrix* cr_mat(size_t rows, size_t cols, const float* vals)
{
    size_t size = cols * rows * sizeof(float);
    Matrix* mat = cr_malloc(sizeof(Matrix));
    mat->cols = cols;
    mat->rows = rows;
    mat->m = cr_malloc(size);
    memcpy(mat->m, vals, size);
    return mat;
}
#endif
