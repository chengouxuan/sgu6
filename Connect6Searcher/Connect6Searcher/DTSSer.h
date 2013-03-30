#pragma once

/*
 * 这个文件定义了双威胁搜索器, Double Threat Space SearchER.
 * 
 * DTSSer 操作全局棋盘 complexBoard, 和棋盘附加信息 segmentTable.
 *
 * DTSSer通过全局函数等调用全局实例 complexBoard 和segmentTable,
 * 且 DTSSer 自己也有一个全局实例 dtsser.
 *
 *                                                            --2011.08.21
 *
 */

#include "Connect6.h"
#include <vector>
#include "ComplexBoard.h"
#include "SegmentTable.h"
#include "movesearcher.h"
#include "Zone.h"
#include <set>
#include "TranspositionTable.h"
#include "Array.h"

class DMoveArray: public Array <DMove, ROW_MAX * COL_MAX * ROW_MAX * COL_MAX, DMoveEqual> {
public:
    void PushBack(const DMove &dm) {
        __super::PushBack(dm);
    }
    void PushBack(int r1, int c1, int r2, int c2, bool isBlack) {
        __super::PushBack(DMove(r1, c1, r2, c2, isBlack));
    }
    void PushBack(const Point &p1, const Point &p2, bool isBlack) {
        __super::PushBack(DMove(p1, p2, isBlack));
    }
    bool Find(const DMove &dm) {
        return __super::Find(dm);
    }
    bool Find(int r1, int c1, int r2, int c2, bool isBlack) {
        return __super::Find(DMove(r1, c1, r2, c2, isBlack));
    }
    bool Find(const Point &p1, const Point &p2, bool isBlack) {
        return __super::Find(DMove(p1, p2, isBlack));
    }
};

class DTSSer {
public:
    static const int _DTSS_DEPTH_MAX = 15;
    static /*const*/ int _dtss_depth/* = 5*/;           // 搜索深度
    static /*const*/ int _id_dtss_depth/* = 2*/; // 使用"迭代"双威胁搜索的层数

    static DTSSer *sharedInstance();

    bool _isBlack;                              // is black the offensive side, 黑方进攻为 true, 白方进攻为 false
    DMove _dMove;                               // 搜索得的着法
    bool _isIterativeDTSS;                      // 是否使用"迭代"双威胁搜索

    int _nodes;                                 // 结点统计
    int _dropedSearches;                        // 因深度限制导致搜索失败的统计
    int _idtssSuccesses;                        // "迭代"双威胁搜索成功统计
    
    ComplexBoard *_complexBoard;

    //************************************
    // Method:    ResetCounters
    // FullName:  DTSSer::ResetCounters
    // Access:    public 
    // Returns:   void
    // Qualifier: 统计清零
    //************************************
    void ResetCounters() {
        _nodes = 0;
        _dropedSearches = 0;
        _idtssSuccesses = 0;
        _transTableBlack._hits = 0;
        _transTableWhite._hits = 0;
    }

    DTSSer() {}

    //************************************
    // Method:    Search
    // FullName:  DTSSer::Search
    // Access:    public 
    // Returns:   bool
    // Qualifier: 双威胁搜索, 外部调用此函数进行搜索
    // Parameter: bool isBlackOffensive 黑方进攻为 true, 白方进攻为 false
    // Parameter: bool isIDTSS 使用"迭代"双威胁搜索为 true, 否则为 false,
    //            默认 true, 使用"迭代"方法会增加节点数和成功率
    //************************************
    bool Search(bool isBlackOffensive, bool isIDTSS = true) {
        bool b = false;
        _isIterativeDTSS = isIDTSS;
        _isBlack = isBlackOffensive;
        if(! DTSS(_dtss_depth, isIDTSS)) {
            return false;
        } else {
            b = GetTransTable(isBlackOffensive).LookUp(b, _dMove) && b;
            assert(b);
            return true;
        }
    }

    bool RelavanceZone(bool isBlackOffensive, Zone &zone);

    class Item { // 置换表项
    public:
        enum Type {
            TYPE_NOT_A_ITEM,
            TYPE_SUCCESS,
            TYPE_FAIL
        } _type;
        unsigned __int64 _checkSum;
        DMove _dMove;

        Item()
            : _type(TYPE_NOT_A_ITEM) 
            , _checkSum(0)
            , _dMove(DMove()) {}
        Item(Type type, unsigned __int64 checkSum, const DMove &dm)
            : _type(type)
            , _checkSum(checkSum)
            , _dMove(dm) {}
    };

    class TransTable: public TranspositionTable <Item, 1048583> { // 置换表
    public:
        void Enter(bool isDTSSSuccess, const DMove &dm = DMove()) {
            __super::Enter(Item(isDTSSSuccess ? Item::TYPE_SUCCESS : Item::TYPE_FAIL, ::CheckSum(), dm), ::Hash());
        }
        bool LookUp(bool &isDTSSSuccess, DMove &dm) {
            Item item;
            if(!__super::LookUp(::CheckSum(), ::Hash(), item) || item._type == Item::TYPE_NOT_A_ITEM) {
                return false;
            }
            dm = item._dMove;
            isDTSSSuccess = (item._type == Item::TYPE_SUCCESS);
            return true;
        }
    } _transTableBlack, _transTableWhite;

    TransTable &GetTransTable(bool isBlack) {
        return isBlack ? _transTableBlack : _transTableWhite;
    }

private:
    bool DTSS(int depth, bool isIterativeDTSS);

    // returns false if moveArray. GetItem(0) is not Double Threat move or there exists counter threats,
    // otherwise returns true and the moveArray contains the threat move and all possible blocking moves.
    bool GenerateBlocks(DMoveArray &moveArray);

    void AddEmptyCells(Zone &zone, SegmentTable::Table &tab);

    int MakeNMoves(DMoveArray &dMoveArray);

    void UnmakeNMoves(int n);

    bool SuccessRateIsLow(bool isBlackOffensive) {
        return false;
        return ::GetSegmentTable(isBlackOffensive, 2).Size() * 11 +
            ::GetSegmentTable(isBlackOffensive, 3).Size() * 19 -
            ::GetSegmentTable(!isBlackOffensive, 2).Size() * 2 -
            ::GetSegmentTable(!isBlackOffensive, 3).Size() * 3 < 13;
    }
};

extern DTSSer dtsser;
