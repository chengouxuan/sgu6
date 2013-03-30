#pragma once

/*
 * ComplexBoard �洢��ά�����ֹ������̵���Ϣ:
 * 1. �������� _data;
 * 2. ��ϣֵ _hash;
 * 3. У��� _checkSum;
 * 4. ���Ӷ�ջ _stack;
 *
 * ComplexBoard ͬʱά�� SegmentTable, SegmentTable ��Ӧ��������, ���ڱ���
 * ����Ƕ�ײ���̫��Ŀ���, ComplexBoard ��д�� ComplexBoard ֮��, ������
 * ������һ���ļ� "SegmentTable.h" ֮��.
 *
 * ComplexBoard ����һ��ȫ��ʵ�� complexBoard.
 *
 * �ļ�ͬʱ���������Ӷ�ջ�� MoveStack, �� MoveArray ����.
 *
 *                                                                  --2011.08.21
 *
 */

#include "Connect6.h"
#include "RandTable.h"
#include "utilities.h"
#include "SegmentTable.h"
#include <cassert>
#include "MoveArray.h"

class MoveStack: private MoveArray {
public:
    MoveStack() {}
    int Size() {
        return __super::Size();
    }
    void Clear() {
        __super::Clear();
    }
    void Push(int row, int col, bool isBlack) {
        PushBack(row, col, isBlack);
    }
    void Push(const Point &p, bool isBlack) {
        PushBack(p, isBlack);
    }
    void Push(const Move &m) {
        PushBack(m);
    }
    void Pop() {
        __super::PopBack();
    }
    Move Top() {
        return ItemRef(Size() - 1);
    }

private:
    MoveStack(const MoveStack &);
};

class ComplexBoard {
public:
    unsigned int _hash;               // ���̹�ϣֵ (zobrist ����)
    unsigned __int64 _checkSum;       // ����У��� (zobrist ����)
    CellType _data[ROW_MAX][COL_MAX]; // ���̻�������
    MoveStack _stack;                 // ���Ӷ�ջ

    //************************************
    // Method:    GetCell
    // FullName:  ComplexBoard::GetCell
    // Access:    public 
    // Returns:   CellType ���ӵ�����
    // Qualifier: ȡ������ĳһ���еĸ��ӵ�����
    // Parameter: int row ��
    // Parameter: int col ��
    //************************************
    CellType GetCell(int row, int col) {
        assert(::IsInsideBoard(row, col));
        return _data[row][col];
    }

    //************************************
    // Method:    GetCell
    // FullName:  ComplexBoard::GetCell
    // Access:    public 
    // Returns:   CellType ���ӵ�����
    // Qualifier: ȡ������ĳһ���еĸ��ӵ�����
    // Parameter: const Point & p ָ������ֵ�ĵ�
    //************************************
    CellType GetCell(const Point &p) {
        return GetCell(p._row, p._col);
    }

private:
    //************************************
    // Method:    UpdateSegment
    // FullName:  ComplexBoard::UpdateSegment
    // Access:    private 
    // Returns:   void
    // Qualifier: ���� (MakeMove) ��ȡ������ (UnmakeMove) ��ʱ����Ҫ�������̸�����Ϣ SegmentTable,
    //            Segment Ϊ SegmentTable ��һ��Ԫ��, �˺�������һ��Ԫ��
    // Parameter: int row ��
    // Parameter: int col ��
    // Parameter: int dir ����
    // Parameter: bool isBlack ��ɫΪ true, ��ɫΪ false
    // Parameter: bool isIncrease ����Ϊ true, ȡ������Ϊ false
    //************************************
    void UpdateSegment(int row, int col, int dir, bool isBlack, bool isIncrease) {
        if(::IsInsideBoard(row, col)) {
            SegmentTable::Item &seg = segmentTable._segment[row][col][dir];
            if(seg.IsInsideBoard()) {
                segmentTable._table[seg._blacks][seg._whites].Remove(seg);
                int &v = (isBlack ? seg._blacks : seg._whites);
                v += (isIncrease ? 1 : -1);
                segmentTable._table[seg._blacks][seg._whites].Add(seg);
            }
        }
    }

    //************************************
    // Method:    UpdateDirection
    // FullName:  ComplexBoard::UpdateDirection
    // Access:    private 
    // Returns:   void
    // Qualifier: ����һ������� Segment
    // Parameter: int row ��
    // Parameter: int col ��
    // Parameter: int dir ���� [0, 4)
    // Parameter: bool isBlack ��ɫΪ true, ��ɫΪ false
    // Parameter: bool isIncrease ����Ϊtrue, ȡ������Ϊ false
    //************************************
    void UpdateDirection(int row, int col, int dir, bool isBlack, bool isIncrease);

public:
    //************************************
    // Method:    MakeMove
    // FullName:  ComplexBoard::MakeMove
    // Access:    public 
    // Returns:   void
    // Qualifier: ����
    // Parameter: int row ��
    // Parameter: int col ��
    // Parameter: bool isBlack ��ɫΪ true, ��ɫΪ false
    //************************************
    void MakeMove(int row, int col, bool isBlack);

    //************************************
    // Method:    MakeMove
    // FullName:  ComplexBoard::MakeMove
    // Access:    public 
    // Returns:   void
    // Qualifier: ����
    // Parameter: const Move & move ������Ϣ
    //************************************
    void MakeMove(const Move &move) {
        MakeMove(move._row, move._col, move._isBlack);
    }

    //************************************
    // Method:    MakeMove
    // FullName:  ComplexBoard::MakeMove
    // Access:    public 
    // Returns:   void
    // Qualifier: ����
    // Parameter: const Point & p ���ӵ�
    // Parameter: bool isBlack ��ɫΪ true, ��ɫΪ false
    //************************************
    void MakeMove(const Point &p, bool isBlack) {
        MakeMove(p._row, p._col, isBlack);
    }

    //************************************
    // Method:    MakeDMove
    // FullName:  ComplexBoard::MakeDMove
    // Access:    public 
    // Returns:   void
    // Qualifier: ��������
    // Parameter: int r1 ��1
    // Parameter: int c1 ��1
    // Parameter: int r2 ��2
    // Parameter: int c2 ��2
    // Parameter: bool isBlack ��ɫΪ true, ��ɫΪ false
    //************************************
    void MakeDMove(int r1, int c1, int r2, int c2, bool isBlack) {
        MakeMove(r1, c1, isBlack);
        MakeMove(r2, c2, isBlack);
    }

    //************************************
    // Method:    MakeDMove
    // FullName:  ComplexBoard::MakeDMove
    // Access:    public 
    // Returns:   void
    // Qualifier: ��������
    // Parameter: const Point & p1 ���ӵ�1
    // Parameter: const Point & p2 ���ӵ�2
    // Parameter: bool isBlack ��ɫΪ true, ��ɫΪ false
    //************************************
    void MakeDMove(const Point &p1, const Point &p2, bool isBlack) {
        MakeDMove(DMove(p1, p2, isBlack));
    }

    //************************************
    // Method:    MakeDMove
    // FullName:  ComplexBoard::MakeDMove
    // Access:    public 
    // Returns:   void
    // Qualifier: ��������
    // Parameter: const DMove & dMove �������ӵ���Ϣ
    //************************************
    void MakeDMove(const DMove &dMove) {
        MakeDMove(dMove._r1, dMove._c1, dMove._r2, dMove._c2, dMove._isBlack);
    }

    //************************************
    // Method:    UnmakeLastMove
    // FullName:  ComplexBoard::UnmakeLastMove
    // Access:    public 
    // Returns:   void
    // Qualifier: ȡ�����һ������
    //************************************
    void UnmakeLastMove();

    //************************************
    // Method:    UnmakeLastDMove
    // FullName:  ComplexBoard::UnmakeLastDMove
    // Access:    public 
    // Returns:   void
    // Qualifier: ȡ�������������
    //************************************
    void UnmakeLastDMove() {
        UnmakeLastMove();
        UnmakeLastMove();
    }

    //************************************
    // Method:    NewSearchInit
    // FullName:  ComplexBoard::NewSearchInit
    // Access:    public 
    // Returns:   void
    // Qualifier: ������ʼǰ��ʼ��
    // Parameter: Board board ���̻�������
    //************************************
    void NewSearchInit(Board board);

    //************************************
    // Method:    PrintData
    // FullName:  ComplexBoard::PrintData
    // Access:    public 
    // Returns:   void
    // Qualifier: ��ӡ������Ϣ
    //************************************
    void PrintData() {
        printf("hash = %08x, checkSum = %016I64x\nBoard\n", _hash, _checkSum);
        ::PrintBoard(_data);
    }

    //************************************
    // Method:    Sontes
    // FullName:  ComplexBoard::Sontes
    // Access:    public 
    // Returns:   int ������������
    // Qualifier: ȡ��������������
    //************************************
    int Sontes() {
        return _stack.Size();
    }
};

extern ComplexBoard complexBoard;
