#pragma once


#include "utils/Card.h"
#include "utils/List.h"

class TableauColumn {
    List<Card> *cards;

public:
    TableauColumn();
    ~TableauColumn();
    void Reset();
    void AddCard(Card *c);
    void AddRange(List<Card> *hand);
    void Render(uint8_t x, uint8_t y, bool selected, uint8_t selection, RenderBuffer *buffer);

    int8_t FirstNonFlipped();
};
