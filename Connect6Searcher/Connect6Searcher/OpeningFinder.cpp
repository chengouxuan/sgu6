#include "OpeningFinder.h"
#include "defines.h"
#include "RandTable.h"

bool OpeningFinder::Find(Board board, bool isBlack) {
    Board b;
    std::fill(b[0], b[ROW_MAX], CELL_TYPE_EMPTY);
    int midR = ROW_MAX / 2;
    int midC = COL_MAX / 2;
    b[ROW_MAX / 2][COL_MAX / 2] = CELL_TYPE_BLACK;
    if(memcmp(b, board, sizeof(Board)) == 0) {
        _dMove = DMove(midR, midC - 2, midR - 1, midC - 1, isBlack);
        return true;
    }
    int row = -1, col = -1;
    FOR_EACH_CELL(r, c) {
        if(board[r][c] == CELL_TYPE_BLACK) {
            if(! (row == -1 && col == -1)) {
                return false;
            }
            row = r;
            col = c;
        }
        if(board[r][c] == CELL_TYPE_WHITE) {
            return false;
        }
    }
    if(row == midR) {
        int sign = (midC - col) / std::abs(midC - col);
        _dMove = DMove(midR, col + 1 * sign, midR, col + 2 * sign, isBlack);
    } else if(col == midC) {
        int sign = (midR - row) / std::abs(midR - row);
        _dMove = DMove(row + 1 * sign, midC, row + 2 * sign, midC, isBlack);
    } else {
        int signR = (midR - row) / std::abs(midR - row);
        int signC = (midC - col) / std::abs(midC - col);
        _dMove = DMove(row + 1 * signR, col + 1 * signC, row + 2 * signR, col + 2 * signC, isBlack);
    }
    return true;
    //if(_table.Find(board, _dMove)) {
    //    assert(_dMove._isBlack == isBlack);
    //    return true;
    //}
    return false;
}

void OpeningFinder::Initilalize() {
    //std::vector <std::vector <CSaverMove> > move;
    //Opening::Load("opening.dat", move);
    //FOR_EACH(i, (int)move.size()) {
    //    Board board;
    //    std::fill(board[0], board[ROW_MAX], CELL_TYPE_EMPTY);
    //    FOR_EACH(k, (int)move[i].size() - 2) {
    //        CSaverMove m = move[i][k];
    //        board[m.GetRow()][m.GetCol()] = (m.IsBlack() ? CELL_TYPE_BLACK : CELL_TYPE_WHITE);
    //    }
    //    size_t sz = move[i].size();
    //    CSaverMove m = move[i][sz - 2];
    //    CSaverMove n = move[i][sz - 1];
    //    _table.Insert(board, DMove(m.GetRow(), m.GetCol(), n.GetRow(), n.GetCol(), m.IsBlack()));
    //}
}

OpeningFinder openingFinder;

//OpeningFinder::Table::Key::Key(Board board) {
//    _checkSum = 0;
//    _data.clear();
//    FOR_EACH_CELL(r, c) {
//        _checkSum ^= ::randTable.Rand64(r, c, board[r][c]);
//        if(board[r][c] != CELL_TYPE_EMPTY) {
//            _data.push_back(CSaverMove(r, c, board[r][c] == CELL_TYPE_BLACK).Data());
//        }
//    }
//    std::sort(_data.begin(), _data.end());
//}
//
//unsigned int OpeningFinder::Table::Hash(Board board) {
//    unsigned int hash = 0;
//    FOR_EACH_CELL(r, c) {
//        hash ^= ::randTable.Rand32(r, c, board[r][c]);
//    }
//    return hash;
//}
//
//bool OpeningFinder::Table::Find(Board board, DMove &dMove) {
//    unsigned int ind = Hash(board) % _SIZE;
//    Key key(board);
//    FOR_EACH(i, (int)_bucket[ind].size()) {
//        if(_bucket[ind][i]._key.IsMatch(key)) {
//            dMove = _bucket[ind][i]._dMove;
//            return true;
//        }
//    }
//    return false;
//}
