#pragma once

#include <cassert>
#include "connect6.h"
#include "SegmentTable.h"

class Zone {
public:
    int _index[ROW_MAX][COL_MAX];
    Point _pointArray[ROW_MAX * COL_MAX];
    int _points;

    Zone(): _points(0) {
        memset(_index, -1, sizeof(_index));
    }
    bool IsInZone(int row, int col) {
        return _index[row][col] != -1;
    }
    bool IsInZone(const Point &p) {
        return IsInZone(p._row, p._col);
    }
    void AddPoint(int row, int col) {
        if(! IsInZone(row, col)) {
            _pointArray[_points] = Point(row, col);
            _index[row][col] = _points++;
        }
    }
    void AddPoint(const Point &p) {
        AddPoint(p._row, p._col);
    }
    void AddSegment(SegmentTable::Item seg) {
        FOR_EACH(i, 6) {
            AddPoint(seg.GetPoint(i));
        }
    }
    void AddSegmentTable(SegmentTable::Table &tab);
    void Remove(int row, int col) {
        if(IsInZone(row, col)) {
            Point &x = _pointArray[--_points];
            Point &y = _pointArray[_index[row][col]];
            std::swap(x, y);
            std::swap(_index[x._row][x._col], _index[y._row][y._col]);
            _index[row][col] = -1;
        }
    }
    void Remove(const Point &p) {
        Remove(p._row, p._col);
    }
    int Size() {
        return _points;
    }
    Point GetPoint(int index) {
        assert(0 <= index && index < _points);
        return _pointArray[index];
    }
    void Clear();

private:
    Zone(const Zone &);
};