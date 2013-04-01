#include "MoveGenerator.h"
#include "MoveSearcher.h"
//#include "MoveArray.h"
#include "Zone.h"
#include "DTSSer.h"
#include "utilities.h"

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
    GenerateThreats(isBlack, arr);
    static MGDMoveArray block;
    GeneratePreBlock(isBlack, block);
    FOR_EACH(i, block.Size()) {
        arr.PushBack(block.ItemRef(i));
    }
    static MGDMoveArray prom;
    if(arr.Size() > 0) {
        return;
    } else {
        GenerateByEvals(isBlack, prom);
        FOR_EACH(i, prom.Size()) {
            arr.PushBack(prom.ItemRef(i));
        }
        //if(dtsser.RelavanceZone(!isBlack, zone) && zone.Size() > 2) {
        //    prom.Clear();
        //    FOR_EACH(i, zone.Size()) {
        //        if(::GetCell(zone.GetPoint(i)) == CELL_TYPE_EMPTY) {
        //            prom.PushBack(zone.GetPoint(i), isBlack, 0);
        //        } else {
        //       //     zone.Remove(zone.GetPoint(i));
        //        }
        //    }
        //    Couple(isBlack, zone, prom, _PROMISINGS, arr);
        //    return;
        //}
        //Promisings(isBlack, _SINGLE_PROMISINGS, prom);
        //Couple(isBlack, prom, _PROMISINGS, arr);
    }
    //static MGDMoveArray preBlock;
    //GeneratePreBlock(isBlack, preBlock);
    //if(dMoveArray.Size() > 0) {
    //    return;
    //} else
    //if(dtsser.RelavanceZone(!isBlack, zone) && zone.Size() > 1) {
    //    moveArray.Clear();
    //    FOR_EACH(i, zone.Size()) {
    //        if(::GetCell(zone.GetPoint(i)) == CELL_TYPE_EMPTY) {
    //            moveArray.Add(zone.GetPoint(i), isBlack);
    //        }
    //    }
    //    //Promisings(isBlack, zone, moveArray);
    //    CouplePromisings(isBlack, zone, moveArray, dMoveArray);
    //} else {
    //    Promisings(isBlack, moveArray);
    //    CouplePromisings(isBlack, moveArray, dMoveArray);
    //}
}

//void MoveGenerator::CoupleSingleBlocks(bool isBlack, MGMoveArray &block, MGDMoveArray &arr) {
//    arr.Clear();
//
//    FOR_EACH(i, block.Size()) {
//        FOR_EACH_CELL(r, c) {
//            DMove dm(block.ItemRef(i).GetPoint(), Point(r, c), isBlack);
//            if(! dm.IsValid()) {
//                continue;
//            }
//            int scr = DMoveEvaluate(dm);
//            if(dm.GetPoint1() == dm.GetPoint2() || arr.Find(dm, scr)) {
//                continue;
//            }
//            if(arr.Size() < _SINGLE_BLOCKS) {
//                arr.PushBack(dm, scr);
//            } else {
//                MGDMove &min = arr.MinScoreRef();
//                if(min._score < scr) {
//                    min = MGDMove(dm, scr);
//                }
//            }
//        }
//    }
//    assert(arr.Size() > 0);
//}
//
//void MoveGenerator::CoupleSingleBlocks(bool isBlack, Zone &zone, MGMoveArray &block, MGDMoveArray &arr) {
//    arr.Clear();
//
//    assert(block.Size() > 0);
//    assert(zone.Size() > 0);
//
//    FOR_EACH(i, block.Size()) {
//        FOR_EACH(k, zone.Size()) {
//            Point p = zone.GetPoint(k);
//            DMove dm(block.GetItem(i).GetPoint(), p, isBlack);
//            if(! dm.IsValid()) {
//                continue;
//            }
//            int scr = DMoveEvaluate(dm);
//            if(arr.Find(dm, scr)) {
//                continue;
//            }
//            if(dMoveArray.Size() < _SINGLE_BLOCKS) {
//                arr.PushBack(dm, isBlack);
//            } else {
//                MGDMove &min = arr.MinScore();
//                if(min._score < scr) {
//                    min = MGDMove(dm, scr);
//                }
//            }
//        }
//    }
//    assert(dMoveArray.Size() > 0);
//}

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
    //bool isRele = dtsser.RelavanceZone(!isBlack, zone) && zone.Size() > 0;
    FOR_EACH(i, block.Size()) {
        ::UnmakeLastMove();
    }
/*    if(isRele) {
        Couple(isBlack, zone, block, _SINGLE_BLOCKS, arr);
    } else */{
        Couple(isBlack, block, _SINGLE_BLOCKS, arr);
    }
}

//#include "utilities.h"

//int MoveGenerator::MoveEvaluate(int row, int col, bool isBlack) {
//    static int dr[] = {0, -1, -1, 1, 0, 1, 1, -1, 0};
//    static int dc[] = {-1, 0, -1, -1, 1, 0, 1, 1, -1};
//    CellType connect = (isBlack ? CELL_TYPE_BLACK : CELL_TYPE_WHITE);
//    CellType block = (isBlack ? CELL_TYPE_WHITE : CELL_TYPE_BLACK);
//    int ret = 0;
//    FOR_EACH(dir, 8) {
//        int r, c;
//        int i = 1;
//        while(i <= _SCORE_TAB_SZ && ::IsInsideBoard(r = row + i * dr[dir], c = col + i * dc[dir])) {
//            if(::GetCell(r, c) == connect) {
//                ret += _connectScore[i - 1];
//                break;
//            } else if(::GetCell(r, c) == block) {
//                ret += _blockScore[i - 1];
//                break;
//            }
//            ++i;
//        }
//    }
//    return ret;
//}
//
//int MoveGenerator::DMoveEvaluate(const DMove &dm) {
//    ::MakeMove(dm.GetPoint1(), dm._isBlack);
//    int s1 = MoveEvaluate(dm.GetPoint2(), dm._isBlack);
//    ::UnmakeLastMove();
//    ::MakeMove(dm.GetPoint2(), dm._isBlack);
//    int s2 = MoveEvaluate(dm.GetPoint1(), dm._isBlack);
//    ::UnmakeLastMove();
//    return s1 + s2;
//}

//MoveGenerator::MoveGenerator() {
//    FILE *file = fopen("ConnectScore.txt", "r");
//    if(file == NULL) {
//        printf("cannot open ConnectScore.txt\n");
//        exit(1);
//    }
//    char line[1024];
//    int i = 0;
//    int scr;
//    while(i < _SCORE_TAB_SZ && fgets(line, sizeof(line) - 1, file) != NULL) {
//        if(sscanf(line, "%d", &scr) == 1) {
//            printf("connect score %d: %d\n", i, scr);
//            _connectScore[i++] = scr;
//        }
//    }
//    if(i != _SCORE_TAB_SZ) {
//        printf("bad ConnectScore.txt\n");
//        exit(1);
//    }
//    fclose(file);
//    file = fopen("BlockScore.txt", "r");
//    if(file == NULL) {
//        printf("cannot open BlockScore.txt\n");
//        exit(1);
//    }
//    i = 0;
//    while(i < _SCORE_TAB_SZ && fgets(line, sizeof(line) - 1, file) != NULL) {
//        if(sscanf(line, "%d", &scr) == 1) {
//            printf("block score %d: %d\n", i, scr);
//            _blockScore[i++] = scr;
//        }
//    }
//    if(i != _SCORE_TAB_SZ) {
//        printf("bad BlockScore.txt\n");
//        exit(1);
//    }
//    fclose(file);
//}

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
        } else {
            //zone.Remove(zone.GetPoint(i));
        }
    }
    Couple(isBlack, zone, arr, _PRE_BLOCKS, preBlock);
    //Couple(isBlack, arr, _PRE_BLOCKS, preBlock);
}

void MoveGenerator::GenerateByEvals(bool isBlack, MGDMoveArray &arr) {
    static MGMoveArray eval;
    SingleByEvals(isBlack, _SINGLE_BY_EVALS, eval);
    Couple(isBlack, eval, _BY_EVALS, arr);
}

#include "utilities.h"

void MoveGenerator::GenerateOpenings(bool isBlack, MGDMoveArray &arr) {
    arr.Clear();
    static Counter cnt;
    cnt.Zero();
    static Zone zone;
    zone.Clear();
    FOR_EACH(i, 6) {
        SegmentTable::Table &tab = ::GetSegmentTable(!isBlack, i);
        FOR_EACH(k, tab.Size()) {
            FOR_EACH(m, 6) {
                Point p = tab.GetItem(k).GetPoint(m);
                if((cnt.Cnt(p) += _blockScore[i] + _connectScore[i]) < _OPENING_EVAL) {
                    continue;
                }
                if(::GetCell(p) == CELL_TYPE_EMPTY && !zone.IsInZone(p)) {
                    zone.AddPoint(p);
                }
            }
        }
    }
    FOR_EACH(i, zone.Size()) {
        FOR_EACH(k, i) {
            int scr = DMoveEvaluate(zone.GetPoint(i), zone.GetPoint(k), isBlack);
            assert(scr >= 0);
            if(arr.Size() < _OPENINGS) {
                arr.PushBack(MGDMove(zone.GetPoint(i), zone.GetPoint(k), isBlack, scr));
            } else {
                MGDMove &min = arr.MinScoreRef();
                if(min._score < scr) {
                    min = MGDMove(zone.GetPoint(i), zone.GetPoint(k), isBlack, scr);
                }
            }
        }
    }
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

