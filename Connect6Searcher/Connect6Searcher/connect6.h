#pragma once

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>

#ifdef  UNICODE                     // r_winnt

#ifndef _TCHAR_DEFINED
#define tsprintf wsprintf
#endif /* !_TCHAR_DEFINED */

#else   /* UNICODE */               // r_winnt

#ifndef _TCHAR_DEFINED
#define tsprintf wsprintf
#endif /* !_TCHAR_DEFINED */

#endif /* UNICODE */                // r_winnt

const int ROW_MAX = 19;
const int COL_MAX = 19;
const int INFINITY = 1000000000;

#ifdef NDEBUG
#undef assert
#define assert(exp) (((exp) || fprintf(stderr, "assertion %s failed on line %d, file %s\n", #exp, __LINE__, __FILE__)), (void)0)
#endif

enum CellType {
	CELL_TYPE_BLACK,
	CELL_TYPE_WHITE,
	CELL_TYPE_EMPTY,
	CELL_TYPE_NOT_A_CELL
};
//
//class BoardsRow
//{
//    CellType cell[COL_MAX];
//
//public:
//    BoardsRow();
//
//    CellType &operator[](int c)
//    {
//        assert(0 <= c && c < COL_MAX);
//        return cell[c];
//    }
//};
//
//class Board
//{
//    BoardsRow row[ROW_MAX];
//
//public:
//    BoardsRow &operator[](int r)
//    {
//        assert(0 <= r && r < ROW_MAX);
//        return row[r];
//    }
//
//    Board() {}
//
//    Board(CellType cell[ROW_MAX][COL_MAX])
//    {
//        for (int r=0; r<ROW_MAX; ++r) {
//            for (int c=0; c<COL_MAX; ++c) {
//                row[r][c] = cell[r][c];
//            }
//        }
//    }
//};

typedef CellType Board[ROW_MAX][COL_MAX];

struct SearchData {
    CellType _board[ROW_MAX][COL_MAX];
    bool _isBlack;
    int _moves;
};

const int PARAM_SIZE_MAX = 1024;

struct RequestData {
    enum Type {
        TYPE_EXIT,
        TYPE_SEARCH,
        TYPE_PARAM,
        TYPE_TERM_SEARCH,
        TYPE_BREAK_ALL_SEARCH
    } _type;
    union Data {
        SearchData _searchData;
        int _param[PARAM_SIZE_MAX];
    } _data;
};

struct MoveData {
    int _r1, _c1, _r2, _c2;
    MoveData() {}
    MoveData(int r1, int c1, int r2, int c2): _r1(r1), _c1(c1), _r2(r2), _c2(c2) {}
};

class Point {
public:
	int _row, _col;

	Point(): _row(0), _col(0) {}
	Point(int row, int col): _row(row), _col(col) {}
	Point(const Point &p) {
		memcpy(this, &p, sizeof(*this));
	}
};

inline bool operator < (const Point &x, const Point &y) {
    return x._row * COL_MAX + x._col < y._row * COL_MAX + y._col;
}

inline bool operator == (const Point &x, const Point &y) {
    return x._row == y._row && x._col == y._col;
}

inline bool operator != (const Point &x, const Point &y) {
    return ! (x == y);
}

class DPoint {
public:
    int _r1, _c1, _r2, _c2;
    DPoint() {}
    DPoint(int r1, int c1, int r2, int c2)
        : _r1(r1)
        , _c1(c1)
        , _r2(r2)
        , _c2(c2) {}
    Point GetPoint1() const {
        return Point(_r1, _c1);
    }
    Point GetPoint2() const {
        return Point(_r2, _c2);
    }
    //bool IsEqual(const DPoint &dp) {
    //    if(this->GetPoint1() == dp.GetPoint1() && this->GetPoint2() == dp.GetPoint2()) {
    //        return true;
    //    } else if(this->GetPoint2() == dp.GetPoint1() && this->GetPoint1() == dp.GetPoint2()) {
    //        return true;
    //    } else {
    //        return false;
    //    }
    //}
};

#include <algorithm>

CellType GetCell(int, int);

class Move {
public:
    int _row, _col;
	bool _isBlack; // true 放黑子，false 放白子

	Move(int row, int col, bool isBlack): _row(row), _col(col), _isBlack(isBlack) {}
    Move(const Point &p, bool isBlack): _row(p._row), _col(p._col), _isBlack(isBlack) {}
	Move(): _isBlack(true) {}
	Move(const Move &m) {
		memcpy(this, &m, sizeof(*this));
	}
    Point GetPoint() const {
        return Point(_row, _col);
    }
    //bool IsEqual(int row, int col, bool isBlack) {
    //    return !isBlack == !_isBlack && row == _row && col == _col;
    //}
    //bool IsEqual(const Point &p, bool isBlack) {
    //    return IsEqual(p._row, p._col, isBlack);
    //}
    bool IsValid() {
        return ::GetCell(_row, _col) == CELL_TYPE_EMPTY;
    }
};

class MoveEqual {
public:
    bool operator () (const Move &x, const Move &y) {
        return x._row == y._row && x._col == y._col && !x._isBlack == !y._isBlack;
    }
};

class DMove { // Double Move
public:
    int _r1, _c1, _r2, _c2;
    bool _isBlack;

    DMove() {}
    DMove(int r1, int c1, int r2, int c2, bool isBlack)
        : _r1(r1)
        , _c1(c1)
        , _r2(r2)
        , _c2(c2)
        , _isBlack(isBlack) {}
    DMove(const Point &p1, const Point &p2, bool isBlack)
        : _r1(p1._row)
        , _c1(p1._col)
        , _r2(p2._row)
        , _c2(p2._col)
        , _isBlack(isBlack) {}
    DMove(const DMove &dm) {
        memcpy(this, &dm, sizeof(*this));
    }
    Point GetPoint1() const {
        return Point(_r1, _c1);
    }
    Point GetPoint2() const {
        return Point(_r2, _c2);
    }
    void Print() {
        printf("%s (%02d, %c), (%02d, %c)", _isBlack ? "B" : "W", _r1, _c1 + 'A', _r2, _c2 + 'A');
    }
    //bool IsEqual(const DMove &dm) const {
    //    if(!this->_isBlack != !dm._isBlack) {
    //        return false;
    //    } else if(this->GetPoint1() == dm.GetPoint1() && this->GetPoint2() == dm.GetPoint2()) {
    //        return true;
    //    } else if(this->GetPoint2() == dm.GetPoint1() && this->GetPoint1() == dm.GetPoint2()) {
    //        return true;
    //    }
    //    return false;
    //}
    bool IsValid() {
        return GetPoint1() != GetPoint2() && ::GetCell(_r1, _c1) == CELL_TYPE_EMPTY && ::GetCell(_r2, _c2) == CELL_TYPE_EMPTY;
    }
};

class DMoveEqual {
public:
    bool operator () (const DMove &x, const DMove &y) {
        if(!x._isBlack != !y._isBlack) {
            return false;
        } else if(x.GetPoint1() == y.GetPoint1() && x.GetPoint2() == y.GetPoint2()) {
            return true;
        } else {
            return x.GetPoint2() == y.GetPoint1() && x.GetPoint1() == y.GetPoint2();
        }
    }
};
