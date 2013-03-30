#include "SegmentTable.h"
#include "ComplexBoard.h"

SegmentTable segmentTable;

CellType SegmentTable::Item::GetCell(int i) {
    assert(0 <= i && i < 6);
    assert(this->IsInsideBoard());
    return complexBoard.GetCell(_row + i * dr[_dir], _col + i * dc[_dir]);
}
