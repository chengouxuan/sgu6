#include "evaluator.h"
#include <cstdio>
#include <cassert>
#include "utilities.h"
#include "defines.h"
#include "movesearcher.h"
#include "connect6.h"
//#include "oldevaluator.h"

Evaluator::Evaluator() {
    FILE *file = fopen("Shapes.txt", "r");
    if(file == NULL) {
        printf("cannot open Shapes.txt\n");
        exit(1);
    }
    char pat[1024], line[1024];
    int scr;
    memset(_score, 0, sizeof(_score));
    printf("\nscore table:\n");
    int i = 0;
    while(fgets(line, sizeof(line) - 1, file) != NULL) {
        if(sscanf(line, "%s%d", pat, &scr) == 2 && *pat != '#') {
            size_t len = strlen(pat);
            printf("%2d: score[ %9s ] = %9d\n", i++, pat, scr);
            assert(_SHAPE_LEN_MIN <= len && len <= _SHAPE_LEN_MAX);
            const char *p = pat - 1;
            int mask = 0;
            while(*++p) {
                mask <<= 1;
                if(*p == '1') {
                    mask |= 1;
                }
            }
            assert(_score[len][mask] == 0);
            _score[len][mask] = scr;
            mask = 0;
            while(--p >= pat) {
                mask <<= 1;
                if(*p == '1') {
                    mask |= 1;
                }
            }
            assert(_score[len][mask] == 0 || _score[len][mask] == scr);
            _score[len][mask] += scr;
        }
    }
    fclose(file);
}

int Evaluator::Evaluate(bool isBlacksTurn, int depth)
{
    int scoreFixing = 0;
    if (depth != 0 && depth % 2 == 1) {
        scoreFixing = -351861;
    } else if (depth != 0 && depth % 2 == 0) {
        scoreFixing = 351861;
    }

    int sign = (isBlacksTurn ? 1 : -1);

    int score;
    if(_transTable.LookUp(score)) {
        ++_transTable._hits;
        return score * sign + scoreFixing;
    }

    ++_evaluations;

    score = SimpleEval();

    int sz = 0;
    CellType line[_LINE_LEN_MAX];

    FOR_EACH_ROW(row) {
        FOR_EACH_COL(col) {
            line[col] = ::GetCell(row, col);
        }
        score += LineScore(line, COL_MAX);
    }

    FOR_EACH_COL(col) {
        FOR_EACH_ROW(row) {
            line[row] = ::GetCell(row, col);
        }
        score += LineScore(line, ROW_MAX);
    }

    // 对角线
    FOR_EACH_ROW(row) {
        sz = 0;
        int i = 0;
        while(++i, IsInsideBoard(row + i, 0 + i)) {
            line[sz++] = ::GetCell(row + i, 0 + i);
        }
        score += LineScore(line, sz);
    }

    FOR_EACH_COL(col) {
        sz = 0;
        if(col == 0) {
            continue;
        }
        int i = 0;
        while(++i, IsInsideBoard(0 + i, col + i)) {
            line[sz++] = ::GetCell(0 + i, col + i);
        }
        score += LineScore(line, sz);
    }

    // 反对角线
    FOR_EACH_ROW(row) {
        sz = 0;
        int i = 0;
        while(++i, IsInsideBoard(row + i, 18 - i)) {
            line[sz++] = ::GetCell(row + i, 18 - i);
        }
        score += LineScore(line, sz);
    }

    FOR_EACH_COL(col) {
        sz = 0;
        if(col == 18) {
            continue;
        }
        int i = 0;
        while(++i, IsInsideBoard(0 + i, col - i)) {
            line[sz++] = ::GetCell(0 + i, col - i);
        }
        score += LineScore(line, sz);
    }

#ifdef _DEBUG
#endif
    _transTable.Enter(score);
    return score * sign + scoreFixing;
}

Evaluator evaluator;

int Evaluator::LineScore(CellType line[_LINE_LEN_MAX], int len) {
    int scr = 0;
#ifdef _DEBUG
#endif
    for(int s=_SHAPE_LEN_MIN; s<=_SHAPE_LEN_MAX; ++s) {
        unsigned int maskB = 0, maskW = 0;
        int lastB = -1, lastW = -1;
        FOR_EACH(i, len) {
            maskB <<= 1;
            maskW <<= 1;
            if(line[i] == CELL_TYPE_BLACK) {
                maskB |= 1;
                lastB = i;
            } else if(line[i] == CELL_TYPE_WHITE) {
                maskW |= 1;
                lastW = i;
            }
            if(s <= i + 1) {
                if(i - lastW >= s) {
                    scr += _score[s][maskB & ~(~0 << s)];
                }
                if(i - lastB >= s) {
                    scr -= _score[s][maskW & ~(~0 << s)];
                }
            }
        }
    }
    return scr;
}

int Evaluator::SimpleEval() {
    int ret = 0;
    FOR_EACH(i, 6) {
        ret += ::GetSegmentTable(true, i).Size() * _segmentScore[i]/* * i*/;
        ret -= ::GetSegmentTable(false, i).Size() * _segmentScore[i]/* * i*/;
    }
    return ret;
}

const int Evaluator::_segmentScore[6] = {2, 139,  10321,  65651, 100621, 741071};