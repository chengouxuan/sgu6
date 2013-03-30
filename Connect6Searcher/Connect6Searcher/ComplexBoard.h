#pragma once

/*
 * ComplexBoard 存储，维护各种关于棋盘的信息:
 * 1. 基本棋盘 _data;
 * 2. 哈希值 _hash;
 * 3. 校验和 _checkSum;
 * 4. 棋子堆栈 _stack;
 *
 * ComplexBoard 同时维护 SegmentTable, SegmentTable 本应属于棋盘, 处于避免
 * 代码嵌套层数太多的考虑, ComplexBoard 被写在 ComplexBoard 之外, 并单独
 * 处在另一个文件 "SegmentTable.h" 之中.
 *
 * ComplexBoard 存在一个全局实例 complexBoard.
 *
 * 文件同时定义了棋子堆栈类 MoveStack, 从 MoveArray 派生.
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
    unsigned int _hash;               // 棋盘哈希值 (zobrist 方法)
    unsigned __int64 _checkSum;       // 棋盘校验和 (zobrist 方法)
    CellType _data[ROW_MAX][COL_MAX]; // 棋盘基本数组
    MoveStack _stack;                 // 棋子堆栈

    //************************************
    // Method:    GetCell
    // FullName:  ComplexBoard::GetCell
    // Access:    public 
    // Returns:   CellType 格子的类型
    // Qualifier: 取得棋盘某一行列的格子的类型
    // Parameter: int row 行
    // Parameter: int col 列
    //************************************
    CellType GetCell(int row, int col) {
        assert(::IsInsideBoard(row, col));
        return _data[row][col];
    }

    //************************************
    // Method:    GetCell
    // FullName:  ComplexBoard::GetCell
    // Access:    public 
    // Returns:   CellType 格子的类型
    // Qualifier: 取得棋盘某一行列的格子的类型
    // Parameter: const Point & p 指定行列值的点
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
    // Qualifier: 落子 (MakeMove) 或取消落子 (UnmakeMove) 的时候需要更新棋盘附加信息 SegmentTable,
    //            Segment 为 SegmentTable 中一个元素, 此函数更新一个元素
    // Parameter: int row 行
    // Parameter: int col 列
    // Parameter: int dir 方向
    // Parameter: bool isBlack 黑色为 true, 白色为 false
    // Parameter: bool isIncrease 落子为 true, 取消落子为 false
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
    // Qualifier: 更新一个方向的 Segment
    // Parameter: int row 行
    // Parameter: int col 列
    // Parameter: int dir 方向 [0, 4)
    // Parameter: bool isBlack 黑色为 true, 白色为 false
    // Parameter: bool isIncrease 落子为true, 取消落子为 false
    //************************************
    void UpdateDirection(int row, int col, int dir, bool isBlack, bool isIncrease);

public:
    //************************************
    // Method:    MakeMove
    // FullName:  ComplexBoard::MakeMove
    // Access:    public 
    // Returns:   void
    // Qualifier: 落子
    // Parameter: int row 行
    // Parameter: int col 列
    // Parameter: bool isBlack 黑色为 true, 白色为 false
    //************************************
    void MakeMove(int row, int col, bool isBlack);

    //************************************
    // Method:    MakeMove
    // FullName:  ComplexBoard::MakeMove
    // Access:    public 
    // Returns:   void
    // Qualifier: 落子
    // Parameter: const Move & move 落子信息
    //************************************
    void MakeMove(const Move &move) {
        MakeMove(move._row, move._col, move._isBlack);
    }

    //************************************
    // Method:    MakeMove
    // FullName:  ComplexBoard::MakeMove
    // Access:    public 
    // Returns:   void
    // Qualifier: 落子
    // Parameter: const Point & p 落子点
    // Parameter: bool isBlack 黑色为 true, 白色为 false
    //************************************
    void MakeMove(const Point &p, bool isBlack) {
        MakeMove(p._row, p._col, isBlack);
    }

    //************************************
    // Method:    MakeDMove
    // FullName:  ComplexBoard::MakeDMove
    // Access:    public 
    // Returns:   void
    // Qualifier: 落两个子
    // Parameter: int r1 行1
    // Parameter: int c1 列1
    // Parameter: int r2 行2
    // Parameter: int c2 列2
    // Parameter: bool isBlack 黑色为 true, 白色为 false
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
    // Qualifier: 落两个子
    // Parameter: const Point & p1 落子点1
    // Parameter: const Point & p2 落子点2
    // Parameter: bool isBlack 黑色为 true, 白色为 false
    //************************************
    void MakeDMove(const Point &p1, const Point &p2, bool isBlack) {
        MakeDMove(DMove(p1, p2, isBlack));
    }

    //************************************
    // Method:    MakeDMove
    // FullName:  ComplexBoard::MakeDMove
    // Access:    public 
    // Returns:   void
    // Qualifier: 落两个子
    // Parameter: const DMove & dMove 落两个子的信息
    //************************************
    void MakeDMove(const DMove &dMove) {
        MakeDMove(dMove._r1, dMove._c1, dMove._r2, dMove._c2, dMove._isBlack);
    }

    //************************************
    // Method:    UnmakeLastMove
    // FullName:  ComplexBoard::UnmakeLastMove
    // Access:    public 
    // Returns:   void
    // Qualifier: 取消最后一个落子
    //************************************
    void UnmakeLastMove();

    //************************************
    // Method:    UnmakeLastDMove
    // FullName:  ComplexBoard::UnmakeLastDMove
    // Access:    public 
    // Returns:   void
    // Qualifier: 取消最后两个落子
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
    // Qualifier: 搜索开始前初始化
    // Parameter: Board board 棋盘基本数组
    //************************************
    void NewSearchInit(Board board);

    //************************************
    // Method:    PrintData
    // FullName:  ComplexBoard::PrintData
    // Access:    public 
    // Returns:   void
    // Qualifier: 打印棋盘信息
    //************************************
    void PrintData() {
        printf("hash = %08x, checkSum = %016I64x\nBoard\n", _hash, _checkSum);
        ::PrintBoard(_data);
    }

    //************************************
    // Method:    Sontes
    // FullName:  ComplexBoard::Sontes
    // Access:    public 
    // Returns:   int 棋盘中棋子数
    // Qualifier: 取得棋盘中棋子数
    //************************************
    int Sontes() {
        return _stack.Size();
    }
};

extern ComplexBoard complexBoard;
