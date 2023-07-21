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
    void Clear();
    void AddRange(List<Card> *hand);
    void Merge(TableauColumn *other);
    void Render(uint8_t x, uint8_t y, bool selected, uint8_t selection, RenderBuffer *buffer);
    Card* TopCard();
    Card* LastCard();

    int8_t FirstNonFlipped();
    uint8_t Count();
    Card* Pop();
    void Reveal();
    bool CanPlace(TableauColumn *other);
};
