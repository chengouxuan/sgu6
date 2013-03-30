#pragma once

#include <windows.h>
#include <cassert>

#define BLACK 0
#define WHITE 1 // 切勿随便交换WHITE和BLACK的01的值
#define DRAW 1
#define NOT_DRAW 0
#define POS_MAX (1 << 21)
#define MOVES_MAX (1 << 9)
#define GET_POS(x) ((x) >> 11)
#define GET_MOVES(x) (((x) & ~(~0 << 11)) >> 2)
#define GET_WINNER(x) (((x) >> 1) & 1)
#define GET_DRAW(x) ((x) & 1)
#define SET_POS(x, v) (((x) ^= (GET_POS(x) ^ (v)) << 11), 0)
#define SET_MOVES(x, v) (((x) ^= (GET_MOVES(x) ^ (v)) << 2), 0)
#define SET_BLACK_WINNER(x) (((x) &= ~2), 0) // 修改BLACK WHITE宏时必须同时修改此行
#define SET_WHITE_WINNER(x) (((x) |= 2), 0) // 修改BLACK WHITE宏时必须同时修改此行
#define SET_WINNER(x, v) (((v) == BLACK ? SET_BLACK_WINNER(x) : SET_WHITE_WINNER(x)), 0)
#define SET_DRAW(x) (((x) |= 1), 0)
#define UNSET_DRAW(x) (((x) &= ~1), 0)

class SaverSummary
{
private:
    DWORD _data;

public:
    SaverSummary(): _data(0) {}

    SaverSummary(int pos, int moves, bool isDraw, bool isBlackWinner)
        : _data(0)
    {
        SetPos(pos);
        SetMoves(moves);

        if(isDraw) {
            SetDraw();
            return;
        }

        isBlackWinner ? SetBlackWinner() : SetWhiteWinner();
    }

    DWORD &Data()
    {
        return _data;
    }

    int GetPos()
    {
        return GET_POS(_data);
    }

    int GetMoves() const
    {
        return GET_MOVES(_data);
    }
    bool IsDraw()
    {
        return GET_DRAW(_data) == DRAW;
    }

    bool IsBlackWinner()
    {
        return GET_WINNER(_data) == BLACK;
    }

    void SetPos(int pos)
    {
        assert(0 <= pos && pos < POS_MAX);
        SET_POS(_data, pos);
        assert(GET_POS(_data) == pos);
    }

    void SetMoves(int moves)
    {
        assert(0 <= moves && moves < MOVES_MAX);
        SET_MOVES(_data, moves);
        assert(GET_MOVES(_data) == moves);
    }

    void SetBlackWinner()
    {
        SET_BLACK_WINNER(_data);
        assert(GET_WINNER(_data) == BLACK);
    }

    void SetWhiteWinner()
    {
        SET_WHITE_WINNER(_data);
        assert(GET_WINNER(_data) == WHITE);
    }

    void SetWinner(int color)
    {
        assert(color == BLACK || color == WHITE);
        color == BLACK ? SetBlackWinner() : SetWhiteWinner();
        assert(color == GET_WINNER(_data));
    }

    void SetDraw()
    {
        SET_DRAW(_data);
        assert(IsDraw());
    }

    void UnsetDraw()
    {
        UNSET_DRAW(_data);
        assert(! IsDraw());
    }
};

#define ROW_MAX 19
#define COL_MAX 19
#define GET_COLOR(x) ((x) & 1)
#define GET_ROW(x) (((x) & ~(~0 << 6)) >> 1)
#define GET_COL(x) (((x) & ~(~0 << 11)) >> 6)
#define SET_COLOR_BLACK(x) (((x) &= 0), 0)
#define SET_COLOR_WHITE(x) (((x) |= 1), 0)
#define SET_COLOR(x, v) (((v) == BLACK ? SET_COLOR_BLACK(x) : SET_COLOR_WHITE(x)), 0)
#define SET_ROW(x, r) (((x) ^= (GET_ROW(x) ^ (r)) << 1), 0)
#define SET_COL(x, c) (((x) ^= (GET_COL(x) ^ (c)) << 6), 0)

class SaverMove
{
private:
    WORD _data;

public:
    SaverMove(): _data(0) {}

    SaverMove(int row, int col, bool isBlack)
    {
        isBlack ? SetColorBlack() : SetColorWhite();
        SetRow(row);
        SetCol(col);
    }

    const WORD &Data() const
    {
        return _data;
    }

    bool IsBlack()
    {
        return GET_COLOR(_data) == BLACK;
    }

    int GetRow()
    {
        return GET_ROW(_data);
    }

    int GetCol()
    {
        return GET_COL(_data);
    }

    void SetColorBlack()
    {
        SET_COLOR_BLACK(_data);
        assert(GET_COLOR(_data) == BLACK);
    }

    void SetColorWhite()
    {
        SET_COLOR_WHITE(_data);
        assert(GET_COLOR(_data) == WHITE);
    }

    void SetColor(int color)
    {
        assert(color == BLACK || color == WHITE);
        color == BLACK ? SetColorBlack() : SetColorWhite();
        assert((color == BLACK) == IsBlack());
    }

    void SetRow(int row)
    {
        SET_ROW(_data, row);
        assert(row == GET_ROW(_data));
    }

    void SetCol(int col)
    {
        SET_COL(_data, col);
        assert(col == GET_COL(_data));
    }
};

#include <vector>
#include <cstdio>

#define SUMMARIES_MAX (1 << 10)
#define HEADER_BYTES (8 + SUMMARIES_MAX * sizeof(SaverSummary) / sizeof(byte))
#define SUMMARY_BEGIN_POS 8
#define SUMMARY_POS(s) (SUMMARY_BEGIN_POS + (s) * sizeof(SaverSummary) / sizeof(byte))

#define GAMES_PER_FILE 100

class SaverGame
{
private:
    typedef std::vector <SaverMove> MoveArray;
    MoveArray _move;
    bool _isBlackWinner;
    bool _isDraw;

public:
    SaverGame(): _isBlackWinner(false), _isDraw(false) {}

    void AddMove(int row, int col, bool isBlack)
    {
        _move.push_back(SaverMove(row, col, isBlack));
    }

    void SetBlackWinner()
    {
        _isBlackWinner = true;
        _isDraw = false;
    }
    void SetWhiteWinner() {
        _isBlackWinner = false;
        _isDraw = false;
    }

    void SetWinner(bool isBlack)
    {
        isBlack ? SetBlackWinner() : SetWhiteWinner();
    }

    void SetDraw()
    {
        _isDraw = true;
    }
    void UnsetDraw() {
        _isDraw = false;
    }

    bool IsDraw()
    {
        return _isDraw;
    }

    bool IsBlackWinner()
    {
        return !IsDraw() && _isBlackWinner;
    }

    int GetMovesCount()
    {
        return (int)_move.size();
    }

    SaverMove GetMove(int i)
    {
        assert(0 <= i && i < (int)_move.size());
        return _move[i];
    }

    void SaveToDisk(const char *path);
    void LoadFromDisk(const char *path, int n);
    static int GetGamesCount(const char *path);

    static bool FileNotFull(const char *path)
    {
        return GetGamesCount(path) < GAMES_PER_FILE;
    }

    static bool FileNotExist(const char *path)
    {
        FILE *f = fopen(path, "rb");
        if (! f) {
            return true;
        }

        fclose(f);
        return false;
    }
};
