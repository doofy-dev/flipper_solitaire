#include "Card.h"
#include "RenderBuffer.h"
#include "../assets.h"

#define CARD_HEIGHT 23
#define CARD_HALF_HEIGHT 11
#define CARD_WIDTH 17
#define CARD_HALF_WIDTH 8

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

void Card::Render(uint8_t x, uint8_t y, bool selected, RenderBuffer *buffer) {

    if (exposed) {
        buffer->draw_rbox(x, y, x + 16, y + 22, White);
        buffer->draw_rbox_frame(x, y, x + 16, y + 22, Black);
//        buffer->draw_line(x+15, y, x+15, y+22, White);

            buffer->draw(&(letters[value]), (Vector) {(float) x + 5, (float) y + 6}, 0);
           buffer->draw(&(suits[suit]), (Vector) {(float) x + 12, (float) y + 6}, 0);

          buffer->draw(&(letters[value]), (Vector) {(float) x + 12, (float) y + 17}, M_PI);
           buffer->draw(&(suits[suit]), (Vector) {(float) x + 5, (float) y + 17}, M_PI);
        if (selected) {
            buffer->draw_rbox(x, y, x + 17, y + 23, Flip);
        }
    } else {
        RenderBack(x, y, selected, buffer);
//        buffer->draw(&backSide, (Vector) {(float) x + 8, (float) y + 11}, 0);
    }

}

void Card::RenderEmptyCard(uint8_t x, uint8_t y, RenderBuffer *buffer) {
    buffer->draw_rbox(x, y, x + 17, y + 23, Flip);
    buffer->draw_rbox_frame(x + 2, y + 2, x + 14, y + 20, Flip);
}

void Card::RenderBack(uint8_t x, uint8_t y, bool selected, RenderBuffer *buffer) {
    buffer->draw_rbox(x + 1, y + 1, x + 16, y + 22, White);
    buffer->draw_rbox_frame(x, y, x + 16, y + 22, Black);
    buffer->draw(&backSide, (Vector) {(float) x + 9, (float) y + 12}, 0);
    if (selected) {
        buffer->draw_rbox(x, y, x + 17, y + 23, Flip);
    }
}
