#include "ComplexBoard.h"

void ComplexBoard::NewSearchInit(Board board) {
    FOR_EACH(b, 7) {
        FOR_EACH(w, 7) {
            segmentTable._table[b][w].Clear();
        }
    }
    FOR_EACH_SEG(r, c, d) {
        SegmentTable::Item &item = (segmentTable._segment[r][c][d] = SegmentTable::Item(r, c, d, 0, 0));
        if(item.IsInsideBoard()) {
            segmentTable._table[0][0].Add(item);
        }
    }
    _hash = 0;
    _checkSum = 0;
    _stack.Clear();
    std::fill(_data[0], _data[ROW_MAX], CELL_TYPE_EMPTY);
    FOR_EACH_CELL(r, c) {
        if(board[r][c] == CELL_TYPE_BLACK) {
            MakeMove(r, c, true);
        } else if(board[r][c] == CELL_TYPE_WHITE) {
            MakeMove(r, c, false);
        } else {
            assert(board[r][c] == CELL_TYPE_EMPTY);
        }
    }
}

void ComplexBoard::MakeMove(int row, int col, bool isBlack) {
    assert(::IsInsideBoard(row, col));
    assert(_data[row][col] == CELL_TYPE_EMPTY);

    _hash ^= ::randTable.Rand32(row, col, _data[row][col]);
    _checkSum ^= ::randTable.Rand64(row, col, _data[row][col]);

    _data[row][col] = (isBlack ? CELL_TYPE_BLACK : CELL_TYPE_WHITE);

    _hash ^= ::randTable.Rand32(row, col, _data[row][col]);
    _checkSum ^= ::randTable.Rand64(row, col, _data[row][col]);

    FOR_EACH(i, 4) {
        UpdateDirection(row, col, i, isBlack, true);
    }

    _stack.Push(row, col, isBlack);
}

void ComplexBoard::UnmakeLastMove() {
    int row = _stack.Top()._row;
    int col = _stack.Top()._col;

    assert(_data[row][col] != CELL_TYPE_EMPTY);

    _hash ^= ::randTable.Rand32(row, col, _data[row][col]);
    _checkSum ^= ::randTable.Rand64(row, col, _data[row][col]);

    _data[row][col] = CELL_TYPE_EMPTY;

    _hash ^= ::randTable.Rand32(row, col, _data[row][col]);
    _checkSum ^= ::randTable.Rand64(row, col, _data[row][col]);

    FOR_EACH(i, 4) {
        UpdateDirection(row, col, i, _stack.Top()._isBlack, false);
    }

    _stack.Pop();
}

void ComplexBoard::UpdateDirection(int row, int col, int dir, bool isBlack, bool isIncrease) {
    FOR_EACH(i, 6) {
        if(! ::IsInsideBoard(row - dr[dir] * i, col - dc[dir] * i)) {
            return;
        }
        UpdateSegment(row - dr[dir] * i, col - dc[dir] * i, dir, isBlack, isIncrease);
    }
}

ComplexBoard complexBoard; // tightly coupled with segmentTable
