#pragma once

#include <furi.h>

class RenderBuffer;

enum CardValue {
    NONE = -1,
    TWO = 0,        //1
    THREE = 1,      //2
    FOUR = 2,       //3
    FIVE = 3,       //4
    SIX = 4,        //5
    SEVEN = 5,      //6
    EIGHT = 6,      //7
    NINE = 7,       //8
    TEN = 8,        //9
    JACK = 9,       //10
    QUEEN = 10,     //11
    KING = 11,      //12
    ACE = 12,       //13
};

struct Card {
    uint8_t suit = 0;
    CardValue value = NONE;
    bool exposed = false;

    Card(uint8_t s, uint8_t v) : suit(s), value((CardValue) v) {}

    void Render(int8_t x, int8_t y, bool selected, RenderBuffer *buffer, uint8_t size_limit = 22);

    static void RenderEmptyCard(uint8_t x, uint8_t y, RenderBuffer *buffer);

    static void RenderBack(uint8_t x, uint8_t y, bool selected, RenderBuffer *buffer, uint8_t size_limit= 22);

    static void TryRender(Card *c, uint8_t x, uint8_t y, bool selected, RenderBuffer *buffer, uint8_t size_limit= 22);


    bool CanPlaceFoundation(Card *where);

    bool CanPlaceColumn(Card *where);

    void print();
};