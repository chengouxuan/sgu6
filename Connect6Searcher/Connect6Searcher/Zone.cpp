#include "Zone.h"

void Zone::Clear() {
    while(Size() > 0) {
        Remove(GetPoint(0));
    }
}

void Zone::AddSegmentTable(SegmentTable::Table &tab) {
    FOR_EACH(i, tab.Size()) {
        AddSegment(tab.GetItem(i));
    }
}
