#include "EventDirect.h"

void EventDirect::Get(int index, int** start, int** end, unsigned int* color)
{
#ifdef DEBUG
    if (index >= static_cast<int>(_EventItems.size()))
    {
        return;
    }
    auto eventItem = _EventItems[EventNames_[index]];
    if (color)
        *color = eventItem->color; // 全員に同じ色、タイプに基づいて色を返す
    if (start)
        *start = &eventItem->_Start;
    if (end)
        *end = &eventItem->_End;
#endif
}
