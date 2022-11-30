#ifdef CRIT_TEST
#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/parameterized.h>
#include <criterion/redirect.h>
#include "transform/matrix.h"

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

Test(matrix, new) {
    float vals[] = {
        1, 2, 3, 4,
        2, 3, 4, 1,
        3, 4, 1, 2,
    };
    Matrix* mat = NewMatrix(3, 4, vals);

    cr_assert(ne(ptr, mat->m, &vals));
    cr_assert(eq(sz, mat->cols, 4));
    cr_assert(eq(sz, mat->rows, 3));

    DestroyMatrix(mat);
}

Test(matrix, not_same_dim)
{
    float valsA[] = {
        1, 2, 3, 4,
        2, 3, 4, 1,
        3, 4, 1, 2,
    };
    Matrix* a = NewMatrix(3, 4, valsA);
    float valsB[] = {
        1, 2,
        0, -1,
    };
    Matrix* b = NewMatrix(2, 2, valsB);
    cr_assert(eq(int, MatEqual(a, b), 0));

    DestroyMatrix(a);
    DestroyMatrix(b);
}

Test(matrix, not_same)
{
    float valsA[] = {
        3, 3,
        5, 5,
    };
    Matrix* a = NewMatrix(2, 2, valsA);
    float valsB[] = {
        1, 2,
        0, -1,
    };
    Matrix* b = NewMatrix(2, 2, valsB);
    cr_assert(eq(int, MatEqual(a, b), 0));

    DestroyMatrix(a);
    DestroyMatrix(b);
}

Test(matrix, same)
{
    float valsA[] = {
        1, 0,
        0, 1,
    };
    Matrix* a = NewMatrix(2, 2, valsA);
    float valsB[] = {
        1, 0,
        0, 1,
    };
    Matrix* b = NewMatrix(2, 2, valsB);
    cr_assert(eq(int, MatEqual(a, b), 1));

    DestroyMatrix(a);
    DestroyMatrix(b);
}

struct mat_tr {
    Matrix* actual;
    Matrix* expected;
};

struct mat_tr* make_mat_tr(const float* actual, size_t a_rows, size_t a_cols,
        const float* expected, size_t e_rows, size_t e_cols)
{
    struct mat_tr* tr = cr_malloc(sizeof(struct mat_tr));
    tr->actual = cr_mat(a_rows, a_cols, actual);
    tr->expected = cr_mat(e_rows, e_cols, expected);
    return tr;
}

void cleanup_mat_tr(struct criterion_test_params *crp) {
    for (size_t i = 0; i < crp->length; i++) {
        struct mat_tr* tr = ((struct mat_tr**) crp->params)[i];
        cr_free(tr->actual->m);
        cr_free(tr->actual);
        cr_free(tr->expected->m);
        cr_free(tr->expected);
        cr_free(tr);
    }
    cr_free(crp->params);
}

ParameterizedTestParameters(matrix, transpose) {
    const size_t size = 3;
    struct mat_tr** params = cr_calloc(size, sizeof(struct mat_tr*));

    float m1[] = {
        1, 2, 3, 4,
        2, 3, 4, 1,
        3, 4, 1, 2,
    };
    float m1_e[] = {
        1, 2, 3,
        2, 3, 4,
        3, 4, 1,
        4, 1, 2,
    };
    params[0] = make_mat_tr(m1, 3, 4, m1_e, 4, 3);

    float m2[] = {
        1, 0,
        0, 1
    };
    float m2_e[] = {
        1, 0,
        0, 1,
    };
    params[1] = make_mat_tr(m2, 2, 2, m2_e, 2, 2);

    float m3[] = {
        1,
        2,
        3,
        4,
        5,
        6,
    };
    float m3_e[] = {
        1, 2, 3, 4, 5, 6
    };
    params[2] = make_mat_tr(m3, 6, 1, m3_e, 1, 6);

    return cr_make_param_array(struct mat_tr*, params, size, cleanup_mat_tr);
}

ParameterizedTest(struct mat_tr** tr, matrix, transpose) {
    Matrix* actual = (*tr)->actual;
    Matrix* transposed = MatTranspose(actual);
    Matrix* expected = (*tr)->expected;

    cr_assert(ne(ptr, transposed, actual));
    cr_assert(ne(ptr, transposed->m, actual->m));
    cr_assert(eq(sz, transposed->cols, actual->rows));
    cr_assert(eq(sz, transposed->rows, actual->cols));
    int r = MatEqual(transposed, expected);
    if (!r)
    {
        printf("expected =\n");
        PrintMatrix(expected);
        printf("transposed =\n");
        PrintMatrix(transposed);
    }
    cr_assert(eq(int, r, 1));
}

struct mat_mul {
    Matrix* a;
    Matrix* b;
    Matrix* expected;
};

struct mat_mul* make_mat_mul(const float* a, size_t a_rows, size_t a_cols,
        const float* b, size_t b_rows, size_t b_cols,
        const float* expected, size_t e_rows, size_t e_cols)
{
    struct mat_mul* ml = cr_malloc(sizeof(struct mat_mul));
    ml->a = cr_mat(a_rows, a_cols, a);
    ml->b = cr_mat(b_rows, b_cols, b);
    ml->expected = cr_mat(e_rows, e_cols, expected);
    return ml;
}

void cleanup_mat_mul(struct criterion_test_params *crp) {
    for (size_t i = 0; i < crp->length; i++) {
        struct mat_mul* ml = ((struct mat_mul**) crp->params)[i];
        cr_free(ml->a->m);
        cr_free(ml->a);
        cr_free(ml->b->m);
        cr_free(ml->b);
        cr_free(ml->expected->m);
        cr_free(ml->expected);
        cr_free(ml);
    }
    cr_free(crp->params);
}

ParameterizedTestParameters(matrix, multiply) {
    const size_t size = 5;
    struct mat_mul** params = cr_calloc(size, sizeof(struct mat_mul*));

    float a1[] = {
        1, 0,
        0, 1
    };
    float b1[] = {
        1, 2,
        3, 4
    };
    params[0] = make_mat_mul(a1, 2, 2, b1, 2, 2, b1, 2, 2);

    float a2[] = {
        1, 2,
        2, 1,
    };
    float b2[] = {
        -1, -2,
        -2, -1,
    };
    float e2[] = {
        -5, -4,
        -4, -5
    };
    params[1] = make_mat_mul(a2, 2, 2, b2, 2, 2, e2, 2, 2);

    float a3[] = {
        1,  2,  0,
        0, -1, -1,
        3,  2,  0,
    };
    float b3[] = {
        1,
        -1,
        1,
    };
    float e3[] = {
        -1,
        0,
        1,
    };
    params[2] = make_mat_mul(a3, 3, 3, b3, 3, 1, e3, 3, 1);

    float a4[] = {
        1, 2, 0,
        -1, 0, -1
    };
    float b4[] = {
        3,
        0,
        9,
    };
    float e4[] = {
        3,
        -12
    };
    params[3] = make_mat_mul(a4, 2, 3, b4, 3, 1, e4, 2, 1);

    float a5[] = {
        1,  2,
        0, -1
    };
    float e5[] = {
        1, 0,
        0, 1,
    };
    params[4] = make_mat_mul(a5, 2, 2, a5, 2, 2, e5, 2, 2);

    return cr_make_param_array(struct mat_mul*, params, size, cleanup_mat_mul);
}

ParameterizedTest(struct mat_mul** tr, matrix, multiply) {
    const Matrix* a = (*tr)->a;
    const Matrix* b = (*tr)->b;
    const Matrix* expected = (*tr)->expected;
    Matrix* ab = MatMultiply(a, b);

    cr_assert(ne(ptr, ab, NULL), "A and B can be multiplied but they were not");
    cr_assert(eq(sz, ab->rows, a->rows));
    cr_assert(eq(sz, ab->cols, b->cols));
    int r = MatEqual(ab, expected);
    if (!r)
    {
        printf("expected =\n");
        PrintMatrix(expected);
        printf("A * B =\n");
        PrintMatrix(ab);
    }
    cr_assert(eq(int, r, 1));
}

struct mat_inv {
    Matrix* a;
    int is_invertible;
};

struct mat_inv* make_mat_inv(const float* a, size_t a_rows, size_t a_cols,
        int is_invertible)
{
    struct mat_inv* mi = cr_malloc(sizeof(struct mat_inv));
    mi->a = cr_mat(a_rows, a_cols, a);
    mi->is_invertible = is_invertible;
    return mi;
}

void cleanup_mat_inv(struct criterion_test_params *crp) {
    for (size_t i = 0; i < crp->length; i++) {
        struct mat_inv* mi = ((struct mat_inv**) crp->params)[i];
        cr_free(mi->a->m);
        cr_free(mi->a);
        cr_free(mi);
    }
    cr_free(crp->params);
}

ParameterizedTestParameters(matrix, invert) {
    const size_t size = 8;
    struct mat_inv** params = cr_calloc(size, sizeof(struct mat_inv*));

    float a1[] = {
        1, 0,
        0, 1
    };
    params[0] = make_mat_inv(a1, 2, 2, 1);

    float a2[] = {
        1, 2,
        3, 4
    };
    params[1] = make_mat_inv(a2, 2, 2, 1);

    float a3[] = {
        -1, -2,
        -2, -1,
    };
    params[2] = make_mat_inv(a3, 2, 2, 1);

    float a4[] = {
        -5, -4,
        -4, -5
    };
    params[3] = make_mat_inv(a4, 2, 2, 1);

    float a5[] = {
        1,  2,  0,
        0, -1, -1,
        3,  2,  0,
    };
    params[4] = make_mat_inv(a5, 3, 3, 1);

    float a6[] = {
        1, 2, 0,
        -1, 0, -1
    };
    params[5] = make_mat_inv(a6, 2, 3, 0);

    float a7[] = {
        3,
        0,
        9,
    };
    params[6] = make_mat_inv(a7, 3, 1, 0);

    float a8[] = {
        1,  2,
        0, -1
    };
    params[7] = make_mat_inv(a8, 2, 2, 1);

    return cr_make_param_array(struct mat_inv*, params, size, cleanup_mat_inv);
}

ParameterizedTest(struct mat_inv** tr, matrix, invert) {
    Matrix* a = (*tr)->a;

    if ((*tr)->is_invertible)
    {
        Matrix* inv = NewMatrix(a->rows, a->cols, a->m);
        cr_assert(eq(int, MatInvert(inv), 1), "Matrix was not inverted but it is "
                "invertible");
        Matrix* ex_id = GetIdMatrix(a->cols);

        Matrix* id = MatMultiply(inv, a);
        cr_assert(ne(ptr, id, NULL),
                "A and A^-1 should have the same dimension");
        int r = MatEqual(id, ex_id);
        if (!r)
        {
            printf("A^-1 =\n");
            PrintMatrix(inv);
            printf("A * A^-1 =\n");
            PrintMatrix(id);
            printf("Id^%lu =\n", a->cols);
            PrintMatrix(ex_id);
        }
        cr_assert(eq(int, r, 1), "A*A^-1 should be the Id matrix");

        DestroyMatrix(ex_id);
        DestroyMatrix(id);
        DestroyMatrix(inv);
    }
    else
    {
        cr_assert(eq(int, MatInvert(a), 0), "Matrix was inverted but it is not "
                "invertible");
    }
}

#endif
