#pragma once

#include "connect6.h"
#include "ComplexBoard.h"
#include "HistoryScore.h"
//#include "DMoveArray.h"
#include "MoveSearcher.h"
#include "MoveArray.h"
#include "Zone.h"
#include "Array.h"

class MGDMove: public DMove { // MoveGeneratorDMove
public:
    int _score;
    MGDMove(int r1, int c1, int r2, int c2, bool isBlack, int score)
        : DMove(r1, c1, r2, c2, isBlack)
        , _score(score) {}
    MGDMove(const Point &p1, const Point &p2, bool isBlack, int score)
        : DMove(p1, p2, isBlack)
        , _score(score) {}
    MGDMove(const DMove &dm, int score)
        : DMove(dm)
        , _score(score) {}
    MGDMove() {}
    MGDMove(const MGDMove &m)
        : DMove(m._r1, m._c1, m._r2, m._c2, m._isBlack)
        , _score(m._score) {}
};

class MGDMoveEqual {
public:
    bool operator () (const MGDMove &x, const MGDMove &y) {
        return DMoveEqual()(x, y) && x._score == y._score;
    }
};

class MGDMoveArray: public Array <MGDMove, ROW_MAX * COL_MAX * ROW_MAX * COL_MAX, MGDMoveEqual> {
public:
    void PushBack(const MGDMove &m) {
        __super::PushBack(m);
    }
    void PushBack(int r1, int c1, int r2, int c2, bool isBlack, int score) {
        __super::PushBack(MGDMove(r1, c1, r2, c2, isBlack, score));
    }
    void PushBack(const Point &p1, const Point &p2, bool isBlack, int score) {
        __super::PushBack(MGDMove(p1, p2, isBlack, score));
    }
    void PushBack(const DMove &dm, int score) {
        __super::PushBack(MGDMove(dm, score));
    }
    bool Find(int r1, int c1, int r2, int c2, bool isBlack, int score) {
        return __super::Find(MGDMove(r1, c1, r2, c2, isBlack, score));
    }
    bool Find(const Point &p1, const Point &p2, bool isBlack, int score) {
        return __super::Find(MGDMove(p1, p2, isBlack, score));
    }
    bool Find(const DMove &dm, int score) {
        return __super::Find(MGDMove(dm, score));
    }
    MGDMove &MinScoreRef();
};

class MGMove: public Move {
public:
    int _score;
    MGMove(int row, int col, bool isBlack, int score)
        : Move(row, col, isBlack)
        , _score(score) {}
    MGMove(const Point &p, bool isBlack, int score)
        : Move(p, isBlack)
        , _score(score) {}
    MGMove(const Move &m, int score)
        : Move(m)
        , _score(score) {}
    MGMove(const MGMove &m)
        : Move(m._row, m._col, m._isBlack)
        , _score(m._score) {}
    MGMove() {}
};

class MGMoveEqual {
public:
    bool operator () (const MGMove &x, const MGMove &y) {
        return MoveEqual()(x, y) && x._score == y._score;
    }
};

class MGMoveArray: public Array <MGMove, ROW_MAX * COL_MAX, MGMoveEqual> {
public:
    void PushBack(int row, int col, bool isBlack, int score) {
        __super::PushBack(MGMove(row, col, isBlack, score));
    }
    void PushBack(const Point &p, bool isBlack, int score) {
        __super::PushBack(MGMove(p, isBlack, score));
    }
    void PushBack(const Move &m, int score) {
        __super::PushBack(MGMove(m, score));
    }
    bool Find(int row, int col, bool isBlack, int score) {
        return __super::Find(MGMove(row, col, isBlack, score));
    }
    bool Find(const Point &p, bool isBlack, int score) {
        return __super::Find(MGMove(p, isBlack, score));
    }
    bool Find(const Move &m, int score) {
        return __super::Find(MGMove(m, score));
    }
    MGMove &MinScoreRef();
};

class MoveGenerator {
public:
    static const int _NO_MAX = 10;
    static const int _SINGLE_BLOCKS = 17;
    //static const int _MOVES_MAX = 1000;
    //static const int _PROMISING_MOVES_SIZE = 53;
    //static const int _SINGLE_MOVES = 19;
    static const int _THREAT_MOVES = 7;
    //static const int _SINGLE_THREATS = 17;
    //static const int _PRE_BLOCKS = 23;
    //static const int _PROMISINGS = 53;
    //static const int _SINGLE_PROMISINGS = 11;
    static const int _PRE_BLOCKS = 47;
    static const int _BY_EVALS = 51;
    static const int _SINGLE_BY_EVALS = 13;
    static const int _OPENINGS = 7;
    static const int _OPENING_EVAL = 83; // “空位估值大于此才枚举”

    static const int _blockScore[6];
    static const int _connectScore[6];

    MGDMoveArray _arr[_NO_MAX];

    enum MoveType {
        MOVE_TYPE_NOT_MOVE,
        MOVE_TYPE_DOUBLE_BLOCK,
        MOVE_TYPE_SINGLE_BLOCK,
        MOVE_TYPE_PROMISING,
    } _type;
    //static const int _SCORE_TAB_SZ = 5;
    //int _connectScore[_SCORE_TAB_SZ];
    //int _blockScore[_SCORE_TAB_SZ];

    //MoveGenerator();

    // generated moves are stored in static memory specified by variable ind,
    // be sure that not any two nodes use same value of ind currently.
    MGDMoveArray &Generate(bool isBlack, int no);

private:
    void GeneratePromisings(bool isBlack, MGDMoveArray &arr);
    void GenerateDoubleBlocks(bool isBlack, MGDMoveArray &arr);
    void GenerateSingleBlocks(bool isBlack, MGDMoveArray &arr);

    // “空位估值”
    void GenerateOpenings(bool isBlack, MGDMoveArray &arr);

    void SingleBlocks(bool isBlack, MGMoveArray &block);
    //void CoupleSingleBlocks(bool isBlack, MGMoveArray &block, MGDMoveArray &arr);
    //void CoupleSingleBlocks(bool isBlack, Zone &zone, MGMoveArray &block, MGDMoveArray &arr);

    void SingleByEvals(bool isBlack, int sz, MGMoveArray &arr);
    void SingleByEvals(bool isBlack, Zone &zone, int sz, MGMoveArray &moveArray);

    void Couple(bool isBlack, MGMoveArray &moveArray, int sz, MGDMoveArray &dMoveArray);
    void Couple(bool isBlack, Zone &zone, MGMoveArray &moveArray, int sz, MGDMoveArray &dMoveArray);

    void GenerateThreats(bool isBlack, MGDMoveArray &arr);
    //void CoupleThreats(bool isBlack, MGMoveArray &threat, MGDMoveArray &arr);
    void GeneratePreBlock(bool isBlack, MGDMoveArray &preBlock);
    void GenerateByEvals( bool isBlack, MGDMoveArray &arr );
    void GeneratePreConnect(bool isBlack, MGDMoveArray &arr);

    bool IsOpening(bool isBlack) {
        return ::Stones() < 13 &&
            ::GetSegmentTable(!isBlack, 2).Size() < 7 && ::GetSegmentTable(!isBlack, 3).Size() < 3  &&
            ::GetSegmentTable(isBlack, 2).Size() < 11 && ::GetSegmentTable(isBlack, 3).Size() < 5;
    }

    int MoveEvaluate(int row, int col, bool isBlack) {
        ::MakeMove(row, col, isBlack);
        int ret = evaluator.Evaluate(isBlack);
        ::UnmakeLastMove();
        return ret;
    }
    int MoveEvaluate(const Point &p, bool isBlack) {
        return MoveEvaluate(p._row, p._col, isBlack);
    }
    int DMoveEvaluate(const DMove &dm) {
        ::MakeDMove(dm);
        int ret = evaluator.Evaluate(dm._isBlack);
        ::UnmakeLastDMove();
        return ret;
    }
    int DMoveEvaluate(const Point &p1, const Point &p2, bool isBlack) {
        return DMoveEvaluate(DMove(p1, p2, isBlack));
    }
};

extern MoveGenerator moveGenerator;

//const int MoveGenerator::_blockScore[6] = {0, 2, 83, 727, 997, 1259};