#pragma once

#include "connect6.h"
#include <cassert>

class DPointTable {
    int _index[ROW_MAX][COL_MAX][ROW_MAX][COL_MAX];
    DPoint _dPoint[ROW_MAX * COL_MAX * ROW_MAX * COL_MAX];
    int _size;

public:
    DPointTable(): _size(0) {
        memset(_index, -1, sizeof(_index));
    }
    bool IsInTable(int r1, int c1, int r2, int c2) {
        return Index(r1, c1, r2, c2) != -1;
    }
    bool IsInTable(const Point &p1, const Point &p2) {
        return IsInTable(p1._row, p1._col, p2._row, p2._col);
    }
    bool IsInTable(const DPoint &dp) {
        return IsInTable(dp.GetPoint1(), dp.GetPoint2());
    }
    void Clear();
    int Size() {
        return _size;
    }
    void Add(int r1, int c1, int r2, int c2) {
        if(! IsInTable(r1, c1, r2, c2)) {
            _dPoint[_size] = DPoint(r1, c1, r2, c2);
            Index(r1, c1, r2, c2) = Index(r2, c2, r1, c1) = _size++;
        }
    }
    void Add(const Point &p1, const Point &p2) {
        Add(p1._row, p1._col, p2._row, p2._col);
    }
    void Add(const DPoint &dp) {
        Add(dp.GetPoint1(), dp.GetPoint2());
    }

private:
    int &Index(int r1, int c1, int r2, int c2) {
        return _index[r1][c1][r2][c2];
    }
    int &Index(const Point &p1, const Point &p2) {
        return Index(p1._row, p1._col, p2._row, p2._col);
    }
    int &Index(const DPoint &dp) {
        return Index(dp.GetPoint1(), dp.GetPoint2());
    }
    void Remove(int r1, int c1, int r2, int c2) {
        if(IsInTable(r1, c1, r2, c2)) {
            DPoint &x = _dPoint[--_size];
            DPoint &y = _dPoint[Index(r1, c1, r2, c2)];
            std::swap(x, y);
            std::swap(_index[x._r1][x._c1][x._r2][x._c2], _index[y._r1][y._c1][y._r2][y._c2]);
            std::swap(_index[x._r2][x._c2][x._r1][x._c1], _index[y._r2][y._c2][y._r1][y._c1]);
            Index(r1, c1, r2, c2) = Index(r2, c2, r1, c1) = -1;
        }
    }
    void Remove(const Point &p1, const Point &p2) {
        Remove(p1._row, p1._col, p2._row, p2._col);
    }
    void Remove(const DPoint &dp) {
        Remove(dp.GetPoint1(), dp.GetPoint2());
    }

public:
    DPoint GetDPoint(int ind) {
        assert(0 <= ind && ind < _size);
        return _dPoint[ind];
    }

private:
    DPointTable(const DPointTable &);
    DPointTable &operator = (const DPointTable &);
};