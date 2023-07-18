#pragma once

#include "Buffer.h"
#include <gui/gui.h>

struct Vector;
class Sprite;
class RenderBuffer : public Buffer {
    void draw_sprite(Sprite *const sprite, bool is_black,
                     PixelColor draw_color, const Vector& position, uint8_t x_cap, uint8_t y_cap, float rotation);

public:
    explicit RenderBuffer(uint8_t w, uint8_t h);

    virtual ~RenderBuffer();

    void render(Canvas *const canvas);

    void reset();

    void draw_line(int x0, int y0, int x1, int y1, PixelColor draw_mode);
    void draw_rbox(int16_t x0, int16_t y0, int16_t x1, int16_t y1, PixelColor draw_mode);
    void draw_box(int16_t x0, int16_t y0, int16_t x1, int16_t y1, PixelColor draw_mode);
    void draw_rbox_frame(int16_t x0, int16_t y0, int16_t x1, int16_t y1, PixelColor draw_mode);

    void draw_circle(int x, int y, int r, PixelColor draw_mode);

    void draw(Sprite *const sprite, Vector position, uint8_t x_cap, uint8_t y_cap, float rotation);
    void draw(Sprite *const sprite, Vector position, float rotation);
};
