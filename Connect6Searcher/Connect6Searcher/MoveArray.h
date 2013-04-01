#pragma once

#include "connect6.h"
#include <cassert>
#include "Array.h"

class MoveArray: public Array <Move, ROW_MAX * COL_MAX, MoveEqual> {
public:
    void PushBack(int row, int col, bool isBlack) {
        __super::PushBack(Move(row, col, isBlack));
    }
    void PushBack(const Point &p, bool isBlack) {
        PushBack(p._row, p._col, isBlack);
    }
    void PushBack(const Move &move) {
        PushBack(move._row, move._col, move._isBlack);
    }
    bool Find(int row, int col, bool isBlack) {
        return __super::Find(Move(row, col, isBlack));
    }
    bool Find(const Move &m) {
        return Find(m._row, m._col, m._isBlack);
    }
    bool Find(const Point &p, bool isBlack) {
        return Find(p._row, p._col, isBlack);
    }
};