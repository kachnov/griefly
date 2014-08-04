#pragma once

#include "Item.h"

class FloorTile: public Item
{
public:
    DECLARE_SAVED(FloorTile, Item);
    DECLARE_GET_TYPE_ITEM(FloorTile);
    FloorTile();
};
ADD_TO_TYPELIST(FloorTile);