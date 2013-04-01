#pragma once

/*
 * Evaluator 为局面评估, 评估值为以下两者之和:
 * 1. 棋型向量匹配分数.
 * 2. Segment 分数.
 *
 * 在函数实现中看不到有棋盘的参数传递, 因为 Evaluator 所操纵的棋盘 ComplexBoard
 * 的全局实例 cmoplexBoard.
 *
 * Evaluator 有全局实例 evaluator.
 *
 *                                                                          --2011.08.21
 *
 */

#include "Connect6.h"
#include "defines.h"
#include <algorithm>
#include "TranspositionTable.h"
#include "MoveSearcher.h"

unsigned __int64 CheckSum();
unsigned int Hash();

class Evaluator {
public:
    class Item { // 置换表项
    public:
        unsigned __int64 _checkSum;
        int _eval;
        enum Type {
            TYPE_NOT_A_ITEM,
            TYPE_EVAL
        } _type;
        Item()
            : _checkSum(0)
            , _eval(0)
            , _type(TYPE_NOT_A_ITEM) {}
        Item(unsigned __int64 checkSum, int eval)
            : _checkSum(checkSum)
            , _eval(eval)
            , _type(TYPE_EVAL) {}
    };
    class TransTable: public TranspositionTable <Item, 7867687> { // 置换表
    public:
        void Enter(int eval) {
            __super::Enter(Item(::CheckSum(), eval), ::Hash());
        }
        bool LookUp(int &eval) {
            Item item;
            if(__super::LookUp(::CheckSum(), ::Hash(), item)) {
                eval = item._eval;
                return item._type == Item::TYPE_EVAL;
            }
            return false;
        }
    } _transTable;

public:
    static const int _SHAPE_LEN_MAX = 9;                  // 棋型长度最大值
    static const int _SHAPE_LEN_MIN = 6;                  // 棋型长度最小值
    int _score[_SHAPE_LEN_MAX + 1][1 << _SHAPE_LEN_MAX];  // 棋型分数, 通过长度 len 和棋型掩码 mask 索引
    int _evaluations;                                     // 统计
    static const int _segmentScore[6];                    // Segment 分数

    //************************************
    // Method:    ResetCounters
    // FullName:  Evaluator::ResetCounters
    // Access:    public 
    // Returns:   void
    // Qualifier: 统计清零
    //************************************
    void ResetCounters() {
        _evaluations = 0;
        _transTable._hits = 0;
    }

    Evaluator();

    //************************************
    // Method:    Evaluate
    // FullName:  Evaluator::Evaluate
    // Access:    public 
    // Returns:   int
    // Qualifier: 估值
    // Parameter: bool isBlacksTurn 黑方回合为 true, 白方回合为 false
    // Parameter: int depth 深度, 0 表示忽略此参数
    //************************************
    int Evaluate(bool isBlacksTurn, int depth = 0);

private:
    int SimpleEval();

    const static int _LINE_LEN_MAX = 19;

    int LineScore(CellType line[_LINE_LEN_MAX], int len);
};

extern Evaluator evaluator;