#include "opening.h"
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include "../saver/saver.h"

#ifdef _DEBUG
#define debug_printf(...) fprintf(stderr, __VA_ARGS__)
#else
#define debug_printf(...) 0
#endif

#ifdef NDEBUG
#undef assert
#define assert(exp) (((exp) || fprintf(stderr, "assertion %s failed on line %d, file %s\n", #exp, __LINE__, __FILE__)), (void)0)
#endif

void OpeningItem::SetMove( int ind, int row, int col, bool isBlack )
{
    assert(0 <= ind && ind < MOVES);
    assert(0 <= row && row < ROW_MAX);
    assert(0 <= col && col < COL_MAX);

    BITMASK_MOVE_SET_ROW(move[ind], row);
    BITMASK_MOVE_SET_COL(move[ind], col);
    BITMASK_MOVE_SET_IS_BLACK(move[ind], isBlack);
}

bool OpeningItem::IsToGoEqual( int r1, int c1, int r2, int c2, bool isBlack )
{
    if (!!toGoIsBlack != !!isBlack) {
        return false;
    }

    if (r1 == toGoR1 && c1 == toGoC1 && r2 == toGoR2 && c2 == toGoC2) {
        return true;
    }

    if (r1 == toGoR2 && c1 == toGoC2 && r2 == toGoR1 && c2 == toGoC1) {
        return true;
    }

    return false;
}

bool OpeningItem::IsMoveValid( int ind )
{
    assert(0 <= ind && ind < OpeningItem::MOVES);
    return BITMASK_MOVE_IS_VALID(move[ind]);
}

int OpeningItem::GetMovesRow( int ind )
{
    assert(0 <= ind && ind < OpeningItem::MOVES);
    return BITMASK_MOVE_GET_ROW(move[ind]);
}

int OpeningItem::GetMovesCol( int ind )
{
    assert(0 <= ind && ind < OpeningItem::MOVES);
    return BITMASK_MOVE_GET_COL(move[ind]);
}

bool OpeningItem::GetMovesIsBlack( int ind )
{
    assert(0 <= ind && ind < OpeningItem::MOVES);
    return BITMASK_MOVE_GET_IS_BLACK(move[ind]);
}

const char Opening::dataDescriptionFileName[] = "datas.txt";

bool Opening::GetDMove( Board board, bool isBlack, DMove *move )
{
    OpeningItem item;
    int moves = 0;

    for (int r=0; r<ROW_MAX; ++r) {
        for (int c=0; c<COL_MAX; ++c) {

            if (board[r][c] != CELL_TYPE_EMPTY && board[r][c] != CELL_TYPE_NOT_A_CELL) {

                if (moves >= OpeningItem::MOVES) {
                    return false;
                }

                item.SetMove(moves, r, c, board[r][c] == CELL_TYPE_BLACK);

                ++moves;
            }
        }
    }

    item.SortMoves();
    item.toGoIsBlack = isBlack;

    std::vector <int> randInd;

    static const int MOD = 1007;

    for (size_t i=0; i<table.size(); ++i) {

        if (item.IsAllMovesEqual(table[i])/* && !!isBlack == !!table[i].toGoIsBlack*/) {

            float rate = (float)(table[i].wins + 1.0) / (table[i].losts + table[i].wins + 1.0);

            int chances = 2;
            while (chances-- > 0) {
                if ((float)(rand() % MOD) <= rate * MOD) {
                    randInd.push_back(i);
                }
            }
        }
    }

    if (randInd.size() == 0 || !move) {
        return false;
    }

    item = table[randInd[rand() % randInd.size()]];
    *move = DMove(item.toGoR1, item.toGoC1, item.toGoR2, item.toGoC2, item.toGoIsBlack);

    return true;
}

void Opening::BuildTable()
{
    FILE *dataDescriptionFile = fopen(dataDescriptionFileName, "r");
    if (dataDescriptionFile == NULL) {
        printf("cannot open %s\n", dataDescriptionFileName);
        return;
    }

    char dataFileName[1024];
    while (fgets(dataFileName, sizeof(dataFileName), dataDescriptionFile)) {
        if (SaverGame::FileNotExist(dataFileName)) {
            printf("%s in %s not read.\n", dataFileName, dataDescriptionFile);
            continue;
        }

        int gamesCount = SaverGame::GetGamesCount(dataFileName);
        for (int i=0; i<gamesCount; ++i) {
            SaverGame game;
            game.LoadFromDisk(dataFileName, i);

            if (game.IsDraw()) {
                continue;
            }

            debug_printf("game %d\n", i);

            OpeningItem item;
            for (int m=0; m<game.GetMovesCount() && m<OpeningItem::MOVES; ++m) {

                SaverMove saverMove = game.GetMove(m);
                item.SetMove(m, saverMove.GetRow(), saverMove.GetCol(), saverMove.IsBlack());

                bool playerJustSwitched = ((m + 2) % 2 == 0);
                if (playerJustSwitched) {

                    bool isBlacksTurn = (m + 1) % 4 < 2;

                    debug_printf("for all %d table items\n", table.size());
                    OpeningItem movesSotredItem = item;
                    movesSotredItem.SortMoves();

                    bool updated = false;

                    for (int k=0; k<table.size(); ++k) {

                        bool sameMovesToGo = (m + 2 < game.GetMovesCount());

                        SaverMove next1 = game.GetMove(m + 1);
                        SaverMove next2 = game.GetMove(m + 2);

                        sameMovesToGo = (sameMovesToGo &&
                            table[k].IsToGoEqual(next1.GetRow(), next1.GetCol(), next2.GetRow(), next2.GetCol(), next1.IsBlack()));

                        if (sameMovesToGo && table[k].IsAllMovesEqual(movesSotredItem)) {

                            if (!!game.IsBlackWinner() == !!isBlacksTurn) {
                                debug_printf("++table[%d].wins;\n", k);
                                ++table[k].wins;
                            } else if (! game.IsDraw()) {
                                debug_printf("++table[%d].losts;\n", k);
                                ++table[k].losts;
                            }

                            updated = true;
                            break;
                        }
                    } // for all table items

                    if (!updated && m + 2 < game.GetMovesCount()) {
                        if (isBlacksTurn == game.IsBlackWinner()) {
                            movesSotredItem.wins = 1;
                            movesSotredItem.losts = 0;
                        } else {
                            movesSotredItem.wins = 0;
                            movesSotredItem.losts = 1;
                        }

                        SaverMove next1 = game.GetMove(m + 1);
                        SaverMove next2 = game.GetMove(m + 2);

                        movesSotredItem.toGoR1 = next1.GetRow();
                        movesSotredItem.toGoC1 = next1.GetCol();
                        movesSotredItem.toGoR2 = next2.GetRow();
                        movesSotredItem.toGoC2 = next2.GetCol();
                        movesSotredItem.toGoIsBlack = next1.IsBlack();

                        debug_printf("table.push_back()\n");
                        table.push_back(movesSotredItem);

                        if (table.size() % 1000 == 0) {
                            printf("table.size() = %d\n", table.size());
                        }
                    }
                } // if m != 0 && playerJustSwitched
            } // for a SaverGame's first 'MOVES' moves
        }
    }

    fclose(dataDescriptionFile);
    dataDescriptionFile = NULL;
}

void Opening::BoardVerticalReverse( Board boardIn, Board boardOut )
{
    for (int r=0; r<ROW_MAX; ++r) {
        for (int c=0; c<COL_MAX; ++c) {
            boardOut[r][c] = boardIn[ROW_MAX - 1 - r][c];
        }
    }
}

void Opening::BoardRotateClockwise90( Board boardIn, Board boardOut )
{
    for (int r=0; r<ROW_MAX; ++r) {
        for (int c=0; c<COL_MAX; ++c) {
            boardOut[r][c] = boardIn[c][ROW_MAX - 1 - r];
        }
    }
}


