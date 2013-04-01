#include "RandTable.h"
#include "defines.h"

RandTable::RandTable() {
    srand((unsigned int)time(NULL));
    //srand(0);
    FOR_EACH_CELL(r, c) {
        FOR_EACH(i, _RAND_TABLES) {
            _rand32[i][r][c] = (unsigned int)rand() ^ ((unsigned int)rand() << 15) ^ ((unsigned int)rand() << 30);
            _rand64[i][r][c] = ((unsigned __int64)rand())
                ^ ((unsigned __int64)rand() << 15)
                ^ ((unsigned __int64)rand() << 30)
                ^ ((unsigned __int64)rand() << 45)
                ^ ((unsigned __int64)rand() << 60);
        }
    }
}

RandTable randTable;