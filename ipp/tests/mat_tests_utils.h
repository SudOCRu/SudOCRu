#pragma once
#ifdef CRIT_TEST
#include <criterion/parameterized.h>
#include "transform/matrix.h"

Matrix* cr_mat(size_t rows, size_t cols, const float* vals);
#endif
