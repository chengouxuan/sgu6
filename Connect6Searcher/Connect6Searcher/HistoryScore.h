#pragma once

#include "connect6.h"
#include "utilities.h"
#include <cassert>

class HistoryScore {
public:
    int _score[2][ROW_MAX][COL_MAX][ROW_MAX][COL_MAX];

    int TableNo(bool isBlack) {
        return isBlack ? 0 : 1;
    }
    int GetScore(int r1, int c1, int r2, int c2, bool isBlack) {
        assert(::IsInsideBoard(r1, c1) && ::IsInsideBoard(r2, c2));
        return _score[TableNo(isBlack)][r1][c1][r2][c2];
    }
    int GetScore(const DMove &dm) {
        return GetScore(dm._r1, dm._c1, dm._r2, dm._c2, dm._isBlack);
    }
    int GetScore(const Point &p1, const Point &p2, bool isBlack) {
        return GetScore(DMove(p1, p2, isBlack));
    }
    void IncreaseScore(int r1, int c1, int r2, int c2, bool isBlack, int d) {
        assert(::IsInsideBoard(r1, c1) && ::IsInsideBoard(r2, c2));
        _score[TableNo(isBlack)][r2][c2][r1][c1] = (_score[TableNo(isBlack)][r1][c1][r2][c2] += d);
    }
    void IncreaseScore(const DMove &dm, int d) {
        IncreaseScore(dm._r1, dm._c1, dm._r2, dm._c2, dm._isBlack, d);
    }
    void IncreaseScore(const Point &p1, const Point &p2, bool isBlack, int d) {
        IncreaseScore(DMove(p1, p2, isBlack), d);
    }
    void Zero() {
        memset(_score, 0, sizeof(_score));
    }
};

extern HistoryScore historyScore;