#pragma once

/*
 * 文件定义了搜索框架 Frame, 有全局实例 frame
 * Frame 通过全局函数操纵 complexBoard, segmentTable, moveGenerator
 *
 *                                                           --2011.08.21
 *
 */

#include "connect6.h"
#include "TranspositionTable.h"
#include "MoveSearcher.h"
#include "DTSSer.h"
#include "HistoryScore.h"
#include "MoveGenerator.h"
#include "RZDeferder.h"

class Frame {
public:
    static /*const*/ int _depth_limit/* = 3*/;        // 搜索最大深度
    static const int _DTSS_SCORE = INFINITY;  // 双威胁搜索成功分数
    static /*const*/ int _time_limit/* = 10000*/;     // 时间限制, 在未超过该限制和深度限制 _DEPTH_LIMIT 时进入深一层搜索

    DMove _dMove; // 最佳着法
    int _nodes;   // 结点统计

    class Item { // 置换表项
    public:
        enum Type {
            TYPE_FAIL_HIGH,
            TYPE_FAIL_LOW,
            TYPE_SUCCESS,
            TYPE_NOT_A_ITEM
        } _type;
        int _eval;
        int _depth;
        unsigned __int64 _checkSum;
        DMove _dMove;

        Item(Type type, int eval, int depth, __int64 checkSum, const DMove &dm)
            : _type(type)
            , _eval(eval)
            , _depth(depth)
            , _checkSum(checkSum)
            , _dMove(dm) {}
        Item()
            : _type(TYPE_NOT_A_ITEM)
            , _eval(0)
            , _depth(0)
            , _checkSum(0)
            , _dMove(DMove()) {}
        Item(const Item &ti) {
            memcpy(this, &ti, sizeof(*this));
        }
    };

    class TransTable: public TranspositionTable <Item, 1048583> { // 置换表
    public:
        void Enter(int depth, Item::Type type, int eval, const DMove &dm) {
            __super::Enter(Item(type, eval, depth, ::CheckSum(), dm), ::Hash());
        }
        bool LookUp(int depth, int lower, int upper, int &eval, DMove &dm) {
            Item item;
            if(!__super::LookUp(::CheckSum(), ::Hash(), item) || item._type == Item::TYPE_NOT_A_ITEM || item._depth < depth) {
                return false;
            }
            eval = item._eval;
            dm = item._dMove;
            switch(item._type) {
                case Item::TYPE_SUCCESS: return true;
                case Item::TYPE_FAIL_LOW: return lower >= item._eval;
                case Item::TYPE_FAIL_HIGH: return upper <= item._eval;
                default: assert(item._type == Item::TYPE_NOT_A_ITEM); return false;
            }
        }
    } _transTableBlack, _transTableWinte;

    TransTable &GetTransTable(bool isBlack) {
        return isBlack ? _transTableBlack : _transTableWinte;
    }

private:
    //************************************
    // Method:    NegaMax
    // FullName:  Frame::NegaMax
    // Access:    private 
    // Returns:   int
    // Qualifier: 负极大值
    // Parameter: bool isBlack 黑结点为 true, 白结点为 false
    // Parameter: int depth 深度限制
    // Parameter: int lower alpha 值
    // Parameter: int upper beta 值
    //************************************
    int NegaMax(bool isBlack, int depth, int lower, int upper);

    //int MTDF(bool isBlack, int guess);

    //************************************
    // Method:    IDMTDF
    // FullName:  Frame::IDMTDF
    // Access:    private 
    // Returns:   int
    // Qualifier: 迭代加深 MTD(f)
    // Parameter: bool isBlack 黑方为 true, 白方为 false
    //************************************
    int IDMTDF(bool isBlack);

    //int MiniWind(bool isBlack, int depth, int lower, int upper);

    //************************************
    // Method:    FindWinningMove
    // FullName:  Frame::FindWinningMove
    // Access:    private 
    // Returns:   bool true 找到, false 找不到
    // Qualifier: 形成六连的着法
    // Parameter: bool isBlack 黑方为 true, 白方为 false
    // Parameter: DMove & dm 着法
    //************************************
    bool FindWinningMove(bool isBlack, DMove &dm);

    //************************************
    // Method:    IsGameOver
    // FullName:  Frame::IsGameOver
    // Access:    private 
    // Returns:   bool
    // Qualifier: 是否理论上游戏已经结束
    // Parameter: bool isBlack 黑方回合为 true, 白方回合为 false
    // Parameter: int & eval 若游戏结束此值为结点值
    // Parameter: DMove & dm 若游戏结束此着法为该节点的最佳着法
    //************************************
    bool IsGameOver(bool isBlack, int &eval, DMove &dm) {
        if(FindWinningMove(isBlack, dm)) {
            eval = +INFINITY;
            return true;
        } else if(dtsser.Search(isBlack, true)) {
            eval = _DTSS_SCORE;
            dm = dtsser._dMove;
            return true;
        } else {
            return false;
        }
    }

public:
    //************************************
    // Method:    ResetCounters
    // FullName:  Frame::ResetCounters
    // Access:    public 
    // Returns:   void
    // Qualifier: 统计清零
    //************************************
    void ResetCounters() {
        _nodes = 0;
        _transTableBlack._hits = 0;
        _transTableWinte._hits = 0;
    }

private:
    void NewSearchInit(Board board) {
        complexBoard.NewSearchInit(board);
        historyScore.Zero();
        evaluator.ResetCounters();
        dtsser.ResetCounters();
        this->ResetCounters();
    }

    //************************************
    // Method:    SortByHistoryScore
    // FullName:  Frame::SortByHistoryScore
    // Access:    private static 
    // Returns:   void
    // Qualifier: 着法排序
    // Parameter: MGDMoveArray & 被排序的着法
    //************************************
    static void SortByHistoryScore(MGDMoveArray &);

    //int MiniWind(bool isBlack) {
    //    MiniWind(isBlack, _DEPTH_LIMIT, -INFINITY, +INFINITY);
    //    int eval;
    //    bool b = GetTransTable(isBlack).LookUp(0, -INFINITY, +INFINITY, eval, _dMove);
    //    assert(b);
    //    return eval;
    //}

    //int Aspiration(bool isBlack, int guess, int wind) {
    //    int scr = MiniWind(isBlack, _DEPTH_LIMIT, guess - wind, guess + wind);
    //    if(guess - wind < scr && scr < guess + wind) {
    //        printf("aspiration success\n");
    //        GetTransTable(isBlack).LookUp(0, guess - wind, guess + wind, scr, _dMove);
    //    } else if(scr <= guess - wind) {
    //        printf("aspiration fail low\n");
    //        MiniWind(isBlack, _DEPTH_LIMIT, -INFINITY, guess - wind);
    //        GetTransTable(isBlack).LookUp(0, -INFINITY, guess - wind, scr, _dMove);
    //    } else {
    //        printf("aspiration fail high\n");
    //        MiniWind(isBlack, _DEPTH_LIMIT, guess + wind, +INFINITY);
    //        GetTransTable(isBlack).LookUp(0, guess + wind, +INFINITY, scr, _dMove);
    //    }
    //    return scr;
    //}

public:
    //************************************
    // Method:    Search
    // FullName:  Frame::Search
    // Access:    public 
    // Returns:   void
    // Qualifier: 搜索, 外部调用掉用此函数进行搜索
    // Parameter: Board board 棋盘数组
    // Parameter: bool isBlack 黑方回合为 true, 白方回合为 false
    //************************************
    void Search(Board board, bool isBlack) {
        NewSearchInit(board);
        //DMove dm;
        searcher.SetDMove(RandomDMove(isBlack));
        //if(FindWinningMove(isBlack, _dMove)) {
        //    printf("win\n");
        //    searcher.SetDMove(_dMove);
        //    return;
        //} else if(dtsser.Search(isBlack, true)) {
        //    printf("dtss\n");
        //    searcher.SetDMove(dtsser._dMove);
        //    return;
        //} else if(!FindWinningMove(!isBlack, dm) && rzdefender.Defende(isBlack)) {
        //    searcher.SetDMove(rzdefender._dMove);
        //    printf("relevance zone defende\n");
        //    return;
        //}
        IDMTDF(isBlack);
        //MTDF(isBlack, NegaMax(isBlack, _DEPTH_LIMIT - 2, -INFINITY, +INFINITY));
        //MiniWind(isBlack);
        //Aspiration(isBlack, NegaMax(isBlack, _DEPTH_LIMIT - 2, -INFINITY, +INFINITY), 123456);
    }
};

extern Frame frame;