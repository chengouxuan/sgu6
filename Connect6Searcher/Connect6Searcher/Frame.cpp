#include "Frame.h"
#include "connect6.h"
//#include "DMoveArray.h"
#include "ComplexBoard.h"
#include "MoveSearcher.h"
#include "MoveGenerator.h"
#include <algorithm>

extern bool g_breakSearch;


void Frame::SortByHistoryScore(MGDMoveArray &dMove) {
    FOR_EACH(i, dMove.Size()) {
        FOR_EACH(k, dMove.Size() - i - 1) {
            if(historyScore.GetScore(dMove.ItemRef(k)) < historyScore.GetScore(dMove.ItemRef(k + 1))) {
                std::swap(dMove.ItemRef(k), dMove.ItemRef(k + 1));
            }
        }
    }
}

int Frame::NegaMax(bool isBlack, int depth, int lower, int upper) {
#ifdef _DEBUG
    printf("%s depth = %d\n", isBlack ? "B" : "W", depth);
    ::PrintComplexBoard();
#endif
    int eval;
    DMove dm = RandomDMove(isBlack);
    if(GetTransTable(isBlack).LookUp(depth, lower, upper, eval, dm)) {
        ++GetTransTable(isBlack)._hits;
        return eval;
    } else if(IsGameOver(isBlack, eval, dm)) {
        GetTransTable(isBlack).Enter(_depth_limit, Item::TYPE_SUCCESS, eval, dm);
        return eval;
    } else if(depth <= 0) {
        return evaluator.Evaluate(isBlack, depth);
    } else if(g_breakSearch) {
        return evaluator.Evaluate(isBlack, depth);
    }
    ++_nodes;
    bool isSuccess = false;
    int maxScore = -INFINITY;
    MGDMoveArray &arr = moveGenerator.Generate(isBlack, depth);
    SortByHistoryScore(arr);
    FOR_EACH(i, arr.Size()) {
        ::MakeDMove(arr.ItemRef(i));
        int score = -NegaMax(!isBlack, depth - 1, -upper, -lower);
        ::UnmakeLastDMove();
        if(score > maxScore || maxScore == -INFINITY) {
            dm = arr.ItemRef(i);
            maxScore = score;
        }
        if(maxScore >= upper) {
            GetTransTable(isBlack).Enter(depth, Item::TYPE_FAIL_HIGH, maxScore, dm);
            historyScore.IncreaseScore(arr.ItemRef(i), 1 << depth);
            return maxScore;
        }
        if(maxScore > lower) {
            lower = maxScore;
            isSuccess = true;
        }
    }
    GetTransTable(isBlack).Enter(depth, isSuccess ? Item::TYPE_SUCCESS : Item::TYPE_FAIL_LOW, maxScore, dm);
    return maxScore;
}

bool Frame::FindWinningMove(bool isBlack, DMove &dm) {
    SegmentTable::Table &four = GetSegmentTable(isBlack, 4);
    SegmentTable::Table &five = GetSegmentTable(isBlack, 5);
    if(four.Size() > 0) {
        SegmentTable::Item seg = four.GetItem(0);
        FOR_EACH(i, 6) {
            FOR_EACH(k, i) {
                Point pi = seg.GetPoint(i), pk = seg.GetPoint(k);
                if(::GetCell(pi) == CELL_TYPE_EMPTY && ::GetCell(pk) == CELL_TYPE_EMPTY) {
                    dm = DMove(pi, pk, isBlack);
                    return true;
                }
            }
        }
        assert(false);
    } else if(five.Size() > 0) {
        dm = RandomDMove(isBlack);
        SegmentTable::Item seg = five.GetItem(0);
        FOR_EACH(i, 6) {
            Point p = seg.GetPoint(i);
            if(DMove(dm.GetPoint1(), p, isBlack).IsValid()) {
                dm = DMove(dm.GetPoint1(), p, isBlack);
                return true;
            } else if(DMove(p, dm.GetPoint2(), isBlack).IsValid()) {
                dm = DMove(p, dm.GetPoint2(), isBlack);
                return true;
            }
        }
        assert(false);
    }
    return false;
}

//int Frame::MiniWind(bool isBlack, int depth, int lower, int upper) {
//    assert(lower < upper);
//#ifdef _DEBUG
//    printf("%s depth = %d\n", isBlack ? "B" : "W", depth);
//    ::PrintComplexBoard();
//#endif
//    int eval;
//    DMove dm = RandomDMove(isBlack);
//    if(GetTransTable(isBlack).LookUp(depth, lower, upper, eval, dm)) {
//        ++GetTransTable(isBlack)._hits;
//        return eval;
//    } else if(IsGameOver(isBlack, eval, dm)) {
//        GetTransTable(isBlack).Enter(_DEPTH_LIMIT, Item::TYPE_SUCCESS, eval, dm);
//        return eval;
//    } else if(depth <= 0) {
//        return evaluator.Evaluate(isBlack);
//    }
//    ++_nodes;
//    MGDMoveArray &arr = moveGenerator.Generate(isBlack, depth);
//    assert(arr.Size() > 0);
//    SortByHistoryScore(arr);
//    int maxScore = -INFINITY;
//    FOR_EACH(i, arr.Size()) {
//        int wnd = (maxScore > -upper ? maxScore : -upper);//*/std::max(maxScore, -upper);
//        ::MakeDMove(arr.ItemRef(i));
//        int score = -MiniWind(!isBlack, depth - 1, wnd, wnd + 1);
//        ::UnmakeLastDMove();
//        if(wnd < score && score < upper) {
//            ::MakeDMove(arr.ItemRef(i));
//            maxScore = -MiniWind(!isBlack, depth - 1, score, -lower);
//            ::UnmakeLastDMove();
//            dm = arr.ItemRef(i);
//        } else if(maxScore < score || maxScore == -INFINITY) {
//            maxScore = score;
//            dm = arr.ItemRef(i);
//        }
//        if(upper <= maxScore) {
//            historyScore.IncreaseScore(arr.ItemRef(i), 1 << depth);
//            GetTransTable(isBlack).Enter(depth, Item::TYPE_FAIL_HIGH, maxScore, arr.ItemRef(i));
//            return maxScore;
//        }
//    }
//    Item::Type type = (lower < maxScore && maxScore < upper ? Item::TYPE_SUCCESS : Item::TYPE_FAIL_LOW);
//    GetTransTable(isBlack).Enter(depth, type, maxScore, dm);
//    return maxScore;
//}

int Frame::IDMTDF(bool isBlack) {
    int depth = 1;
    clock_t cl = clock();
    int step = 1500000;
    int eval;
    bool b;
    static int g[2] = {0, 0};
#ifdef _DEBUG
    while(depth <= _depth_limit && !g_breakSearch) {
#else
    while(depth <= _depth_limit && clock() - cl < _time_limit && !g_breakSearch) {
#endif
        printf("depth limit = %d\n", depth);
        int lower = -INFINITY, upper = +INFINITY;
        int guess = g[depth % 2];
        while(lower + step < upper) {
            printf("lower = %d, upper = %d, guess = %d\n", lower, upper, guess);
            int score = NegaMax(isBlack, depth, guess, guess + 1);
            b = GetTransTable(isBlack).LookUp(0, guess, guess + 1, eval, _dMove);
            if (! b) {
                printf ("look up transposition table failed\n");
            }
            if(score <= guess) {
                upper = score;
                guess = upper - 1 - step;
            } else {
                lower = score;
                guess = lower + step;
            }
        }
        // binary search like
        guess = (lower + upper) / 2;
        g[depth % 2] = guess;
        int oldGuess = guess - 1;
        while(lower < upper && oldGuess != guess) {
            printf("lower = %d, upper = %d, guess = %d\n", lower, upper, guess);
            int score = NegaMax(isBlack, depth, guess, guess + 1);
            b = GetTransTable(isBlack).LookUp(0, guess, guess + 1, eval, _dMove);
            if(score <= guess) {
                upper = score;
            } else {
                lower = score;
            }
            oldGuess = guess;
            guess = (lower + upper) / 2;
        }
        g[depth++ % 2] = guess;
        searcher.SetDMove(_dMove);
        printf("%d ms\n", clock() - cl);
    }
    return g[1];
}

int Frame::_depth_limit;
int Frame::_time_limit;

Frame frame;