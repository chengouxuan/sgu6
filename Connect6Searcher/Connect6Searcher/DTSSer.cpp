#include "DTSSer.h"
#include "Zone.h"
#include "connect6.h"
#include "DPointTable.h"

#undef _DEBUG

bool DTSSer::GenerateBlocks(DMoveArray &moveArray) {
    ::MakeDMove(moveArray.ItemRef(0));
    SegmentTable::Table &four = ::GetSegmentTable(_isBlack, 4);
    SegmentTable::Table &five = ::GetSegmentTable(_isBlack, 5);
    if(four.Size() + five.Size() < 2 || ::ThreatSegs(_isBlack) > 0) {
        ::UnmakeLastDMove();
        return false;
    }
    SegmentTable::Table &tab1 = (four.Size() == 0 ? five : four);
    SegmentTable::Item seg1 = tab1.GetItem(0);
    FOR_EACH(i, 6) {
        Point p1 = seg1.GetPoint(i);
        if(::GetCell(p1) != CELL_TYPE_EMPTY) {
            continue;
        }
        ::MakeMove(p1, !_isBlack);
        if(four.Size() + five.Size() == 0) {
            ::UnmakeLastMove();
            ::UnmakeLastDMove();
            return false;
        }
        SegmentTable::Table &tab2 = (four.Size() == 0 ? five : four);
        SegmentTable::Item seg2 = tab2.GetItem(0);
        FOR_EACH(k, 6) {
            Point p2 = seg2.GetPoint(k);
            if(::GetCell(p2) != CELL_TYPE_EMPTY) {
                continue;
            }
            ::MakeMove(p2, !_isBlack);
            if(four.Size() + five.Size() == 0 && !moveArray.Find(p1, p2, !_isBlack)) {
                moveArray.PushBack(p1, p2, !_isBlack);
            }
            ::UnmakeLastMove();
        }
        ::UnmakeLastMove();
    }
    ::UnmakeLastDMove();
    return true;
}

bool DTSSer::DTSS(int depth, bool isIterativeDTSS) {
#ifdef _DEBUG
    //printf("depth = %d\n", depth);
    //::PrintComplexBoard();
#endif
    bool b;
    DMove dm;
    if(GetTransTable(_isBlack).LookUp(b, dm)) {
        ++GetTransTable(_isBlack)._hits;
        return b;
    } else if(depth <= 0 || SuccessRateIsLow(_isBlack)) {
        ++_dropedSearches;
        return false;
    }
    ++_nodes;

    static DPointTable dPoint[_DTSS_DEPTH_MAX + 1];
    dPoint[depth].Clear();

    static Zone zone[_DTSS_DEPTH_MAX + 1];
    zone[depth].Clear();

    if(ThreatSegs(_isBlack) <= 0) {
        AddEmptyCells(zone[depth], ::GetSegmentTable(_isBlack, 3));
        AddEmptyCells(zone[depth], ::GetSegmentTable(_isBlack, 2));
    } else {
        AddEmptyCells(zone[depth], ::GetSegmentTable(!_isBlack, 4));
        AddEmptyCells(zone[depth], ::GetSegmentTable(!_isBlack, 5));
    }

    static DMoveArray doubleThreat[_DTSS_DEPTH_MAX + 1];
    doubleThreat[depth].Clear();

    // conservative DTSS
    FOR_EACH(i, zone[depth].Size()) {
        if(::GetCell(zone[depth].GetPoint(i)) != CELL_TYPE_EMPTY) {
            continue;
        }

        ::MakeMove(zone[depth].GetPoint(i), _isBlack);

        static Zone z[_DTSS_DEPTH_MAX + 1];
        z[depth].Clear();
        AddEmptyCells(z[depth], ::GetSegmentTable(_isBlack, 3));

        ::UnmakeLastMove();

        FOR_EACH(k, z[depth].Size()) {
            if(::GetCell(z[depth].GetPoint(k)) != CELL_TYPE_EMPTY) {
                continue;
            }
            if(dPoint[depth].IsInTable(zone[depth].GetPoint(i), z[depth].GetPoint(k))) {
                continue;
            }
            dPoint[depth].Add(zone[depth].GetPoint(i), z[depth].GetPoint(k));

            static DMoveArray dMoveArray[_DTSS_DEPTH_MAX + 1];
            dMoveArray[depth].Clear();
            dMoveArray[depth].PushBack(zone[depth].GetPoint(i), z[depth].GetPoint(k), _isBlack);

            if(GenerateBlocks(dMoveArray[depth])) {
                int n = MakeNMoves(dMoveArray[depth]);
                bool isSuccess = (dMoveArray[depth].Size() == 1) || DTSS(depth - 1, false);
                UnmakeNMoves(n);
                if(isSuccess) {
                    GetTransTable(_isBlack).Enter(true, dMoveArray[depth].ItemRef(0));
                    return true;
                }
                doubleThreat[depth].PushBack(dMoveArray[depth].ItemRef(0));
            }
        }
    }
    if(!isIterativeDTSS || std::abs(depth - _dtss_depth) >= _id_dtss_depth) {
        GetTransTable(_isBlack).Enter(false);
        return false;
    }
    // iterative DTSS
    FOR_EACH(i, doubleThreat[depth].Size()) {
        static DMoveArray dMove[_DTSS_DEPTH_MAX + 1];
        dMove[depth].Clear();
        dMove[depth].PushBack(doubleThreat[depth].ItemRef(i));
        GenerateBlocks(dMove[depth]);

        ::MakeDMove(dMove[depth].ItemRef(0));
        bool isSuccess = true;
        for(int k=1; k<dMove[depth].Size(); ++k) {
            ::MakeDMove(dMove[depth].ItemRef(k));
            isSuccess = isSuccess && DTSS(depth - 1, true);
            ::UnmakeLastDMove();
            if(! isSuccess) {
                break;
            }
        }
        ::UnmakeLastDMove();
        if(isSuccess) {
            GetTransTable(_isBlack).Enter(true, dMove[depth].ItemRef(0));
            if(depth == _dtss_depth) {
                ++_idtssSuccesses;
            }
            return true;
        }
    }
    GetTransTable(_isBlack).Enter(false);
    return false;
}

bool DTSSer::RelavanceZone(bool isBlackOffensive, Zone &zone) {
    _isIterativeDTSS = false;
    _isBlack = isBlackOffensive;
    zone.Clear();
    if(! DTSS(_dtss_depth, false)) {
        return false;
    } else {
        int cnt = 0;
        while(true) {
            bool b = DTSS(_dtss_depth, false);
            assert(b);
            GetTransTable(isBlackOffensive).LookUp(b, _dMove);
            assert(b);
            ::MakeDMove(_dMove);
            zone.AddSegmentTable(::GetSegmentTable(isBlackOffensive, 4));
            zone.AddSegmentTable(::GetSegmentTable(isBlackOffensive, 5));
            ::UnmakeLastDMove();

            static DMoveArray dMoveArray;
            dMoveArray.Clear();
            dMoveArray.PushBack(_dMove);
            GenerateBlocks(dMoveArray);
            if(dMoveArray.Size() == 1) {
                break;
            }
            cnt += MakeNMoves(dMoveArray);
            zone.AddSegmentTable(::GetSegmentTable(!isBlackOffensive, 2));
            zone.AddSegmentTable(::GetSegmentTable(!isBlackOffensive, 3));
        }
        UnmakeNMoves(cnt);
        return true;
    }
}

int DTSSer::MakeNMoves(DMoveArray &dMoveArray) {
    int cnt = 0;
    FOR_EACH(i, dMoveArray.Size()) {
        DMove dm = dMoveArray.ItemRef(i);
        if(::GetCell(dm.GetPoint1()) == CELL_TYPE_EMPTY) {
            ++cnt;
            ::MakeMove(dm.GetPoint1(), dm._isBlack);
        }
        if(::GetCell(dm.GetPoint2()) == CELL_TYPE_EMPTY) {
            ++cnt;
            ::MakeMove(dm.GetPoint2(), dm._isBlack);
        }
    }
    return cnt;
}

void DTSSer::UnmakeNMoves(int n) {
    while(n-- > 0) {
        ::UnmakeLastMove();
    }
}

void DTSSer::AddEmptyCells(Zone &zone, SegmentTable::Table &tab) {
    FOR_EACH(i, tab.Size()) {
        FOR_EACH(k, 6) {
            if(tab.GetItem(i).GetCell(k) == CELL_TYPE_EMPTY && !zone.IsInZone(tab.GetItem(i).GetPoint(k))) {
                zone.AddPoint(tab.GetItem(i).GetPoint(k));
            }
        }
    }
}

int DTSSer::_dtss_depth;
int DTSSer::_id_dtss_depth;

DTSSer dtsser;
