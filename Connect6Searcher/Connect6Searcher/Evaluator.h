#pragma once

/*
 * Evaluator Ϊ��������, ����ֵΪ��������֮��:
 * 1. ��������ƥ�����.
 * 2. Segment ����.
 *
 * �ں���ʵ���п����������̵Ĳ�������, ��Ϊ Evaluator �����ݵ����� ComplexBoard
 * ��ȫ��ʵ�� cmoplexBoard.
 *
 * Evaluator ��ȫ��ʵ�� evaluator.
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
    class Item { // �û�����
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
    class TransTable: public TranspositionTable <Item, 7867687> { // �û���
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
    static const int _SHAPE_LEN_MAX = 9;                  // ���ͳ������ֵ
    static const int _SHAPE_LEN_MIN = 6;                  // ���ͳ�����Сֵ
    int _score[_SHAPE_LEN_MAX + 1][1 << _SHAPE_LEN_MAX];  // ���ͷ���, ͨ������ len ���������� mask ����
    int _evaluations;                                     // ͳ��
    static const int _segmentScore[6];                    // Segment ����

    //************************************
    // Method:    ResetCounters
    // FullName:  Evaluator::ResetCounters
    // Access:    public 
    // Returns:   void
    // Qualifier: ͳ������
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
    // Qualifier: ��ֵ
    // Parameter: bool isBlacksTurn �ڷ��غ�Ϊ true, �׷��غ�Ϊ false
    // Parameter: int depth ���, 0 ��ʾ���Դ˲���
    //************************************
    int Evaluate(bool isBlacksTurn, int depth = 0);

private:
    int SimpleEval();

    const static int _LINE_LEN_MAX = 19;

    int LineScore(CellType line[_LINE_LEN_MAX], int len);
};

extern Evaluator evaluator;