#pragma once

const u8 TEST_INVALID[] = {
    1, 0, 0, 4, 8, 9, 0, 0, 6,
    0, 3, 7, 0, 0, 0, 0, 4, 0,
    7, 0, 0, 0, 0, 1, 2, 9, 5,
    0, 0, 7, 1, 2, 0, 6, 0, 0,
    5, 0, 0, 7, 0, 3, 0, 0, 8,
    0, 0, 6, 0, 9, 5, 7, 0, 0,
    9, 1, 4, 6, 0, 0, 0, 0, 0,
    0, 2, 0, 0, 0, 0, 0, 3, 7,
    8, 0, 0, 5, 1, 2, 0, 0, 4,
};

const u8 TEST_BOARD1[] = {
    1, 0, 0, 4, 8, 9, 0, 0, 6,
    7, 3, 0, 0, 0, 0, 0, 4, 0,
    0, 0, 0, 0, 0, 1, 2, 9, 5,
    0, 0, 7, 1, 2, 0, 6, 0, 0,
    5, 0, 0, 7, 0, 3, 0, 0, 8,
    0, 0, 6, 0, 9, 5, 7, 0, 0,
    9, 1, 4, 6, 0, 0, 0, 0, 0,
    0, 2, 0, 0, 0, 0, 0, 3, 7,
    8, 0, 0, 5, 1, 2, 0, 0, 4,
};

const u8 TEST_BOARD2[] =  {
    5, 3, 0, 0, 7, 0, 0, 0, 0,
    6, 0, 0, 1, 9, 5, 0, 0, 0,
    0, 9, 8, 0, 0, 0, 0, 6, 0,
    8, 0, 0, 0, 6, 0, 0, 0, 3,
    4, 0, 0, 8, 0, 3, 0, 0, 1,
    7, 0, 0, 0, 2, 0, 0, 0, 6,
    0, 6, 0, 0, 0, 0, 2, 8, 0,
    0, 0, 0, 4, 1, 9, 0, 0, 5,
    0, 0, 0, 0, 8, 0, 0, 7, 9,
};

const u8 TEST_BOARD3[] =  {
    0, 2, 0, 6, 0, 8, 0, 0, 0,
    5, 8, 0, 0, 0, 9, 7, 0, 0,
    0, 0, 0, 0, 4, 0, 0, 0, 0,
    3, 7, 0, 0, 0, 0, 5, 0, 0,
    6, 0, 0, 0, 0, 0, 0, 0, 4,
    0, 0, 8, 0, 0, 0, 0, 1, 3,
    0, 0, 0, 0, 2, 0, 0, 0, 0,
    0, 0, 9, 8, 0, 0, 0, 3, 6,
    0, 0, 0, 3, 0, 6, 0, 9, 0,
};

const u8 TEST_BOARD4[] =  {
    4, 9, 0, 0, 0, 0, 0, 2, 0,
    0, 0, 8, 0, 0, 6, 0, 0, 4,
    0, 0, 0, 0, 0, 0, 1, 6, 9,
    0, 0, 0, 4, 0, 5, 0, 0, 7,
    0, 8, 5, 0, 6, 0, 4, 0, 0,
    3, 0, 0, 0, 2, 0, 0, 1, 0,
    0, 0, 0, 6, 7, 0, 9, 0, 0,
    0, 0, 9, 0, 3, 1, 0, 7, 0,
    8, 0, 0, 0, 0, 0, 0, 0, 1,
};

const u8 TEST_BOARD5[] =  {
    8, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 3, 6, 0, 0, 0, 0, 0,
    0, 7, 0, 0, 9, 0, 2, 0, 0,
    0, 5, 0, 0, 0, 7, 0, 0, 0,
    0, 0, 0, 0, 4, 5, 7, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 3, 0,
    0, 0, 1, 0, 0, 0, 0, 6, 8,
    0, 0, 8, 5, 0, 0, 0, 1, 0,
    0, 9, 0, 0, 0, 0, 4, 0, 0,
};

const u8 TEST_BOARD6[] =  {
    8, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 6, 0, 0, 0, 0, 3,
    0, 7, 4, 0, 8, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 3, 0, 0, 2,
    0, 8, 0, 0, 4, 0, 0, 1, 0,
    6, 0, 0, 5, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 7, 8, 0,
    5, 0, 0, 0, 0, 9, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 4, 0,
};

const u8 TEST_BOARDXX[] =
{
    9, 0, 0, 8, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 0, 0, 1, 0, 0, 0, 3,
    0, 1, 0, 0, 0, 0, 0, 6, 0,
    0, 0, 0, 4, 0, 0, 0, 7, 0,
    7, 0, 8, 6, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 3, 0, 1, 0, 0,
    4, 0, 0, 0, 0, 0, 2, 0, 0
};

const u8 TEST_BOARD1_S[] =  {
    1, 5, 2, 4, 8, 9, 3, 7, 6,
    7, 3, 9, 2, 5, 6, 8, 4, 1,
    4, 6, 8, 3, 7, 1, 2, 9, 5,
    3, 8, 7, 1, 2, 4, 6, 5, 9,
    5, 9, 1, 7, 6, 3, 4, 2, 8,
    2, 4, 6, 8, 9, 5, 7, 1, 3,
    9, 1, 4, 6, 3, 7, 5, 8, 2,
    6, 2, 5, 9, 4, 8, 1, 3, 7,
    8, 7, 3, 5, 1, 2, 9, 6, 4,
};

const u8 TEST_BOARD2_S[] =  {
    5, 3, 4, 6, 7, 8, 9, 1, 2,
    6, 7, 2, 1, 9, 5, 3, 4, 8,
    1, 9, 8, 3, 4, 2, 5, 6, 7,
    8, 5, 9, 7, 6, 1, 4, 2, 3,
    4, 2, 6, 8, 5, 3, 7, 9, 1,
    7, 1, 3, 9, 2, 4, 8, 5, 6,
    9, 6, 1, 5, 3, 7, 2, 8, 4,
    2, 8, 7, 4, 1, 9, 6, 3, 5,
    3, 4, 5, 2, 8, 6, 1, 7, 9,
};

const u8 TEST_BOARD3_S[] =  {
    1, 2, 3, 6, 7, 8, 9, 4, 5,
    5, 8, 4, 2, 3, 9, 7, 6, 1,
    9, 6, 7, 1, 4, 5, 3, 2, 8,
    3, 7, 2, 4, 6, 1, 5, 8, 9,
    6, 9, 1, 5, 8, 3, 2, 7, 4,
    4, 5, 8, 7, 9, 2, 6, 1, 3,
    8, 3, 6, 9, 2, 4, 1, 5, 7,
    2, 1, 9, 8, 5, 7, 4, 3, 6,
    7, 4, 5, 3, 1, 6, 8, 9, 2,
};

const u8 TEST_BOARD4_S[] =  {
    4, 9, 6, 1, 5, 7, 8, 2, 3,
    2, 1, 8, 3, 9, 6, 7, 5, 4,
    7, 5, 3, 2, 8, 4, 1, 6, 9,
    9, 6, 2, 4, 1, 5, 3, 8, 7,
    1, 8, 5, 7, 6, 3, 4, 9, 2,
    3, 7, 4, 9, 2, 8, 5, 1, 6,
    5, 3, 1, 6, 7, 2, 9, 4, 8,
    6, 4, 9, 8, 3, 1, 2, 7, 5,
    8, 2, 7, 5, 4, 9, 6, 3, 1,
};

const u8 TEST_BOARD5_S[] =  {
    8, 1, 2, 7, 5, 3, 6, 4, 9,
    9, 4, 3, 6, 8, 2, 1, 7, 5,
    6, 7, 5, 4, 9, 1, 2, 8, 3,
    1, 5, 4, 2, 3, 7, 8, 9, 6,
    3, 6, 9, 8, 4, 5, 7, 2, 1,
    2, 8, 7, 1, 6, 9, 5, 3, 4,
    5, 2, 1, 9, 7, 4, 3, 6, 8,
    4, 3, 8, 5, 2, 6, 9, 1, 7,
    7, 9, 6, 3, 1, 8, 4, 5, 2,
};

const u8 TEST_BOARD6_S[] =  {
    1, 2, 6, 4, 3, 7, 9, 5, 8,
    8, 9, 5, 6, 2, 1, 4, 7, 3,
    3, 7, 4, 9, 8, 5, 1, 2, 6,
    4, 5, 7, 1, 9, 3, 8, 6, 2,
    9, 8, 3, 2, 4, 6, 5, 1, 7,
    6, 1, 2, 5, 7, 8, 3, 9, 4,
    2, 6, 9, 3, 1, 4, 7, 8, 5,
    5, 4, 8, 7, 6, 9, 2, 3, 1,
    7, 3, 1, 8, 5, 2, 6, 4, 9,
};
