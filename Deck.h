#pragma once

#include "utils/Card.h"
#include "utils/List.h"

class Deck {
    List<Card> stock_pile;
    List<Card> waste_pile;
    uint8_t deck_count;
public:
    explicit Deck(uint8_t count);
    void Generate();
    void Render(RenderBuffer *buffer);
    void Cycle();
    Card* GetLastWaste();
    Card* Extract();
    void AddToWaste(Card* c);
};
