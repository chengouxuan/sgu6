// main.cpp : 定义控制台应用程序的入口点。
//

#include "Connect6.h"
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <cassert>
#include <cstdio>
#include "defines.h"
#include "MoveSearcher.h"
#include "utilities.h"
#include "Frame.h"

#pragma comment(lib,"WS2_32.lib")

extern MoveSearcher searcher;

#include <queue>

using std::queue;

struct ReqQueData
{
    RequestData reqData;
    SOCKADDR_IN addr;
    int sock;

    ReqQueData() { memset(this, 0, sizeof(*this)); }
    ReqQueData(RequestData rd, SOCKADDR_IN ad, int sc)
        : reqData(rd)
        , addr(ad)
        , sock(sc) {}
};

class ReqQue
{
    queue <ReqQueData> que;
    HANDLE hMutex;
    static ReqQue inst;

public:
    ReqQue() {
        TCHAR mutexName[1024];
        tsprintf(mutexName, TEXT("ReqQue::hMutex: %d"), GetCurrentProcessId());
        if ((hMutex = CreateMutex(NULL, false, mutexName)) == NULL) {
            printf("error creating mutex\n");
            exit(1);
        }
    }

    bool getReq(RequestData *rd, SOCKADDR_IN *ad, int *sc)
    {
        WaitForSingleObject(hMutex, INFINITY);

        bool hasRequest = (que.size() > 0);
        if (hasRequest && rd != NULL) {
            *rd = que.front().reqData;
        }

        if (hasRequest && ad != NULL) {
            *ad = que.front().addr;
        }

        if (hasRequest && sc != NULL) {
            *sc = que.front().sock;
        }

        if (hasRequest) {
            que.pop();
        }

        ReleaseMutex(hMutex);

        return hasRequest;
    }

    void putReq(RequestData *rd, SOCKADDR_IN *ad, int *sc)
    {
        WaitForSingleObject(hMutex, INFINITY);

        ReqQueData data;

        if (rd != NULL) {
            data.reqData = *rd;
        }

        if (ad != NULL) {
            data.addr =  *ad;
        }

        if (sc != NULL) {
            data.sock = *sc;
        }

        if (rd != NULL || ad != NULL || sc != NULL) {
            que.push(data);
        }

        ReleaseMutex(hMutex);
    }

    void clearAllReq()
    {
        WaitForSingleObject(hMutex, INFINITY);

        while (! que.empty()) {
            ReqQueData data = que.front();
            que.pop();
            closesocket(data.sock);
        }

        ReleaseMutex(hMutex);
    }

    static ReqQue *instance()
    {
        return &inst;
    }
};

ReqQue ReqQue::inst;

DWORD SearchRequestProcessing(LPVOID);

bool g_breakSearch;

void ReInitParams();


#include "opening/opening.h"

Opening opening;

int main(int argc, char* argv[])
{
    ReInitParams();

    opening.BuildTable();

    if (argc != 2) {
        printf("argument error\n");
        return 1;
    }

    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(1, 1);
    if (WSAStartup(wVersionRequested, &wsaData) != 0) {
        printf("socket startup error.\n");
        return 1;
    }

    if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
        printf("socket version error.\n");
        return 1;
    }

    printf("port = %d\n", atoi(argv[1]));

    SOCKET sockSrv;
    if ((sockSrv = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf ("socket creation error.\n");
        return 1;
    }

    SOCKADDR_IN addrSrv;
    addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons((u_short)atoi(argv[1]));

    if (bind(sockSrv, (SOCKADDR *)(&addrSrv), sizeof(SOCKADDR)) < 0) {
        printf ("socket binding error.\n");
        return 1;
    }

    if (listen(sockSrv, 10) < 0) {
        printf ("socket listening error.\n");
        return 1;
    }

    HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&SearchRequestProcessing, NULL, 0, NULL);
    if (hThread == NULL) {
        printf ("create thread error.\n");
        return 1;
    }

    for(;;) {

        SOCKADDR_IN addrClient;
        int len = sizeof(addrClient);
        int sockConn;
        if ((sockConn = accept(sockSrv, (SOCKADDR *)(&addrClient), &len)) < 0) {
            printf ("socket acception error.\n");
            continue;
        }
        
        RequestData r;
        int recvLen = 0;
        do {
            recvLen += recv(sockConn, (char *)(&r) + recvLen, sizeof(RequestData) - recvLen, 0);
        } while (recvLen < sizeof(RequestData));

        switch (r._type) {
        case RequestData::TYPE_EXIT:
            exit(0);
            break;

        case RequestData::TYPE_SEARCH:
            ReqQue::instance()->putReq(&r, &addrClient, &sockConn);
            break;

        case RequestData::TYPE_BREAK_ALL_SEARCH:
            ReqQue::instance()->clearAllReq();
            g_breakSearch = true;
            break;

        default:
            printf("unhandled case: type = %d\n", r._type);
            break;
        }
    }

    return 0;
}

DWORD SearchRequestProcessing(LPVOID)
{
    for (;;) {

        SOCKADDR_IN addr;
        RequestData req;
        int sock;
        while (! ReqQue::instance()->getReq(&req, &addr, &sock)) {}

        clock_t cl = clock();

        g_breakSearch = false;

        MoveData m;
        DMove dm;
        if (! opening.GetDMove(req._data._searchData._board, req._data._searchData._isBlack, &dm)) {
            searcher.SearchGoodMoves(req._data._searchData._board, req._data._searchData._isBlack, req._data._searchData._moves);
            m = MoveData(
                searcher.GetDMove()._r1, searcher.GetDMove()._c1,
                searcher.GetDMove()._r2, searcher.GetDMove()._c2);
        } else {
            m = MoveData(dm._r1, dm._c1, dm._r2, dm._c2);
        }

        PrintBoard(req._data._searchData._board);


        if(req._data._searchData._moves == 2) {
            printf("(%02d, %c), (%02d, %c)\n", m._r1, m._c1 + 'A', m._r2, m._c2 + 'A');
        } else {
            printf("(%02d, %c)\n", m._r1, m._c1 + 'A');
        }

        int d = clock() - cl;
        printf("%d.%03d seconds\n", d / 1000, d % 1000);
        printf("%d evaluations\n", evaluator._evaluations);
        printf("%d evaluator hits\n", evaluator._transTable._hits);
        printf("%d nodes\n", frame._nodes);
        printf("%d hits\n", frame._transTableBlack._hits + frame._transTableWinte._hits);
        printf("%d dtss nodes\n", dtsser._nodes);
        printf("%d dtss hits\n", dtsser._transTableBlack._hits + dtsser._transTableWhite._hits);
        printf("%d dtss dropped searches\n", dtsser._dropedSearches);
        printf("%d idtss successes\n", dtsser._idtssSuccesses);

        send(sock, (char *)(&m), sizeof(MoveData), 0);
        closesocket(sock);
    }
}