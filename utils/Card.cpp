#include "Card.h"
#include "RenderBuffer.h"
#include "../assets.h"

static Sprite letters[] = {
        Sprite(sprite_2, BlackOnly),
        Sprite(sprite_3, BlackOnly),
        Sprite(sprite_4, BlackOnly),
        Sprite(sprite_5, BlackOnly),
        Sprite(sprite_6, BlackOnly),
        Sprite(sprite_7, BlackOnly),
        Sprite(sprite_8, BlackOnly),
        Sprite(sprite_9, BlackOnly),
        Sprite(sprite_10, BlackOnly),
        Sprite(sprite_J, BlackOnly),
        Sprite(sprite_Q, BlackOnly),
        Sprite(sprite_K, BlackOnly),
        Sprite(sprite_A, BlackOnly),
};

static Sprite suits[] = {
        Sprite(sprite_hearths, BlackOnly),
        Sprite(sprite_diamonds, BlackOnly),
        Sprite(sprite_spades, BlackOnly),
        Sprite(sprite_clubs, BlackOnly)
};

static Sprite backSide = Sprite(sprite_pattern_big, BlackOnly);

void Card::Render(uint8_t x, uint8_t y, bool selected, RenderBuffer *buffer, uint8_t size_limit) {
    uint8_t height = y + fmin(size_limit, 22);
    if (exposed) {
        buffer->draw_rbox(x, y, x + 16, height, White);
        buffer->draw_rbox_frame(x, y, x + 16, height, Black);
        buffer->draw(&(letters[value]), (Vector) {(float) x + 5, (float) y + 6}, 0);
        buffer->draw(&(suits[suit]), (Vector) {(float) x + 12, (float) y + 6}, 0);

        if (size_limit > 8) {
            buffer->draw(&(letters[value]), (Vector) {(float) x + 12, (float) y + 17}, M_PI);
            buffer->draw(&(suits[suit]), (Vector) {(float) x + 5, (float) y + 17}, M_PI);
        }
        if (selected) {
            buffer->draw_box(x + 1, y + 1, x + 16, height, Flip);
        }
    } else {
        RenderBack(x, y, selected, buffer, size_limit);
    }
}

void Card::RenderEmptyCard(uint8_t x, uint8_t y, RenderBuffer *buffer) {
    buffer->draw_rbox(x, y, x + 17, y + 23, Black);
    buffer->draw_rbox_frame(x + 2, y + 2, x + 14, y + 20, White);
}

void Card::RenderBack(uint8_t x, uint8_t y, bool selected, RenderBuffer *buffer, uint8_t size_limit) {
    uint8_t height = y + fmin(size_limit, 22);

    buffer->draw_box(x + 1, y + 1, x + 16, height, White);
    buffer->draw_rbox_frame(x, y, x + 16, height, Black);
    buffer->draw(&backSide, (Vector) {(float) x + 9, (float) y + 12}, 15, fmin(size_limit, 22), 0);
    if (selected) {
        buffer->draw_box(x + 1, y + 1, x + 16, height, Flip);
    }
}

bool Card::CanPlace(Card *a, Card *b) {
    if (a == nullptr) {
        return b->value == 12;
    }
    return a->suit == b->suit && ((b->value + 1) % 13 == (a->value + 2) % 13);
}
