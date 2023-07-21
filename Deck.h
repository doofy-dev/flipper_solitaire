#pragma once

#include <furi_hal_resources.h>
#include "utils/Card.h"
#include "utils/List.h"
#include "TableauColumn.h"

class Deck {
    List<Card> stock_pile;
    List<Card> waste_pile;
    uint8_t deck_count;
public:
    explicit Deck(uint8_t count);
    void Generate();
    void Render(RenderBuffer *buffer, bool stockpileSelect, bool wasteSelect);
    void Cycle();
    Card* GetLastWaste();
    void AddToWaste(Card* c);
    Card* Extract();
    void Click(InputKey key, bool isDeck, TableauColumn *hand);
};
