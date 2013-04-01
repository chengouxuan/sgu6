#pragma once

#include "connect6.h"
#include <cassert>

template <typename Item, const int SIZE>
class TranspositionTable {
    Item _table[SIZE];
    Item &ItemRef(unsigned int ind) {
        assert(0 <= ind && ind < SIZE);
        return _table[ind];
    }
    unsigned int Ind(unsigned int hash) {
        return hash % SIZE;
    }
public:
    int _hits;

    TranspositionTable() {}
    void Enter(const Item &item, unsigned int hash) {
        _table[Ind(hash)] = item;
    }
    bool LookUp(unsigned __int64 checkSum, unsigned int hash, Item &item) {
        item = ItemRef(Ind(hash));
        return item._checkSum == checkSum;
    }

private:
    TranspositionTable (const TranspositionTable &);
};
