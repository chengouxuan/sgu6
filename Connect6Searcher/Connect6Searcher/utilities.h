#pragma once

#include "Connect6.h"
#include "defines.h"

bool IsGameOver(Board board);
void PrintBoard(Board board);

inline bool IsInsideBoard(const Point &p) {
    return IS_INSIDE_BOARD(p._row, p._col);
}

inline bool IsInsideBoard(int row, int col) {
    return IS_INSIDE_BOARD(row, col);
}

class Counter {
public:
    int _cnt[ROW_MAX][COL_MAX];
    Counter() {
        memset(_cnt, 0, sizeof(_cnt));
    }
    void Zero() {
        memset(_cnt, 0, sizeof(_cnt));
    }
    int &Cnt(int row, int col) {
        return _cnt[row][col];
    }
    int &Cnt(const Point &p) {
        return Cnt(p._row, p._col);
    }
};