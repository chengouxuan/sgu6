#include "DPointTable.h"

void DPointTable::Clear() {
    while(Size() > 0) {
        Remove(GetDPoint(0));
    }
}