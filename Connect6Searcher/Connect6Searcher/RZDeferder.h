#pragma once

/*
 * δ���!
 *
 * Ϊ�˶Ը� "������" �ͳ���, ����ļ������˸��� Relevance Zone ���ӵ��� RZDefender,
 * ������ȫ�� dtsser ��ȡ Relevance Zone.
 * RZDefender ����ȫ�� complexBoard, ���� RZDefender ���κγ�Աǰ, complexBoard �����Ѿ���ʼ��.
 * RZDefender ����ȫ��ʵ�� rzdefender
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
        //int ret = (::GetSegmentTable(m._isBlack, 2).Size() + ::GetSegmentTable(m._isBlack, 3).Size() + ::GetSegmentTable(m._isBlack, 4).Size() + ::GetSegmentTable(m._isBlack, 5).Size())
        //    + 4000 * (-::GetSegmentTable(!m._isBlack, 2).Size() - ::GetSegmentTable(!m._isBlack, 3).Size() * 3);//evaluator.Evaluate(dm._isBlack);
        int ret = evaluator.Evaluate(m._isBlack);
        ::UnmakeLastMove();
        return ret;
    }

    int MoveEvaluate(const Point &p, bool isBlack) {
        return MoveEvaluate(Move(p, isBlack));
    }

    int DMoveEvaluate(const DMove &dm) {
        MakeDMove(dm);
        //int ret = (::GetSegmentTable(dm._isBlack, 2).Size() + ::GetSegmentTable(dm._isBlack, 3).Size() + ::GetSegmentTable(dm._isBlack, 4).Size() + ::GetSegmentTable(dm._isBlack, 5).Size())
        //    + 4000 * (-::GetSegmentTable(!dm._isBlack, 2).Size() - ::GetSegmentTable(!dm._isBlack, 3).Size() * 3);//evaluator.Evaluate(dm._isBlack);
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