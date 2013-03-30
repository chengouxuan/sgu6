#pragma once

/*
 * 文件定义了着法生成器 MoveGenerator, 着法生成器读取全局棋盘 complexBoard (通过全局函数 ::GetCell()等),
 * 根据局势生成着法, 着法分一下四类:
 * 1. 二堵 (DoubleBlock), 需要二子才能阻止对方在下一手六连取胜, 此时 MoveGenerator 生成所有可能堵法.
 * 2. 一堵 (SingleBlock), 只需要一子就能阻止对方在下一手六连取胜, 此时 MoveGenerator 生成所有可能堵法 (第一子),
 *    然后枚举第二子 (棋盘其他所有可放子位), 二子一同估值估值取前若干, 这个数目由 _SINGLE_BLOCKS 指定.
 * 3. 无需堵 (Promisings), 此时生成的着法由四部分组成:
 *    1) PreBlock, 预先堵, 第一子放在对方可胜的 2, 3 Segment, 枚举第二子, 二子一同估值取前若干对, 所取数目由 _PRE_BLOCKS 指定;
 *    2) PreConnct, 预先连, 第一子放在己方可胜的 1, 2 Segment, 枚举第二子, 二子一同估值取前若干对, 所取数目有 _PRE_CONNECTS 指定;
 *    3) Threats, 生成威胁(不论单双), 第一子放在己方可胜的 3 Segment, 枚举第二子, 二子一同估值取前若干对, 所取数目由 _THREAT_MOVES 指定.
 *    4) ExtremThreats, 用两个子形成至少一个威胁, 数目有 _EXTREM_THREATS 指定.
 *    在以上三种方法都失败(生成 0 个着法)时, 采用以下方法:
 *    ByEvals, 枚举第一子, 单子估值取前若干, 数目由 _SINGLE_EVALS 指定, 前若干再枚举第二子, 二子一同估值取前若干, 数目由 _BY_EVALS 指定.
 *  4. 开局 (Openings), 取第 3 种方法的前 2 种.
 *
 * 文件还定义了 Move 的派生类 MGMove, 和 DMove 的派生类 MGDMove, 都增加成员 int _score, 目的是保存估值分数;
 * 同时定义了保存 MGMove 和 MGDMove 的序列 MGMoveArray 和 MGDMoveArray, 都由模板类 Array 派生.
 *
 * MoveGenerator 存在全局实例 moveGenerator, 由 Frame 全局实例 frame 调用.
 *
 * Frame 调用 Generate() 函数, 同时传递一个 int no 参数, Generate() 返回一个 MGDMoveArray 的引用, 所有内存已在
 * 程序启动时分配, 每个 MGDMoveArray 序列所用内存有 no 指定, 所以务必保证不同时使用两个相同 no 编号.
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
    static const int _NO_MAX = 10;                  // Generate(int no) no 限制
    static const int _SINGLE_BLOCKS = 17;           // GenerateSingleBlocks() 生成数目的限制
    //static const int _MOVES_MAX = 1000;
    //static const int _PROMISING_MOVES_SIZE = 53;
    //static const int _SINGLE_MOVES = 19;
    static const int _THREAT_MOVES = 5;             // GenerateThreats() 生成数目的限制
    //static const int _SINGLE_THREATS = 17;
    //static const int _PRE_BLOCKS = 23;
    //static const int _PROMISINGS = 53;
    //static const int _SINGLE_PROMISINGS = 11;
    static const int _PRE_BLOCKS = 23;              // GeneratePreBlocks() 生成数目的限制
    static const int _PRE_CONNECTS = 17;            // GeneratePreConnect() 生成数目的限制
    static const int _BY_EVALS = 51;                // GenerateByEvals() 生成数目的限制
    static const int _SINGLE_BY_EVALS = 13;         // GenerateSingleByEvals() 生成数目的限制
    //static const int _OPENINGS = 7;
    //static const int _OPENING_EVAL = 83; // “空位估值大于此才枚举”
    static const int _EXTREM_THREATS = 3;           // GenerateExtremThreats() 生成数目的限制

    static const int _blockScore[6];
    static const int _connectScore[6];

    MGDMoveArray _arr[_NO_MAX];                     // 预先分配的内存, 由 Generate 返回

    enum MoveType {
        MOVE_TYPE_NOT_MOVE,
        MOVE_TYPE_DOUBLE_BLOCK,
        MOVE_TYPE_SINGLE_BLOCK,
        MOVE_TYPE_PROMISING,
    } _type;                                        // 所返回着法的类型
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
    // Qualifier: 生成着法
    // Parameter: bool isBlack 黑方为 true, 白方为 false
    // Parameter: int no 编号
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
    // Qualifier: 枚举第二子
    // Parameter: bool isBlack 黑方为 true, 白方为 false
    // Parameter: MGMoveArray & moveArray 第一子着法序列
    // Parameter: int sz 将生成的着法的数目最大值
    // Parameter: MGDMoveArray & dMoveArray 将生成的着法
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