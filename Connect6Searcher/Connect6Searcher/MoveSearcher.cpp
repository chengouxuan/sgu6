#include "Connect6.h"
#include "defines.h"
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <algorithm>
#include "MoveSearcher.h"
#include "MoveArray.h"
//#include "DMoveArray.h"
#include "Zone.h"
#include "DTSSer.h"
#include "HistoryScore.h"
#include "Frame.h"
#include <queue>
#include <numeric>
#include "OpeningFinder.h"

DWORD SearchThread(LPVOID p) {
    frame.Search(searcher._board, searcher._isBlack);
    SetEvent(searcher._hEventSearchComplete);
    return 0;
}

extern bool g_breakSearch;

void MoveSearcher::SearchGoodMoves(Board board, bool isBlack, int moves /* = 2 */) {
    if(moves == 1) {
        _dMove._r1 = ROW_MAX / 2;
        _dMove._c1 = COL_MAX / 2;
        return;
    } else if(openingFinder.Find(board, isBlack)) {
        _dMove = openingFinder._dMove;
        return;
    }
    // begin the search thread
    _isBlack = isBlack;
    memcpy(_board, board, sizeof(Board));
    _hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&SearchThread, NULL, 0, &_threadId);
    if(_hThread == NULL) {
        printf("search thread fail\n");
        exit(1);
    }
    SetThreadPriority(_hThread, THREAD_PRIORITY_HIGHEST);
    printf("thread id = %d\n", _threadId);
#ifdef _DEBUG
    if(WAIT_TIMEOUT == WaitForSingleObject(_hEventSearchComplete, INFINITY)) {
#else
    if(WAIT_TIMEOUT == WaitForSingleObject(_hEventSearchComplete, _time_limit)) {
#endif
        printf("time out\n");

        g_breakSearch = true;
        if (WAIT_TIMEOUT == WaitForSingleObject(_hEventSearchComplete, 10000)) {
            printf ("search terminated\n");
        } else {
            printf ("search break\n");
        }

    } else {
        printf("search complete\n");
    }

    WaitForSingleObject(_hMutexDMove, INFINITY);
    TerminateThread(_hThread, 0);
    CloseHandle(_hThread);
    ReleaseMutex(_hMutexDMove);
}


DMove RandomDMove(bool isBlacksTurn) {
    DMove ret;
    do {
        ret = DMove(rand() % ROW_MAX, rand() % COL_MAX, rand() % ROW_MAX, rand() % COL_MAX, isBlacksTurn);
    } while(! ret.IsValid());
    return ret;
}

int CountThreats(bool isBlack) {
    SegmentTable::Table &four = GetSegmentTable(!isBlack, 4);
    SegmentTable::Table &five = GetSegmentTable(!isBlack, 5);
    if(four.Size() + five.Size() == 0) {
        return 0;
    }
    SegmentTable::Table &tab = (four.Size() == 0 ? five : four);
    FOR_EACH(k, 6) {
        Point p = tab.GetItem(0).GetPoint(k);
        if(::GetCell(p) != CELL_TYPE_EMPTY) {
            continue;
        }
        ::MakeMove(p, isBlack);
        int t = four.Size() + five.Size();
        ::UnmakeLastMove();
        if(t == 0) {
            return 1;
        }
    }
    return 2;
}

// read a line of file 'f', and use 'format' to scanf 'p'
void ReadAParam(FILE *f, const char *format, void *p) {
    char buf[1024];
    if(fgets(buf, sizeof(buf), f) == NULL || sscanf(buf, format, p) == EOF) {
        MessageBox(NULL, TEXT("read a param\n"), NULL, MB_OK);
        exit(1);
    }
}

void ReInitParams() {
    FILE *f = fopen("params.txt", "r");
    if(f == NULL) {
        MessageBox(NULL, TEXT("read params\n"), NULL, MB_OK);
        exit(1);
    }
    ReadAParam(f, "%d", (void *)&frame._depth_limit);
    printf("%d read\n", frame._depth_limit);

    ReadAParam(f, "%d", (void *)&frame._time_limit);
    printf("%d read\n", frame._time_limit);

    ReadAParam(f, "%d", (void *)&searcher._time_limit);
    printf("%d read\n", searcher._time_limit);

    ReadAParam(f, "%d", (void *)&dtsser._dtss_depth);
    printf("%d read\n", dtsser._dtss_depth);

    ReadAParam(f, "%d", (void *)&dtsser._id_dtss_depth);
    printf("%d read\n", dtsser._id_dtss_depth);

    fclose(f);
}

int MoveSearcher::_time_limit;

MoveSearcher searcher;