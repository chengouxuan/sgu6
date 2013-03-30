#pragma once

/*
 * �ļ��������ŷ������� MoveGenerator, �ŷ���������ȡȫ������ complexBoard (ͨ��ȫ�ֺ��� ::GetCell()��),
 * ���ݾ��������ŷ�, �ŷ���һ������:
 * 1. ���� (DoubleBlock), ��Ҫ���Ӳ�����ֹ�Է�����һ������ȡʤ, ��ʱ MoveGenerator �������п��ܶ·�.
 * 2. һ�� (SingleBlock), ֻ��Ҫһ�Ӿ�����ֹ�Է�����һ������ȡʤ, ��ʱ MoveGenerator �������п��ܶ·� (��һ��),
 *    Ȼ��ö�ٵڶ��� (�����������пɷ���λ), ����һͬ��ֵ��ֵȡǰ����, �����Ŀ�� _SINGLE_BLOCKS ָ��.
 * 3. ����� (Promisings), ��ʱ���ɵ��ŷ����Ĳ������:
 *    1) PreBlock, Ԥ�ȶ�, ��һ�ӷ��ڶԷ���ʤ�� 2, 3 Segment, ö�ٵڶ���, ����һͬ��ֵȡǰ���ɶ�, ��ȡ��Ŀ�� _PRE_BLOCKS ָ��;
 *    2) PreConnct, Ԥ����, ��һ�ӷ��ڼ�����ʤ�� 1, 2 Segment, ö�ٵڶ���, ����һͬ��ֵȡǰ���ɶ�, ��ȡ��Ŀ�� _PRE_CONNECTS ָ��;
 *    3) Threats, ������в(���۵�˫), ��һ�ӷ��ڼ�����ʤ�� 3 Segment, ö�ٵڶ���, ����һͬ��ֵȡǰ���ɶ�, ��ȡ��Ŀ�� _THREAT_MOVES ָ��.
 *    4) ExtremThreats, ���������γ�����һ����в, ��Ŀ�� _EXTREM_THREATS ָ��.
 *    ���������ַ�����ʧ��(���� 0 ���ŷ�)ʱ, �������·���:
 *    ByEvals, ö�ٵ�һ��, ���ӹ�ֵȡǰ����, ��Ŀ�� _SINGLE_EVALS ָ��, ǰ������ö�ٵڶ���, ����һͬ��ֵȡǰ����, ��Ŀ�� _BY_EVALS ָ��.
 *  4. ���� (Openings), ȡ�� 3 �ַ�����ǰ 2 ��.
 *
 * �ļ��������� Move �������� MGMove, �� DMove �������� MGDMove, �����ӳ�Ա int _score, Ŀ���Ǳ����ֵ����;
 * ͬʱ�����˱��� MGMove �� MGDMove ������ MGMoveArray �� MGDMoveArray, ����ģ���� Array ����.
 *
 * MoveGenerator ����ȫ��ʵ�� moveGenerator, �� Frame ȫ��ʵ�� frame ����.
 *
 * Frame ���� Generate() ����, ͬʱ����һ�� int no ����, Generate() ����һ�� MGDMoveArray ������, �����ڴ�����
 * ��������ʱ����, ÿ�� MGDMoveArray ���������ڴ��� no ָ��, ������ر�֤��ͬʱʹ��������ͬ no ���.
 *
 *                                                                                                      --2011.08.22
 *
 */

#include "connect6.h"
#include "ComplexBoard.h"
#include "HistoryScore.h"
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

class MGDMoveArray: public Array <MGDMove, ROW_MAX * COL_MAX * ROW_MAX * COL_MAX, MGDMoveEqual> { // MoveGenerator DMove Array
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
    bool Find(const MGDMove &m) {
        return __super::Find(m);
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
    MGDMove &MaxScoreRef();
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
    static const int _NO_MAX = 10;                  // Generate(int no) no ����
    static const int _SINGLE_BLOCKS = 17;           // GenerateSingleBlocks() ������Ŀ������
    //static const int _MOVES_MAX = 1000;
    //static const int _PROMISING_MOVES_SIZE = 53;
    //static const int _SINGLE_MOVES = 19;
    static const int _THREAT_MOVES = 5;             // GenerateThreats() ������Ŀ������
    //static const int _SINGLE_THREATS = 17;
    //static const int _PRE_BLOCKS = 23;
    //static const int _PROMISINGS = 53;
    //static const int _SINGLE_PROMISINGS = 11;
    static const int _PRE_BLOCKS = 23;              // GeneratePreBlocks() ������Ŀ������
    static const int _PRE_CONNECTS = 17;            // GeneratePreConnect() ������Ŀ������
    static const int _BY_EVALS = 51;                // GenerateByEvals() ������Ŀ������
    static const int _SINGLE_BY_EVALS = 13;         // GenerateSingleByEvals() ������Ŀ������
    //static const int _OPENINGS = 7;
    //static const int _OPENING_EVAL = 83; // ����λ��ֵ���ڴ˲�ö�١�
    static const int _EXTREM_THREATS = 3;           // GenerateExtremThreats() ������Ŀ������

    static const int _blockScore[6];
    static const int _connectScore[6];

    MGDMoveArray _arr[_NO_MAX];                     // Ԥ�ȷ�����ڴ�, �� Generate ����

    enum MoveType {
        MOVE_TYPE_NOT_MOVE,
        MOVE_TYPE_DOUBLE_BLOCK,
        MOVE_TYPE_SINGLE_BLOCK,
        MOVE_TYPE_PROMISING,
    } _type;                                        // �������ŷ�������
    //static const int _SCORE_TAB_SZ = 5;
    //int _connectScore[_SCORE_TAB_SZ];
    //int _blockScore[_SCORE_TAB_SZ];

    //MoveGenerator();

    // generated moves are stored in static memory specified by variable ind,
    // be sure that not any two nodes use same value of ind currently.
    //************************************
    // Method:    Generate
    // FullName:  MoveGenerator::Generate
    // Access:    public 
    // Returns:   MGDMoveArray &
    // Qualifier: �����ŷ�
    // Parameter: bool isBlack �ڷ�Ϊ true, �׷�Ϊ false
    // Parameter: int no ���
    //************************************
    MGDMoveArray &Generate(bool isBlack, int no);

private:
    void GeneratePromisings(bool isBlack, MGDMoveArray &arr);
    void GenerateDoubleBlocks(bool isBlack, MGDMoveArray &arr);
    void GenerateSingleBlocks(bool isBlack, MGDMoveArray &arr);

    void GenerateOpenings(bool isBlack, MGDMoveArray &arr);

    void SingleBlocks(bool isBlack, MGMoveArray &block);
    //void CoupleSingleBlocks(bool isBlack, MGMoveArray &block, MGDMoveArray &arr);
    //void CoupleSingleBlocks(bool isBlack, Zone &zone, MGMoveArray &block, MGDMoveArray &arr);

    void SingleByEvals(bool isBlack, int sz, MGMoveArray &arr);
    void SingleByEvals(bool isBlack, Zone &zone, int sz, MGMoveArray &moveArray);

    //************************************
    // Method:    Couple
    // FullName:  MoveGenerator::Couple
    // Access:    private 
    // Returns:   void
    // Qualifier: ö�ٵڶ���
    // Parameter: bool isBlack �ڷ�Ϊ true, �׷�Ϊ false
    // Parameter: MGMoveArray & moveArray ��һ���ŷ�����
    // Parameter: int sz �����ɵ��ŷ�����Ŀ���ֵ
    // Parameter: MGDMoveArray & dMoveArray �����ɵ��ŷ�
    //************************************
    void Couple(bool isBlack, MGMoveArray &moveArray, int sz, MGDMoveArray &dMoveArray);
    void Couple(bool isBlack, Zone &zone, MGMoveArray &moveArray, int sz, MGDMoveArray &dMoveArray);

    void GenerateThreats(bool isBlack, MGDMoveArray &arr);

    //void CoupleThreats(bool isBlack, MGMoveArray &threat, MGDMoveArray &arr);

    void GeneratePreBlock(bool isBlack, MGDMoveArray &preBlock);

    void GeneratePreConnect(bool isBlack, MGDMoveArray &arr);

    void GenerateByEvals(bool isBlack, MGDMoveArray &arr);

    void GenerateExtremThreats(bool isBlack, MGDMoveArray &arr);

    bool IsOpening(bool isBlack) {
        return false;
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