#ifdef CRIT_TEST
#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/parameterized.h>
#include <criterion/redirect.h>
#include "grid_slicer/hough_lines.h"

void redirect_io()
{
    cr_redirect_stdout();
    cr_redirect_stderr();
}

Line* make_line(unsigned int val, float theta, float rho, float x1,
        float y1, float x2, float y2)
{
    Line* l = (Line*) malloc(sizeof(Line));
    l->val = val;
    l->theta = theta;
    l->rho = rho;
    l->x1 = x1;
    l->y1 = y1;
    l->x2 = x2;
    l->y2 = y2;
    return l;
}

void free_lines(Line** lines, size_t len)
{
    for(size_t i = 0; i < len; i++)
        free(lines[i]);
    free(lines);
}

/*
struct merge_sim {
    Line** lines;
    size_t in_count;
    Line** expected;
    size_t out_count;
};

struct merge_sim* make_merge_sim(Line** lines, size_t in, Line** expected,
        size_t out)
{
    struct p_move* pm = cr_malloc(sizeof(struct p_move));
    pm->lines = lines;
    pm->in = in;
    pm->expected = expected;
    pm->out_count = out_count;
    return pm;
}

void cleanup_merge_sims(struct criterion_test_params *crp) {
    for (size_t i = 0; i < crp->length; i++) {
        struct merge_sim* ms = ((struct merge_sim**) crp->params)[i];
        FreeLines(ms->lines, ms->in_count);
        FreeLines(ms->expected, ms->out_count);
    }
    cr_free(crp->params);
}


ParameterizedTestParameters(ipp, mergeSimilar) {
    const size_t size = 1;
    struct merge_sim** params = cr_calloc(size, sizeof(struct merge_sim*));

    params[0] = make_merge_sim();

    return cr_make_param_array(struct merge_sim*, params, size,
            cleanup_merge_sims);
}
*/

Test(ipp, mergeSimilar) {
    const size_t expected_count = 3;
    const size_t in_len = 5;
    Line** input = calloc(in_len, sizeof(Line*));
    input[0] = make_line(45, M_PI/1.98,   56, -1000, -1000, 1000, 1000);
    input[1] = make_line(38, M_PI/2.02,   50, -1000, -1000, 1000, 1000);
    input[2] = make_line(12, M_PI/5.00, -100, -1000, -1000, 1000, 1000);
    input[3] = make_line(29, M_PI/4.70,  -70, -1000, -1000, 1000, 1000);
    input[4] = make_line(29, M_PI/2.00,   61, -1000, -1000, 1000, 1000);

    size_t out_len = 0;
    Line** out = AverageLines(input, in_len, &out_len);

    cr_assert(eq(u32, out_len, expected_count));
    cr_assert(eq(ptr, out[0], input[0]), "First line %p <-> %p",
            out[0], input[0]);
    cr_assert(eq(ptr, out[1], input[2]), "Second line %p <-> %p",
            out[1], input[2]);
    cr_assert(eq(ptr, out[2], input[3]), "Third line %p <-> %p",
            out[2], input[3]);

    // Cleaning
    free_lines(input, in_len);
}

#endif
