#include "RZDeferder.h"
#include "Zone.h"
#include "DTSSer.h"

bool RZDefender::Defende(bool isBlack) {
    static MGDMoveArray arr;
    arr.Clear();
    static Zone zone;
    if(! (dtsser.RelavanceZone(!isBlack, zone) && zone.Size() > 0)) {
        return false;
    }
    static MGMoveArray sing; // single move
    sing.Clear();
    FOR_EACH(i, zone.Size()) {
        Point p = zone.GetPoint(i);
        if(::GetCell(p) == CELL_TYPE_EMPTY) {
            sing.PushBack(p, isBlack, MoveEvaluate(p, isBlack));
        }
    }
    //SortMGMoveByScore(sing);
    FOR_EACH(i, sing.Size()) {
        ::MakeMove(sing.ItemRef(i));
        if(! (dtsser.RelavanceZone(!isBlack, zone) && zone.Size() > 0)) {
            // a single move is able to defend, in this case, enumerate all second moves
            // forming some double moves.
            static MGDMoveArray t;
            EnumSecondMove(sing.ItemRef(i), t);
            FOR_EACH(k, t.Size()) {
                if(! arr.Find(t.ItemRef(k)), 0) {
                    arr.PushBack(t.ItemRef(k), 0);
                }
            }
        }
        // try all second moves
        static MGMoveArray sec; // second move
        sec.Clear();
        FOR_EACH(k, zone.Size()) {
            Point p = zone.GetPoint(k);
            if(::GetCell(p) == CELL_TYPE_EMPTY) {
                sec.PushBack(p, isBlack, 0);
            }
        }
        //SortMGMoveByScore(sec);
        FOR_EACH(k, sec.Size()) {
            ::MakeMove(sec.ItemRef(k));
            DMove m(sing.ItemRef(i).GetPoint(), sec.ItemRef(k).GetPoint(), isBlack);
            if(!arr.Find(m, 0) && !dtsser.Search(!isBlack, false)) {
                arr.PushBack(m, 0);
            }
            ::UnmakeLastMove();
        }
        ::UnmakeLastMove();
    }
    if(arr.Size() <= 0) {
        return false;
    }
    FOR_EACH(i, arr.Size()) {
        arr.ItemRef(i)._score = DMoveEvaluate(arr.ItemRef(i));
    }
    _dMove = arr.MaxScoreRef();
    return true;
}

void RZDefender::EnumSecondMove(const Move &move, MGDMoveArray &arr) {
    arr.Clear();
    FOR_EACH_CELL(r, c) {
        if(::GetCell(r, c) == CELL_TYPE_EMPTY && Point(r, c) != move.GetPoint()) {
            DMove dm(move.GetPoint(), Point(r, c), move._isBlack);
            arr.PushBack(dm, DMoveEvaluate(dm));
        }
    }
}

void RZDefender::SortMGMoveByScore(MGMoveArray &arr) {
    // bubble sort
    FOR_EACH(i, arr.Size()) {
        FOR_EACH(k, arr.Size() - i - 1) {
            if(arr.ItemRef(k)._score < arr.ItemRef(k + 1)._score) {
                std::swap(arr.ItemRef(k), arr.ItemRef(k + 1));
            }
        }
    }
    assert(arr.Size() > 0);
}

RZDefender rzdefender;