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
}

void OpeningFinder::Initilalize() {
}

OpeningFinder openingFinder;

