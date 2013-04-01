#pragma once

#include <vector>

template <typename Item, const int _SIZE, typename ItemEqual>
class Array {
private:
    std::vector <Item> _vec;

public:
    Array() {
        _vec.reserve(_SIZE);
    }
    int Size() {
        return (int)_vec.size();
    }
    void Clear() {
        _vec.erase(_vec.begin(), _vec.end());
    }
    void PushBack(const Item &item) {
        _vec.push_back(item);
    }
    void PopBack() {
        _vec.pop_back();
    }
    Item &ItemRef(int ind) {
        return _vec[ind];
    }
    bool Find(const Item &item);

private:
    // copying is time-consuming, so forbidden.
    Array(const Array &);
    Array &operator = (const Array &);
};

template <typename Item, const int _SIZE, typename ItemEqual>
bool Array<Item, _SIZE, ItemEqual>::Find(const Item &item) {
    FOR_EACH(i, Size()) {
        if(ItemEqual()(item, ItemRef(i))) {
            return true;
        }
    }
    return false;
}
