#include "Buffer.h"
#include "Sprite.h"

Buffer::Buffer(uint8_t w, uint8_t h) : _width(w), _height(h) {
    data = (uint8_t *) malloc(sizeof(uint8_t) * w * ceil(h / 8.0));
}

Buffer::Buffer(const SpriteData *i) {
    FURI_LOG_D("BUFFER","Buffer init");
    _width = i->width;
    _height = i->height;//ceil(i->height / 8.0);
    data = (i->data);
    remove_buffer = false;
}

Buffer::~Buffer() {
    FURI_LOG_D("BUFFER", "Buffer removed");

    if (data && remove_buffer)
        delete data;
}

bool Buffer::test_pixel(uint8_t x, uint8_t y) {
    return data[(y >> 3) * _width + x] & (1 << (y & 7));
}

void Buffer::copy_into(uint8_t *other) {
    int size = (int) (_width * ceil(_height / 8.0));
    for (int i = 0; i < size; i++) {
        other[i] = data[i];
    }
}

void Buffer::copy_from(uint8_t *other) {
    int size = (int) (_width * ceil(_height / 8.0));
    for (int i = 0; i < size; i++) {
        data[i] = other[i];
    }
}

void Buffer::clear() {
    int size = (int) (_width * ceil(_height / 8.0));
    for (int i = 0; i < size; i++) {
        data[i] = 0;
    }
}


bool Buffer::test_coordinate(int x, int y) const {
    return x >= 0 && y >= 0 && x < _width && y < _height;
}

void Buffer::set_pixel_with_check(int16_t x, int16_t y, PixelColor draw_mode) {
    if (test_pixel(x, y))
        set_pixel(x, y, draw_mode);
}

void Buffer::set_pixel(int16_t x, int16_t y, PixelColor draw_mode) {
    uint8_t bit = 1 << (y & 7);
    uint8_t *p = data + (y >> 3) * width() + x;

    switch (draw_mode) {
        case Black:
            *p |= bit;
            break;
        case White:
            *p &= ~bit;
            break;
        case Flip:
            *p ^= bit;
            break;
    }
}

void Buffer::swap(uint8_t *&buffer) {
    uint8_t *back = data;
    data = buffer;
    buffer = back;
}

