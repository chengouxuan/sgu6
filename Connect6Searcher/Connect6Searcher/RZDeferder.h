#pragma once

/*
 * 未完成!
 *
 * 为了对付 "疯狂进攻" 型程序, 这个文件定义了根据 Relevance Zone 落子的类 RZDefender,
 * 将调用全局 dtsser 获取 Relevance Zone.
 * RZDefender 访问全局 complexBoard, 调用 RZDefender 的任何成员前, complexBoard 必须已经初始化.
 * RZDefender 存在全局实例 rzdefender
 *
 */

#include "connect6.h"
#include "MoveSearcher.h"
#include "MoveGenerator.h"

class RZDefender {
public:
    DMove _dMove;

    bool Defende(bool isBlack);

    void SortMGMoveByScore(MGMoveArray &arr);

    int MoveEvaluate(const Move &m) {
        ::MakeMove(m);
        int ret = evaluator.Evaluate(m._isBlack);
        ::UnmakeLastMove();
        return ret;
    }

    int MoveEvaluate(const Point &p, bool isBlack) {
        return MoveEvaluate(Move(p, isBlack));
    }

    int DMoveEvaluate(const DMove &dm) {
        MakeDMove(dm);
        int ret = evaluator.Evaluate(dm._isBlack);
        ::UnmakeLastDMove();
        return ret;
    }

    int DMoveEvaluate(const Point &p1, const Point &p2, bool isBlack) {
        return DMoveEvaluate(DMove(p1, p2, isBlack));
    }

    void EnumSecondMove(const Move &move, MGDMoveArray &arr);
};

extern RZDefender rzdefender;