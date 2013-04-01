#pragma once

#include "connect6.h"
#include <Windows.h>
#include <vector>

class OpeningFinder {
public:
    void Initilalize();

    class Table {
    public:
        static const int _SIZE = 777391;

        Table(): _bucket(_SIZE) {}

        class Key {
        public:
            std::vector <WORD> _data;
            unsigned __int64 _checkSum;
            Key(): _data(0), _checkSum(0) {}
            Key(Board board);
            bool IsMatch(const Key &item) {
                if(_checkSum != item._checkSum) {
                    return false;
                } else {
                    return _data.size() == item._data.size() &&
                        std::equal(_data.begin(), _data.end(), item._data.begin());
                }
            }
        };

        class Item {
        public:
            Key _key;
            DMove _dMove;
            Item() {}
            Item(const Key &key, const DMove &dMove): _key(key), _dMove(dMove) {}
        };

        std::vector <std::vector <Item> > _bucket;

        static unsigned int Hash(Board board);

        bool Find(Board board, DMove &dMove);

        bool Insert(Board board, const DMove &dMove) {
            DMove dm;
            if(Find(board, dm)) {
                return false;
            }
            unsigned int ind = Hash(board) % _SIZE;
            Item item(Key(board), dMove);
            _bucket[ind].push_back(item);
            return true;
        }
    } _table;

    DMove _dMove;
    bool Find(Board board, bool isBlack);
};

extern OpeningFinder openingFinder;