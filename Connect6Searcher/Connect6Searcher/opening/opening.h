#pragma once

#include "../connect6.h"
#include <vector>
#include <cstring>
#include <algorithm>

struct BitmaskMove
{
    __int16 bm;
    BitmaskMove(): bm((__int16)-1) {} // all zero is in fact a valid value, so we set all one to make it a invalid value.
};

struct BitmaskMoveCmp
{
    bool operator()(const BitmaskMove &x, const BitmaskMove &y)
    {
        return x.bm < y.bm;
    }
};

void inline BITMASK_MOVE_SET_ROW(BitmaskMove &bm, int row)
{
    bm.bm &= ~((__int16)0x1f);
    bm.bm |= (__int16)row;
}

void inline BITMASK_MOVE_SET_COL(BitmaskMove &bm, int col)
{
    bm.bm &= ~((__int16)0x1f << 5);
    bm.bm |= (__int16)col << 5;
}

void inline BITMASK_MOVE_SET_IS_BLACK(BitmaskMove &bm, bool isBlack)
{
    bm.bm &= ~((__int16)1 << 10);
    if (isBlack) {
        bm.bm |= (__int16)1 << 10;
    }
}

int inline BITMASK_MOVE_GET_ROW(BitmaskMove &bm)
{
    return bm.bm & 0x1f;
}

int inline BITMASK_MOVE_GET_COL(BitmaskMove &bm)
{
    return (bm.bm >> 5) & 0x1f;
}

bool inline BITMASK_MOVE_GET_IS_BLACK(BitmaskMove &bm)
{
    return (bm.bm >> 10) != (__int16)0;
}

bool inline BITMASK_MOVE_IS_VALID(BitmaskMove &bm) {
    return bm.bm != (__int16)-1;
}

struct OpeningItem
{
    static const int MOVES = 9;

    BitmaskMove move[MOVES];

    int toGoR1;
    int toGoC1;
    int toGoR2;
    int toGoC2;
    bool toGoIsBlack;

    int wins;
    int losts;

    OpeningItem(): toGoR1(0), toGoC1(0), toGoR2(0), toGoC2(0), toGoIsBlack(false), wins(0), losts(0) {}

    void SortMoves() { std::sort(move, move + MOVES, BitmaskMoveCmp()); }

    void SetMove(int ind, int row, int col, bool isBlack);

    bool IsMoveValid(int ind);

    bool IsAllMovesEqual(OpeningItem &other) { return memcmp(move, other.move, sizeof(move)) == 0; }

    bool IsToGoEqual(int r1, int c1, int r2, int c2, bool isBlack);

    int GetMovesRow( int ind );
    int GetMovesCol( int ind );
    bool GetMovesIsBlack( int ind );
};

class Opening
{
public:
    Opening() {}
    bool GetDMove(Board board, bool isBlack, DMove *move);
    void BuildTable();

private:
    //int IndexOfsameMovesItemInTable(OpeningItem &item);
    void BoardVerticalReverse(Board boardIn, Board boardOut);
    void BoardRotateClockwise90(Board boardIn, Board boardOut);
    Point PointVerticalReverse(int row, int col) { return Point(ROW_MAX - 1 - row, col); }
    Point PointRotateClockwise90(int row, int col) { return Point(col, ROW_MAX - 1 - row); }
    Point PointRotateCounterClockwise90(int row, int col) { return Point(COL_MAX - 1 - col, row); }
    //bool GetDMove(Board board, DPoint *point);

private:
    std::vector <OpeningItem> table;
    static const char dataDescriptionFileName[];
};