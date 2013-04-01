#pragma once

/*
 * �ļ������������� MoveSearcher, ������������Ҫ�ĳ�ʼ��, �������̴߳���, ������.
 * ���� SearchGoodMoves() ��ʼ����, Ȼ��� GetDMove() ��ȡ����ŷ�. �������� _TIME_LIMIT
 * �����ڽ���, ���ʱ�䲻������һ��, GetDMove() �����ȡ��һ������ŷ�.
 *
 * �ļ�Ҳ������һЩȫ�ֺ���, ���ڶ� complexBoard, segmentTable ��ȫ�ֱ����ķ���.
 *
 *                                                                              --2011.08.21
 *
 */

#include "Connect6.h"
#include <cassert>
#include "defines.h"
#include "evaluator.h"
#include "utilities.h"
#include "ComplexBoard.h"
#include <set>
#include <queue>
#include <Windows.h>
#include <cstdio>

// ������
class MoveSearcher {
public:
    static /*const*/ int _time_limit/* = 45000*/; // ms

private:
	// ���� SearchGoodMoves() ������������ȡ�ŷ�
    DMove _dMove;

public:
    HANDLE _hMutexDMove;           // ���� _dMove, ʹ�ò��������߳��޸� _dMove ʱ, ɱ�������߳�.
    HANDLE _hThread;               // �����߳̾��
    HANDLE _hEventSearchComplete;  // ��������¼�
    DWORD _threadId;               // �����߳�id

    bool _isBlack;                 // �Ƿ�
    Board _board;                  // ����

    MoveSearcher() {
        TCHAR mutexName[1024];
        tsprintf(mutexName, TEXT("MoveSearcher::_hMutexDMove: %d"), GetCurrentProcessId());
        _hMutexDMove = CreateMutex(NULL, false, mutexName);
        if(_hMutexDMove == NULL) {
            printf("create mutex fail\n");
            exit(1);
        }
        TCHAR eventName[1024];
        tsprintf(eventName, TEXT("MoveSearcher::_hEventSearchComplete: %d"), GetCurrentProcessId());
        _hEventSearchComplete = CreateEvent(NULL, false, false, eventName);
        if(_hEventSearchComplete == NULL) {
            printf("create event fail\n");
            exit(1);
        }
    }

	// ����һ���壨һ�ӻ���ӣ�;
	// board:        ��������
	// isBlacksTurn: true Ϊ�ڷ��غϣ�false Ϊ�׷��غ�
	// moves:        1 ��һ�ӣ�2 �Ŷ���
	void SearchGoodMoves(Board board, bool isBlacksTurn, int moves = 2);

    //************************************
    // Method:    SetDMove
    // FullName:  MoveSearcher::SetDMove
    // Access:    public 
    // Returns:   void
    // Qualifier: ��������ŷ�
    // Parameter: const DMove & dm �ŷ�
    //************************************
    void SetDMove(const DMove &dm) {
        WaitForSingleObject(_hMutexDMove, INFINITY);
        _dMove = dm;
        ReleaseMutex(_hMutexDMove);
    }

    //************************************
    // Method:    GetDMove
    // FullName:  MoveSearcher::GetDMove
    // Access:    public 
    // Returns:   DMove
    // Qualifier: ���� SearchGoodMoves() ȡ������ŷ�
    //************************************
    DMove GetDMove() {
        WaitForSingleObject(_hMutexDMove, INFINITY);
        DMove dm = _dMove;
        ReleaseMutex(_hMutexDMove);
        return dm;
    }
};

extern MoveSearcher searcher;

//************************************
// Method:    GetCell
// FullName:  GetCell
// Access:    public 
// Returns:   CellType ָ�����еĸ�������
// Qualifier: ��� complexBoard ָ�����еĸ�������
// Parameter: int row ��
// Parameter: int col ��
//************************************
inline CellType GetCell(int row, int col) {
    return complexBoard.GetCell(row, col);
}

//************************************
// Method:    GetCell
// FullName:  GetCell
// Access:    public 
// Returns:   CellType ָ����ĸ�������
// Qualifier: ��� complexBoard ָ����ĸ�������
// Parameter: const Point & p ָ���ĵ�
//************************************
inline CellType GetCell(const Point &p) {
    return GetCell(p._row, p._col);
}

//************************************
// Method:    CheckSum
// FullName:  CheckSum
// Access:    public 
// Returns:   unsigned __int64 ���� complexBoard �� 64 λУ���
// Qualifier: ��ȡ complexBoard �� 64 λУ���
//************************************
inline unsigned __int64 CheckSum() {
    return complexBoard._checkSum;
}

//************************************
// Method:    Hash
// FullName:  Hash
// Access:    public 
// Returns:   unsigned int ���� complexBoard �Ĺ�ϣֵ
// Qualifier: ��ȡ complexBoard �Ĺ�ϣֵ
//************************************
inline unsigned int Hash() {
    return complexBoard._hash;
}

//************************************
// Method:    MakeDMove
// FullName:  MakeDMove
// Access:    public 
// Returns:   void
// Qualifier: �� complexBoard ����������
// Parameter: const DMove & dm ������Ϣ
//************************************
inline void MakeDMove(const DMove &dm) {
    complexBoard.MakeDMove(dm);
}

//************************************
// Method:    MakeDMove
// FullName:  MakeDMove
// Access:    public 
// Returns:   void
// Qualifier: �� complexBoard ����������
// Parameter: const Point & p1 ���ӵ�1
// Parameter: const Point & p2 ���ӵ�2
// Parameter: bool isBlack ��ɫΪ true, ��ɫΪ false
//************************************
inline void MakeDMove(const Point &p1, const Point &p2, bool isBlack) {
    complexBoard.MakeDMove(p1, p2, isBlack);
}

//************************************
// Method:    UnmakeLastDMove
// FullName:  UnmakeLastDMove
// Access:    public 
// Returns:   void
// Qualifier: ȡ�� complexBoard �����������
//************************************
inline void UnmakeLastDMove() {
    complexBoard.UnmakeLastDMove();
}

//************************************
// Method:    MakeMove
// FullName:  MakeMove
// Access:    public 
// Returns:   void
// Qualifier: �� complexBoard ��һ����
// Parameter: const Point & p ���ӵ�
// Parameter: bool isBlack ��ɫΪ true, ��ɫΪ false
//************************************
inline void MakeMove(const Point &p, bool isBlack) {
    complexBoard.MakeMove(p, isBlack);
}

//************************************
// Method:    MakeMove
// FullName:  MakeMove
// Access:    public 
// Returns:   void
// Qualifier: �� complexBoard ��һ����
// Parameter: int row ������
// Parameter: int col ������
// Parameter: bool isBlack ��ɫΪ true, ��ɫΪ false
//************************************
inline void MakeMove(int row, int col, bool isBlack) {
    complexBoard.MakeMove(row, col, isBlack);
}

//************************************
// Method:    UnmakeLastMove
// FullName:  UnmakeLastMove
// Access:    public 
// Returns:   void
// Qualifier: ȡ�� complexBoard ���һ������
//************************************
inline void UnmakeLastMove() {
    complexBoard.UnmakeLastMove();
}

//************************************
// Method:    MakeMove
// FullName:  MakeMove
// Access:    public 
// Returns:   void
// Qualifier: �� complexBoard ��һ����
// Parameter: const Move & move ������Ϣ
inline void MakeMove(const Move &move) {
    complexBoard.MakeMove(move);
}

inline void PrintComplexBoard() {
    complexBoard.PrintData();
}

//************************************
// Method:    GetSegmentTable
// FullName:  GetSegmentTable
// Access:    public 
// Returns:   SegmentTable::Table & segmentTable ��ָ���ı������
// Qualifier: ��� segmentTable ��ָ���� "ӵ�� n ��ͬɫ����, 0 ���������ӵ� segment" �����������,
//            segmentTable �� complexBoard ά��, �����޸�����
// Parameter: bool isBlack ��ɫΪ true, ��ɫΪ false
// Parameter: int n segment �а������Ӹ���
//************************************
inline SegmentTable::Table &GetSegmentTable(bool isBlack, int n) {
    return isBlack ? segmentTable._table[n][0] : segmentTable._table[0][n];
}

//************************************
// Method:    ThreatSegs
// FullName:  ThreatSegs
// Access:    public 
// Returns:   int ����в�� segment ���ܸ���
// Qualifier: ����� isBlack ��ָ��һ��������в�� segment (4, 5) �ĸ���. (���� "��һ�ӿ���������в, ����ֵ����1")
// Parameter: bool isBlack
//************************************
inline int ThreatSegs(bool isBlack) {
    return GetSegmentTable(!isBlack, 4).Size() + GetSegmentTable(!isBlack, 5).Size();
}

//************************************
// Method:    Stones
// FullName:  Stones
// Access:    public 
// Returns:   int complexBoard ��������������
// Qualifier: ��� complexBoard ��������������
//************************************
inline int Stones() {
    return complexBoard.Sontes();
}

//************************************
// Method:    RandomDMove
// FullName:  RandomDMove
// Access:    public 
// Returns:   DMove �������
// Qualifier: ��ö� complexBoard �Ϸ��������������, ���ӷ��� isBlacksTurn ָ��
// Parameter: bool isBlacksTurn �ڷ�Ϊ true, �׷�Ϊ false
//************************************
DMove RandomDMove(bool isBlacksTurn);

//************************************
// Method:    CountThreats
// FullName:  CountThreats
// Access:    public 
// Returns:   int ��в��, 0 ��ʾû����в, 1 ��ʾ����һ��ȡ����в, 2 ��ʾ�������ȡ����в����������޷�ȡ����в(��)
// Qualifier: ������в��
// Parameter: bool isBlack �Ժڷ���вΪ true, �԰׷���в Ϊfalse
//************************************
int CountThreats(bool isBlack);
