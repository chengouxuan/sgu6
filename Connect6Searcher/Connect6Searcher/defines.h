#pragma once

#define FOR_EACH(i, sz) for(int i=0, SZ=(sz); i<SZ; ++i)
#define FOR_EACH_ROW(i) FOR_EACH(i, ROW_MAX)
#define FOR_EACH_COL(i) FOR_EACH(i, COL_MAX)
#define FOR_EACH_CELL(r, c) FOR_EACH_ROW(r)FOR_EACH_COL(c)
#define FOR_EACH_SEG(r, c, d) FOR_EACH_CELL(r, c)FOR_EACH(d, 4)
#define IS_INSIDE_BOARD(r, c) (0 <= (r) && (r) < ROW_MAX && 0 <= (c) && (c) < COL_MAX)

const int dr[4] = {1, 0, 1, 1};
const int dc[4] = {0, 1, 1, -1};
