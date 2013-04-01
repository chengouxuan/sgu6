#pragma once

#include <ctime>
#include <cstdlib>
#include <cassert>
#include "Connect6.h"

class RandTable {
public:
    static const int _RAND_TABLES = 3;
    unsigned int _rand32[_RAND_TABLES][ROW_MAX][COL_MAX];
    unsigned __int64 _rand64[_RAND_TABLES][ROW_MAX][COL_MAX];

    RandTable();
    int TableNo(CellType type) {
        switch(type) {
            case CELL_TYPE_BLACK: return 0;
            case CELL_TYPE_WHITE: return 1;
            case CELL_TYPE_EMPTY: return 2;
            default: assert(type == CELL_TYPE_NOT_A_CELL); assert(false); return 0;
        }
    }
    unsigned int Rand32(int row, int col, CellType type) {
        return _rand32[TableNo(type)][row][col];
    }
    unsigned __int64 Rand64(int row, int col, CellType type) {
        return _rand64[TableNo(type)][row][col];
    }
};

extern RandTable randTable;
