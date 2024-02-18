#pragma once
#include <furi.h>
#include <gui/icon.h>

enum PixelColor {
    Black, //or
    White, //
    Flip   //not
};
enum DrawMode {
    WhiteOnly,
    BlackOnly,
    WhiteAsBlack,
    BlackAsWhite,
    WhiteAsInverted,
    BlackAsInverted,
};

struct SpriteData;
/**
 * @class Buffer
 * @brief The Buffer class represents a buffer for storing pixel data.
 *
 * The Buffer class provides methods for manipulating the pixel data stored in the buffer.
 */
class Buffer {
    uint8_t _width=0, _height=0;
protected:
    bool remove_buffer=true;
public:
    uint8_t *data;
    Buffer(uint8_t *data, uint8_t w, uint8_t h);
    Buffer(uint8_t width, uint8_t height);
    virtual ~Buffer();

    bool test_pixel(uint8_t x, uint8_t y);

    void copy_into(uint8_t *other);

    void clear();

    bool test_coordinate(int x, int y) const;
    void set_pixel(int16_t x, int16_t y, PixelColor draw_mode);
    void set_pixel_with_check(int16_t x, int16_t y, PixelColor draw_mode);
    void copy_from(uint8_t *other);
    uint8_t width() { return _width; }
    uint8_t height() { return _height; }
    virtual void swap(uint8_t *&buffer);
    uint16_t pixel(uint8_t x, uint8_t y);

    uint8_t* decode(const Icon *icon);
};