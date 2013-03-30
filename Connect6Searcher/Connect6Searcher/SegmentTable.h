#pragma once

#include "Connect6.h"
#include "defines.h"
#include "utilities.h"
#include <cassert>
#include <algorithm>

class SegmentTable {
public:
    class Item { // an item is a segment
    public:
        int _row; // row of segment's beginning.
        int _col; // col of segment's beginning.
        int _dir; // direction: [0, 4).
        int _whites;
        int _blacks;

        Item() {}
        Item(int row, int col, int dir, int whites, int blacks)
            : _row(row)
            , _col(col)
            , _dir(dir)
            , _whites(whites)
            , _blacks(blacks) {}
        bool IsInsideBoard() {
            assert(::IsInsideBoard(_row, _col));
            assert(0 <= _dir && _dir < 4);
            return ::IsInsideBoard(_row + dr[_dir] * 5, _col + dc[_dir] * 5);
        }
        CellType GetCell(int i);
        int GetRow(int i) {
            return _row + dr[_dir] * i;
        }
        int GetCol(int i) {
            return _col + dc[_dir] * i;
        }
        Point GetPoint(int i) {
            return Point(GetRow(i), GetCol(i));
        }
    };

    class Table {
        int _index[ROW_MAX][COL_MAX][4];
        Item _itemArray[ROW_MAX * COL_MAX * 4];
        int _items;

    public:
        Table(): _items(0) {
            memset(_index, -1, sizeof(_index));
        }
        bool IsInTable(const Item &item) {
            return _index[item._row][item._col][item._dir] != -1;
        }
        void Add(const Item &item) {
            assert(! IsInTable(item));
            _itemArray[_items] = item;
            _index[item._row][item._col][item._dir] = _items++;
        }
        void Remove(const Item &item) {
            assert(IsInTable(item));
            Item &x = _itemArray[_index[item._row][item._col][item._dir]];
            Item &y = _itemArray[--_items];
            std::swap(x, y);
            std::swap(_index[x._row][x._col][x._dir], _index[y._row][y._col][y._dir]);
            _index[item._row][item._col][item._dir] = -1;
        }
        int Size() {
            return _items;
        }
        Item GetItem(int ind) {
            assert(0 <= ind && ind < _items);
            return _itemArray[ind];
        }
        void Clear() {
            _items = 0;
            memset(_index, -1, sizeof(_index));
        }

    private:
        Table(const Table &);
    } _table[7][7]; // access by blacks and whites.
    Item _segment[ROW_MAX][COL_MAX][4]; // access by row, column and direction.

private:
};

extern SegmentTable segmentTable; // maintained by complexBoard and any class can access its members.