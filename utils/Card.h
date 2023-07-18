#pragma once

#include <furi.h>

class RenderBuffer;

struct Card {
    uint8_t suit = 0;
    uint8_t value = 0;
    bool exposed = false;

    Card(uint8_t s, uint8_t v) : suit(s), value(v) {}

    void Render(uint8_t x, uint8_t y, bool selected, RenderBuffer *buffer, uint8_t size_limit);
    static void RenderEmptyCard(uint8_t x, uint8_t y, RenderBuffer *buffer);
    static void RenderBack(uint8_t x, uint8_t y, bool selected, RenderBuffer *buffer, uint8_t size_limit);
};