#include "MoveGenerator.h"
#include "MoveSearcher.h"
//#include "MoveArray.h"
#include "Zone.h"
#include "DTSSer.h"
#include "utilities.h"
#include "DPointTable.h"

MGDMoveArray &MoveGenerator::Generate(bool isBlack, int no) {
    assert(0 <= no && no < _NO_MAX);
    int th = CountThreats(isBlack);
    if(th == 1) {
        GenerateSingleBlocks(isBlack, _arr[no]);
        _type = MOVE_TYPE_SINGLE_BLOCK;
        return _arr[no];
    } else if(th >= 2) {
        GenerateDoubleBlocks(isBlack, _arr[no]);
        _type = MOVE_TYPE_DOUBLE_BLOCK;
        return _arr[no];
    } else if(IsOpening(isBlack)) {
        GenerateOpenings(isBlack, _arr[no]);
    } else {
        GeneratePromisings(isBlack, _arr[no]);
        _type = MOVE_TYPE_PROMISING;
    }
    return _arr[no];
}

void MoveGenerator::GenerateDoubleBlocks(bool isBlack, MGDMoveArray &arr) {
    arr.Clear();
    static Zone zone;
    zone.Clear();
    zone.AddSegmentTable(::GetSegmentTable(!isBlack, 4));
    zone.AddSegmentTable(::GetSegmentTable(!isBlack, 5));
    FOR_EACH(i, zone.Size()) {
        if(::GetCell(zone.GetPoint(i)) != CELL_TYPE_EMPTY) {
            continue;
        }
        ::MakeMove(zone.GetPoint(i), isBlack);
        static Zone z;
        z.Clear();
        z.AddSegmentTable(::GetSegmentTable(!isBlack, 4));
        z.AddSegmentTable(::GetSegmentTable(!isBlack, 5));
        FOR_EACH(k, z.Size()) {
            if(::GetCell(z.GetPoint(k)) != CELL_TYPE_EMPTY || zone.GetPoint(i) == z.GetPoint(k)) {
                continue;
            } else if(arr.Find(zone.GetPoint(i), z.GetPoint(k), isBlack, 0)) {
                continue;
            }
            ::MakeMove(z.GetPoint(k), isBlack);
            if(ThreatSegs(isBlack) <= 0) {
                arr.PushBack(zone.GetPoint(i), z.GetPoint(k), isBlack, 0);
            }
            ::UnmakeLastMove();
        }
        ::UnmakeLastMove();
    }
    if(arr.Size() == 0) {
        FOR_EACH(i, zone.Size()) {
            FOR_EACH(k, i) {
                if(::GetCell(zone.GetPoint(i)) == CELL_TYPE_EMPTY && ::GetCell(zone.GetPoint(k)) == CELL_TYPE_EMPTY) {
                    arr.PushBack(zone.GetPoint(i), zone.GetPoint(k), isBlack, 0);
                    return;
                }
            }
        }
    }
    if(arr.Size() == 0) {
        arr.PushBack(RandomDMove(isBlack), 0);
    }
}

void MoveGenerator::GeneratePromisings(bool isBlack, MGDMoveArray &arr) {
    //static MGMoveArray prom;
    //static Zone zone;
    static DPointTable tab;
    tab.Clear();
    arr.Clear();
    static MGDMoveArray thr;
    GenerateThreats(isBlack, thr);
    FOR_EACH(i, thr.Size()) {
        arr.PushBack(thr.ItemRef(i));
        tab.Add(thr.ItemRef(i).GetPoint1(), thr.ItemRef(i).GetPoint2());
    }
    static MGDMoveArray block;
    GeneratePreBlock(isBlack, block);
    FOR_EACH(i, block.Size()) {
        if(! tab.IsInTable(block.ItemRef(i).GetPoint1(), block.ItemRef(i).GetPoint2())) {
            arr.PushBack(block.ItemRef(i));
            tab.Add(block.ItemRef(i).GetPoint1(), block.ItemRef(i).GetPoint2());
        }
    }
    static MGDMoveArray con;
    GeneratePreConnect(isBlack, con);
    FOR_EACH(i, con.Size()) {
        if(! tab.IsInTable(con.ItemRef(i).GetPoint1(), con.ItemRef(i).GetPoint2())) {
            arr.PushBack(con.ItemRef(i));
            tab.Add(con.ItemRef(i).GetPoint1(), con.ItemRef(i).GetPoint2());
        }
    }
    static MGDMoveArray ex;
    GenerateExtremThreats(isBlack, ex);
    FOR_EACH(i, ex.Size()) {
        if(! tab.IsInTable(ex.ItemRef(i).GetPoint1(), ex.ItemRef(i).GetPoint2())) {
            arr.PushBack(ex.ItemRef(i));
            tab.Add(ex.ItemRef(i).GetPoint1(), ex.ItemRef(i).GetPoint2());
        }
    }
    //static MGDMoveArray prom;
    if(arr.Size() > 0) {
        return;
    } else {
        GenerateByEvals(isBlack, arr);
    }
}

void MoveGenerator::SingleBlocks(bool isBlack, MGMoveArray &block) {
    block.Clear();
    static Counter cnt;
    cnt.Zero();
    SegmentTable::Table &four = ::GetSegmentTable(!isBlack, 4);
    SegmentTable::Table &five = ::GetSegmentTable(!isBlack, 5);
    FOR_EACH(i, four.Size()) {
        FOR_EACH(k, 6) {
            Point p = four.GetItem(i).GetPoint(k);
            if(::GetCell(p) == CELL_TYPE_EMPTY && ++cnt.Cnt(p) >= four.Size() + five.Size() && !block.Find(p, isBlack, 0)) {
                block.PushBack(p, isBlack, 0);
            }
        }
    }
    FOR_EACH(i, five.Size()) {
        FOR_EACH(k, 6) {
            Point p = five.GetItem(i).GetPoint(k);
            if(::GetCell(p) == CELL_TYPE_EMPTY && ++cnt.Cnt(p) >= four.Size() + five.Size() && !block.Find(p, isBlack, 0)) {
                block.PushBack(p, isBlack, 0);
            }
        }
    }
}

void MoveGenerator::SingleByEvals(bool isBlack, int sz, MGMoveArray &arr) {
    arr.Clear();
    FOR_EACH_CELL(r, c) {
        if(::GetCell(r, c) != CELL_TYPE_EMPTY) {
            continue;
        }
        int scr = MoveEvaluate(r, c, isBlack);
        if(arr.Size() < sz) {
            arr.PushBack(r, c, isBlack, scr);
        } else {
            MGMove &min = arr.MinScoreRef();
            if(min._score < scr) {
                min = MGMove(r, c, isBlack, scr);
            }
        }
    }
}

void MoveGenerator::SingleByEvals(bool isBlack, Zone &zone, int sz, MGMoveArray &arr) {
    arr.Clear();
    FOR_EACH(i, zone.Size()) {
        Point p = zone.GetPoint(i);
        if(::GetCell(p) != CELL_TYPE_EMPTY) {
            continue;
        }
        int scr = MoveEvaluate(p, isBlack);
        if(arr.Size() < sz) {
            arr.PushBack(p, isBlack, scr);
        } else {
            MGMove &min = arr.MinScoreRef();
            if(min._score < scr) {
                min = MGMove(p, isBlack, scr);
            }
        }
    }
}

void MoveGenerator::Couple(bool isBlack, Zone &zone, MGMoveArray &prom, int sz, MGDMoveArray &arr) {
    arr.Clear();
    FOR_EACH(i, prom.Size()) {
        FOR_EACH(k, zone.Size()) {
            Point p = zone.GetPoint(k);
            DMove dm(prom.ItemRef(i).GetPoint(), zone.GetPoint(k), isBlack);
            if(! dm.IsValid()) {
                continue;
            }
            int scr = DMoveEvaluate(dm);
            if(arr.Find(dm, scr)) {
                continue;
            }
            if(arr.Size() < sz) {
                arr.PushBack(dm, scr);
            } else {
                MGDMove &min = arr.MinScoreRef();
                if(min._score < scr) {
                    min = MGDMove(dm, scr);
                }
            }
        }
    }
}

void MoveGenerator::Couple(bool isBlack, MGMoveArray &prom, int sz, MGDMoveArray &arr) {
    arr.Clear();
    FOR_EACH(i, prom.Size()) {
        FOR_EACH_CELL(r, c) {
            DMove dm(prom.ItemRef(i).GetPoint(), Point(r, c), isBlack);
            if(! dm.IsValid()) {
                continue;
            }
            int scr = DMoveEvaluate(dm);
            if(arr.Find(dm, scr)) {
                continue;
            }
            if(arr.Size() < sz) {
                arr.PushBack(dm, scr);
            } else {
                MGDMove &min = arr.MinScoreRef();
                if(min._score < scr) {
                    min = MGDMove(dm, scr);
                }
            }
        }
    }
}

void MoveGenerator::GenerateSingleBlocks(bool isBlack, MGDMoveArray &arr) {
    static MGMoveArray block;
    SingleBlocks(isBlack, block);
    static Zone zone;
    FOR_EACH(i, block.Size()) {
        ::MakeMove(block.ItemRef(i));
    }
    FOR_EACH(i, block.Size()) {
        ::UnmakeLastMove();
    }
    
    Couple(isBlack, block, _SINGLE_BLOCKS, arr);
}


void MoveGenerator::GenerateThreats(bool isBlack, MGDMoveArray &arr) {
    arr.Clear();
    SegmentTable::Table &three = ::GetSegmentTable(isBlack, 3);
    static MGMoveArray threat;
    threat.Clear();
    FOR_EACH(i, three.Size()) {
        FOR_EACH(k, 6) {
            Point p = three.GetItem(i).GetPoint(k);
            if(::GetCell(p) == CELL_TYPE_EMPTY && !threat.Find(p, isBlack, 0)) {
                threat.PushBack(p, isBlack, 0);
            }
        }
    }
    Couple(isBlack, threat, _THREAT_MOVES, arr);
}

void MoveGenerator::GeneratePreBlock(bool isBlack, MGDMoveArray &preBlock) {
    static Zone zone;
    zone.Clear();
    static MGMoveArray arr;
    arr.Clear();
    zone.AddSegmentTable(::GetSegmentTable(!isBlack, 3));
    zone.AddSegmentTable(::GetSegmentTable(!isBlack, 2));
    FOR_EACH(i, zone.Size()) {
        if(::GetCell(zone.GetPoint(i)) == CELL_TYPE_EMPTY) {
            arr.PushBack(zone.GetPoint(i), isBlack, 0);
        }
    }
    Couple(isBlack, zone, arr, _PRE_BLOCKS, preBlock);
}

void MoveGenerator::GenerateByEvals(bool isBlack, MGDMoveArray &arr) {
    static MGMoveArray eval;
    SingleByEvals(isBlack, _SINGLE_BY_EVALS, eval);
    Couple(isBlack, eval, _BY_EVALS, arr);
}

#include "utilities.h"

void MoveGenerator::GenerateOpenings(bool isBlack, MGDMoveArray &arr) {
    GeneratePreBlock(isBlack, arr);
    static MGDMoveArray con;
    GeneratePreConnect(isBlack, con);
    FOR_EACH(i, con.Size()) {
        if(! arr.Find(con.ItemRef(i))) {
            arr.PushBack(con.ItemRef(i));
        }
    }
    if(arr.Size() == 0) {
        GenerateByEvals(isBlack, arr);
    }
    //arr.Clear();
    //static Counter cnt;
    //cnt.Zero();
    //static Zone zone;
    //zone.Clear();
    //FOR_EACH(i, 6) {
    //    SegmentTable::Table &tab = ::GetSegmentTable(!isBlack, i);
    //    FOR_EACH(k, tab.Size()) {
    //        FOR_EACH(m, 6) {
    //            Point p = tab.GetItem(k).GetPoint(m);
    //            if((cnt.Cnt(p) += _blockScore[i] + _connectScore[i]) < _OPENING_EVAL) {
    //                continue;
    //            }
    //            if(::GetCell(p) == CELL_TYPE_EMPTY && !zone.IsInZone(p)) {
    //                zone.AddPoint(p);
    //            }
    //        }
    //    }
    //}
    //FOR_EACH(i, zone.Size()) {
    //    FOR_EACH(k, i) {
    //        int scr = DMoveEvaluate(zone.GetPoint(i), zone.GetPoint(k), isBlack);
    //        assert(scr >= 0);
    //        if(arr.Size() < _OPENINGS) {
    //            arr.PushBack(MGDMove(zone.GetPoint(i), zone.GetPoint(k), isBlack, scr));
    //        } else {
    //            MGDMove &min = arr.MinScoreRef();
    //            if(min._score < scr) {
    //                min = MGDMove(zone.GetPoint(i), zone.GetPoint(k), isBlack, scr);
    //            }
    //        }
    //    }
    //}
}

//void MoveGenerator::GeneratePreBlock(bool isBlack, MGDMoveArray &arr) {
//    throw std::exception("The method or operation is not implemented.");
//}

//void MoveGenerator::CoupleThreats(bool isBlack, MGMoveArray &threat, MGDMoveArray &arr) {
//    CouplePromisings(isBlack, threat, arr);
//}

MoveGenerator moveGenerator;

MGMove &MGMoveArray::MinScoreRef() {
    int ind = 0;
    FOR_EACH(i, Size()) {
        if(ItemRef(i)._score < ItemRef(ind)._score) {
            ind = i;
        }
    }
    return ItemRef(ind);
}

MGDMove & MGDMoveArray::MaxScoreRef() {
    int ind = 0;
    FOR_EACH(i, Size()) {
        if(ItemRef(i)._score > ItemRef(ind)._score) {
            ind = i;
        }
    }
    return ItemRef(ind);
}

MGDMove &MGDMoveArray::MinScoreRef() {
    int ind = 0;
    FOR_EACH(i, Size()) {
        if(ItemRef(i)._score < ItemRef(ind)._score) {
            ind = i;
        }
    }
    return ItemRef(ind);
}

const int MoveGenerator::_blockScore[6] = {2, 555, 666, 777, 997, 1259};
const int MoveGenerator::_connectScore[6] = {2, 333, 444, 555, 201, 401};

void MoveGenerator::GeneratePreConnect(bool isBlack, MGDMoveArray &con) {
    static Zone zone;
    zone.Clear();
    static MGMoveArray arr;
    arr.Clear();
    zone.AddSegmentTable(::GetSegmentTable(isBlack, 1));
    zone.AddSegmentTable(::GetSegmentTable(isBlack, 2));
    FOR_EACH(i, zone.Size()) {
        if(::GetCell(zone.GetPoint(i)) == CELL_TYPE_EMPTY) {
            arr.PushBack(zone.GetPoint(i), isBlack, 0);
        } else {
            //zone.Remove(zone.GetPoint(i));
        }
    }
    Couple(isBlack, zone, arr, _PRE_CONNECTS, con);
}

void MoveGenerator::GenerateExtremThreats(bool isBlack, MGDMoveArray &arr) {
    arr.Clear();
    SegmentTable::Table &tab = ::GetSegmentTable(isBlack, 2);
    FOR_EACH(i, tab.Size()) {
        FOR_EACH(a, 6) {
            FOR_EACH(b, a) {
                Point p = tab.GetItem(i).GetPoint(a);
                Point q = tab.GetItem(i).GetPoint(b);
                if(::GetCell(p) != CELL_TYPE_EMPTY || ::GetCell(q) != CELL_TYPE_EMPTY) {
                    continue;
                }
                int scr = DMoveEvaluate(p, q, isBlack);
                if(arr.Find(p, q, isBlack, scr)) {
                    continue;
                }
                if(arr.Size() < _EXTREM_THREATS) {
                    arr.PushBack(p, q, isBlack, scr);
                } else {
                    MGDMove &min = arr.MinScoreRef();
                    if(min._score < scr) {
                        min = MGDMove(p, q, isBlack, scr);
                    }
                }
            }
        }
    }
}

