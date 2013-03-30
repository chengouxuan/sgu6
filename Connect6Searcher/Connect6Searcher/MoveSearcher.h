#pragma once

/*
 * 文件定义了搜索类 MoveSearcher, 负责搜索所需要的初始化, 搜索的线程创建, 撤销等.
 * 调用 SearchGoodMoves() 开始搜索, 然后从 GetDMove() 获取最佳着法. 搜索会在 _TIME_LIMIT
 * 毫秒内结束, 如果时间不够搜索一层, GetDMove() 将会获取到一个随机着法.
 *
 * 文件也定义了一些全局函数, 用于对 complexBoard, segmentTable 等全局变量的访问.
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

// 搜索类
class MoveSearcher {
public:
    static /*const*/ int _time_limit/* = 45000*/; // ms

private:
	// 调用 SearchGoodMoves() 后从这个变量获取着法
    DMove _dMove;

public:
    HANDLE _hMutexDMove;           // 保护 _dMove, 使得不在搜索线程修改 _dMove 时, 杀死搜索线程.
    HANDLE _hThread;               // 搜索线程句柄
    HANDLE _hEventSearchComplete;  // 搜索完成事件
    DWORD _threadId;               // 搜索线程id

    bool _isBlack;                 // 是否
    Board _board;                  // 棋盘

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

	// 搜索一步棋（一子或二子）;
	// board:        棋盘数组
	// isBlacksTurn: true 为黑方回合，false 为白方回合
	// moves:        1 放一子，2 放二子
	void SearchGoodMoves(Board board, bool isBlacksTurn, int moves = 2);

    //************************************
    // Method:    SetDMove
    // FullName:  MoveSearcher::SetDMove
    // Access:    public 
    // Returns:   void
    // Qualifier: 设置最佳着法
    // Parameter: const DMove & dm 着法
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
    // Qualifier: 调用 SearchGoodMoves() 取得最佳着法
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
// Returns:   CellType 指定行列的格子类型
// Qualifier: 获得 complexBoard 指定行列的格子类型
// Parameter: int row 行
// Parameter: int col 列
//************************************
inline CellType GetCell(int row, int col) {
    return complexBoard.GetCell(row, col);
}

//************************************
// Method:    GetCell
// FullName:  GetCell
// Access:    public 
// Returns:   CellType 指定点的格子类型
// Qualifier: 获得 complexBoard 指定点的格子类型
// Parameter: const Point & p 指定的点
//************************************
inline CellType GetCell(const Point &p) {
    return GetCell(p._row, p._col);
}

//************************************
// Method:    CheckSum
// FullName:  CheckSum
// Access:    public 
// Returns:   unsigned __int64 棋盘 complexBoard 的 64 位校验和
// Qualifier: 获取 complexBoard 的 64 位校验和
//************************************
inline unsigned __int64 CheckSum() {
    return complexBoard._checkSum;
}

//************************************
// Method:    Hash
// FullName:  Hash
// Access:    public 
// Returns:   unsigned int 棋盘 complexBoard 的哈希值
// Qualifier: 获取 complexBoard 的哈希值
//************************************
inline unsigned int Hash() {
    return complexBoard._hash;
}

//************************************
// Method:    MakeDMove
// FullName:  MakeDMove
// Access:    public 
// Returns:   void
// Qualifier: 对 complexBoard 下两个棋子
// Parameter: const DMove & dm 落子信息
//************************************
inline void MakeDMove(const DMove &dm) {
    complexBoard.MakeDMove(dm);
}

//************************************
// Method:    MakeDMove
// FullName:  MakeDMove
// Access:    public 
// Returns:   void
// Qualifier: 对 complexBoard 下两个棋子
// Parameter: const Point & p1 落子点1
// Parameter: const Point & p2 落子点2
// Parameter: bool isBlack 黑色为 true, 白色为 false
//************************************
inline void MakeDMove(const Point &p1, const Point &p2, bool isBlack) {
    complexBoard.MakeDMove(p1, p2, isBlack);
}

//************************************
// Method:    UnmakeLastDMove
// FullName:  UnmakeLastDMove
// Access:    public 
// Returns:   void
// Qualifier: 取消 complexBoard 最后两个落子
//************************************
inline void UnmakeLastDMove() {
    complexBoard.UnmakeLastDMove();
}

//************************************
// Method:    MakeMove
// FullName:  MakeMove
// Access:    public 
// Returns:   void
// Qualifier: 对 complexBoard 下一个子
// Parameter: const Point & p 落子点
// Parameter: bool isBlack 黑色为 true, 白色为 false
//************************************
inline void MakeMove(const Point &p, bool isBlack) {
    complexBoard.MakeMove(p, isBlack);
}

//************************************
// Method:    MakeMove
// FullName:  MakeMove
// Access:    public 
// Returns:   void
// Qualifier: 对 complexBoard 下一个子
// Parameter: int row 落子行
// Parameter: int col 落子列
// Parameter: bool isBlack 黑色为 true, 白色为 false
//************************************
inline void MakeMove(int row, int col, bool isBlack) {
    complexBoard.MakeMove(row, col, isBlack);
}

//************************************
// Method:    UnmakeLastMove
// FullName:  UnmakeLastMove
// Access:    public 
// Returns:   void
// Qualifier: 取消 complexBoard 最后一个落子
//************************************
inline void UnmakeLastMove() {
    complexBoard.UnmakeLastMove();
}

//************************************
// Method:    MakeMove
// FullName:  MakeMove
// Access:    public 
// Returns:   void
// Qualifier: 对 complexBoard 下一个子
// Parameter: const Move & move 落子信息
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
// Returns:   SegmentTable::Table & segmentTable 中指定的表的引用
// Qualifier: 获得 segmentTable 中指定的 "拥有 n 个同色棋子, 0 个对手棋子的 segment" 整个表的引用,
//            segmentTable 由 complexBoard 维护, 不可修改引用
// Parameter: bool isBlack 黑色为 true, 白色为 false
// Parameter: int n segment 中包含棋子个数
//************************************
inline SegmentTable::Table &GetSegmentTable(bool isBlack, int n) {
    return isBlack ? segmentTable._table[n][0] : segmentTable._table[0][n];
}

//************************************
// Method:    ThreatSegs
// FullName:  ThreatSegs
// Access:    public 
// Returns:   int 有威胁的 segment 的总个数
// Qualifier: 计算对 isBlack 所指定一方存在威胁的 segment (4, 5) 的个数. (并非 "落一子可以消除威胁, 返回值等于1")
// Parameter: bool isBlack
//************************************
inline int ThreatSegs(bool isBlack) {
    return GetSegmentTable(!isBlack, 4).Size() + GetSegmentTable(!isBlack, 5).Size();
}

//************************************
// Method:    Stones
// FullName:  Stones
// Access:    public 
// Returns:   int complexBoard 棋盘中棋子总数
// Qualifier: 获得 complexBoard 棋盘中棋子总数
//************************************
inline int Stones() {
    return complexBoard.Sontes();
}

//************************************
// Method:    RandomDMove
// FullName:  RandomDMove
// Access:    public 
// Returns:   DMove 随机落子
// Qualifier: 获得对 complexBoard 合法的两个随机落子, 落子方由 isBlacksTurn 指定
// Parameter: bool isBlacksTurn 黑方为 true, 白方为 false
//************************************
DMove RandomDMove(bool isBlacksTurn);

//************************************
// Method:    CountThreats
// FullName:  CountThreats
// Access:    public 
// Returns:   int 威胁数, 0 表示没有威胁, 1 表示需落一子取消威胁, 2 表示需落二子取消威胁或落二子仍无法取消威胁(输)
// Qualifier: 计算威胁数
// Parameter: bool isBlack 对黑方威胁为 true, 对白方威胁 为false
//************************************
int CountThreats(bool isBlack);
