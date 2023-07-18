#include "RenderBuffer.h"
#include "Sprite.h"
#include "Helpers.h"
#include "Vector.h"
#include "cmath"

RenderBuffer::RenderBuffer(uint8_t w, uint8_t h) : Buffer(w, h) {
    FURI_LOG_D("App", "New renderBuffer");
}

void RenderBuffer::reset() {
    int size = width() * (height() / 8);
    for (int i = 0; i < size; i++) {
        data[i] = 0;
    }
}

void RenderBuffer::render(Canvas *const canvas) {
//    canvas_clear(canvas);
    for (uint8_t x = 0; x < width(); x++) {
        for (uint8_t y = 0; y < height(); y++) {
            if (test_pixel(x, y))
                canvas_draw_dot(canvas, x, y);
        }
    }
//    canvas_commit(canvas);
}

void RenderBuffer::draw_line(int x0, int y0, int x1, int y1, PixelColor draw_mode) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;

    while (true) {
        if (test_coordinate(x0, y0)) {
            set_pixel(x0, y0, draw_mode);
        }
        if (x0 == x1 && y0 == y1) break;
        int e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y0 += sy;
        }
    }
}

void RenderBuffer::draw_circle(int x, int y, int r, PixelColor color) {
    int16_t a = r;
    int16_t b = 0;
    int16_t decision = 1 - a;

    while (b <= a) {
        set_pixel_with_check(a + x, b + y, color);
        set_pixel_with_check(b + x, a + y, color);
        set_pixel_with_check(-a + x, b + y, color);
        set_pixel_with_check(-b + x, a + y, color);
        set_pixel_with_check(-a + x, -b + y, color);
        set_pixel_with_check(-b + x, -a + y, color);
        set_pixel_with_check(a + x, -b + y, color);
        set_pixel_with_check(b + x, -a + y, color);

        b++;
        if (decision <= 0) {
            decision += 2 * b + 1;
        } else {
            a--;
            decision += 2 * (b - a) + 1;
        }
    }
}

void RenderBuffer::draw(Sprite *const sprite, Vector position, float rotation) {
    draw(sprite, position, sprite->width(), sprite->height(), rotation);
}


void RenderBuffer::draw(Sprite *const sprite, Vector position, uint8_t x_cap, uint8_t y_cap, float rotation) {
    switch (sprite->draw_mode) {
        default:
        case BlackOnly:
            draw_sprite(sprite, true, Black, position, x_cap, y_cap, rotation);
            break;
        case WhiteOnly:
            draw_sprite(sprite, false, White, position, x_cap, y_cap, rotation);
            break;
        case WhiteAsBlack:
            draw_sprite(sprite, false, Black, position, x_cap, y_cap, rotation);
            break;
        case BlackAsWhite:
            draw_sprite(sprite, true, White, position, x_cap, y_cap, rotation);
            break;
        case WhiteAsInverted:
            draw_sprite(sprite, false, Flip, position, x_cap, y_cap, rotation);
            break;
        case BlackAsInverted:
            draw_sprite(sprite, true, Flip, position, x_cap, y_cap, rotation);
            break;
    }
}

//TODO: proper scaling
void
RenderBuffer::draw_sprite(Sprite *const sprite, bool is_black, PixelColor draw_color, const Vector &position,
                          uint8_t x_cap, uint8_t y_cap, float rotation) {
    Vector anchor = sprite->get_offset();
    float cosTheta = cos(rotation/* + M_PI_2*/);
    float sinTheta = sin(rotation/* + M_PI_2*/);
    float transformedX, transformedY, rotatedX, rotatedY;
    int max_w = fmin(sprite->width(), x_cap);
    int max_h = fmin(sprite->height(), y_cap);
    bool isOn;
    int16_t finalX, finalY;
    for (int y = 0; y < max_h; y++) {
        for (int x = 0; x < max_w; x++) {
            transformedX = (x - anchor.x);
            transformedY = (y - anchor.y);
            rotatedX = transformedX * cosTheta - transformedY * sinTheta;
            rotatedY = transformedX * sinTheta + transformedY * cosTheta;

            finalX = (int16_t) floor(rotatedX + position.x);
            finalY = (int16_t) floor(rotatedY + position.y);
            if (test_coordinate(finalX, finalY)) {
                isOn = sprite->test_pixel(x, y) == is_black;
                if (isOn)
                    set_pixel(finalX, finalY, draw_color);
            }

        }
    }
}

RenderBuffer::~RenderBuffer() {
    FURI_LOG_D("App", "RenderBuffer end");
}

void RenderBuffer::draw_rbox(int16_t x0, int16_t y0, int16_t x1, int16_t y1, PixelColor draw_mode) {
    for (int16_t x = x0; x < x1; x++) {
        for (int16_t y = y0; y < y1; y++) {
            if (((x == x0 || x == x1 - 1) && (y == y0 || y == y1 - 1)) || !test_coordinate(x, y)) continue;
            set_pixel(x, y, draw_mode);
        }
    }
}

void RenderBuffer::draw_rbox_frame(int16_t x0, int16_t y0, int16_t x1, int16_t y1, PixelColor draw_mode) {
    draw_line(x0 + 1, y0, x1 - 1, y0, draw_mode);
    draw_line(x0 + 1, y1, x1 - 1, y1, draw_mode);

    draw_line(x0, y0 + 1, x0, y1 - 1, draw_mode);
    draw_line(x1, y0 + 1, x1, y1 - 1, draw_mode);
}

void RenderBuffer::draw_box(int16_t x0, int16_t y0, int16_t x1, int16_t y1, PixelColor draw_mode) {
    for (int16_t x = x0; x < x1; x++) {
        for (int16_t y = y0; y < y1; y++) {
            set_pixel(x, y, draw_mode);
        }
    }
}