#ifdef CRIT_TEST
#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/parameterized.h>
#include <criterion/redirect.h>
#include "transform/homography.h"
#include "mat_tests_utils.h"

struct homography_t {
    BBox* from;
    BBox* to;
    Matrix* expected;
};

struct homography_t* make_homography_t(BBox* from, BBox* to,
        const float* expected)
{
    struct homography_t* h = cr_malloc(sizeof(struct homography_t));

    h->from = cr_malloc(sizeof(BBox));
    memcpy(h->from, from, sizeof(BBox));

    h->to = cr_malloc(sizeof(BBox));
    memcpy(h->to, to, sizeof(BBox));

    h->expected = cr_mat(8, 1, expected);
    return h;
}

void cleanup_homography_t(struct criterion_test_params *crp) {
    for (size_t i = 0; i < crp->length; i++) {
        struct homography_t* h = ((struct homography_t**) crp->params)[i];
        cr_free(h->from);
        cr_free(h->to);
        cr_free(h->expected->m);
        cr_free(h->expected);
        cr_free(h);
    }
    cr_free(crp->params);
}

ParameterizedTestParameters(homography, solve) {
    const size_t size = 4;
    struct homography_t** p = cr_calloc(size, sizeof(struct homography_t*));

    BBox ft1 = { /*1*/ 0, 0, /*2*/  10, 0, /*3*/ 0, 10, /*4*/ 10, 10 };
    float h[] = {
        1, 0, 0,
        0, 1, 0,
        0, 0,
    };
    p[0] = make_homography_t(&ft1, &ft1, h);

    BBox t2 = { 10, 0, 10, 10, 0, 10, 0, 0 };
    float h2[] = {
        0, -1, 10,
        1, -1, 0,
        0, -0.2,
    };
    p[1] = make_homography_t(&ft1, &t2, h2);

    BBox f3 = { 200, 541, 1216, 413, 1373, 993, 267, 1208 };
    BBox t3 = { 0, 0, 300, 0, 300, 300, 0, 300 };
    float h3[] = {
        0.283354,
        -0.028463,
        -41.272461,
        0.067113,
        0.532711,
        -301.615234,
        -0.000086,
        0.000184,
    };
    p[2] = make_homography_t(&f3, &t3, h3);

    BBox f4 = { 45, 32, 565, 633, 1087, 812, 964, 1131 };
    BBox t4 = { 0, 0, 400, 0, 0, 400, 400, 400 };
    float h4[] = {
        -0.412949,
        0.551559,
        0.955078,
        -7.111084,
        6.153076,
        123.132812, 
        -0.018272,
        0.015193,
    };
    p[3] = make_homography_t(&f4, &t4, h4);

    return cr_make_param_array(struct homography_t*, p, size,
            cleanup_homography_t);
}

ParameterizedTest(struct homography_t** tr, homography, solve) {
    struct homography_t* p = *tr;
    Matrix* actual = GetHomographyMatrix(p->from, p->to);

    int r = MatEqual(actual, p->expected);
    if (!r)
    {
        printf("Actual =\n");
        PrintMatrix(actual);
        printf("Expected =\n");
        PrintMatrix(p->expected);
    }
    DestroyMatrix(actual);
    cr_assert(eq(int, r, 1), "Bad homography matrix");
}

#endif
