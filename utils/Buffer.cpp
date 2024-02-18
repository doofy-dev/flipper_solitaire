#include <toolbox/compress.h>
#include <firmware_api_table.h>
#include "Buffer.h"
#include "Sprite.h"

Buffer::Buffer(uint8_t w, uint8_t h) : _width(w), _height(h) {
    data = (uint8_t *) malloc(sizeof(uint8_t) * (int)ceil(w / 8.0) * ceil(h));
}

Buffer::Buffer(uint8_t *d, uint8_t w, uint8_t h) {
    _width = w;
    _height = h;//ceil(i->height / 8.0);
    data = d;
    remove_buffer = false;
}

Buffer::~Buffer() {
    FURI_LOG_I("BUFFER", "deleting buffer");
    if (remove_buffer) {
        delete data;
    }
}

bool Buffer::test_pixel(uint8_t x, uint8_t y) {
    return data[pixel(x,y)] & (1 << (x & 7));
}

/**
 * @brief Copies the contents of the buffer into another array.
 *
 * This function copies the data from the current buffer into the provided array. The size of the array should be equal
 * to the size of the buffer.
 *
 * @param other The array to copy the data into.
 * @return void
 */
void Buffer::copy_into(uint8_t *other) {
    int size = (int) (_height * ceil(_width / 8.0));
    for (int i = 0; i < size; i++) {
        other[i] = data[i];
    }
}

/**
 * @brief Copies data from another buffer into this buffer.
 *
 * This function copies the data from the given buffer into this buffer.
 * The size of the data being copied is calculated according to the width
 * and height of this buffer. The data is copied byte by byte.
 *
 * @param other A pointer to the data buffer to copy from.
 */
void Buffer::copy_from(uint8_t *other) {
    int size = (int) (_height * ceil(_width / 8.0));
    for (int i = 0; i < size; i++) {
        data[i] = other[i];
    }
}

/**
 * @brief Clears the buffer by setting all data elements to 0.
 *
 * The buffer size is calculated based on the width and height attributes.
 * The width and height are multiplied to get the size in bytes needed to store the buffer data.
 * If the height is not a multiple of 8, the size is rounded up to the next multiple of 8.
 * Each element in the data array is set to 0 using a for loop.
 */
void Buffer::clear() {
    int size = (int) (_height * ceil(_width / 8.0));
    memset(data, 0, sizeof(uint8_t)*size);
/*    for (int i = 0; i < size; i++) {
        data[i] = 0;
    }*/
}


/**
* @brief Checks if the given coordinates are within the bounds of the buffer.
*
* @param x The x-coordinate.
* @param y The y-coordinate.
* @return True if the coordinates are within the buffer's bounds, false otherwise.
*/
bool Buffer::test_coordinate(int x, int y) const {
    return x >= 0 && y >= 0 && x < _width && y < _height;
}

/**
 * @brief Sets a pixel in the buffer with a given draw mode, after checking if the pixel is within the buffer dimensions.
 *
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * @param draw_mode The draw mode to use for setting the pixel.
 *
 * This function checks if the pixel at the given coordinates is within the buffer dimensions by calling the `test_pixel` function.
 * If the pixel is within the buffer, it calls the `set_pixel` function to set the pixel with the provided draw mode.
 *
 * @see test_pixel
 * @see set_pixel
 */
void Buffer::set_pixel_with_check(int16_t x, int16_t y, PixelColor draw_mode) {
    if (test_pixel(x, y))
        set_pixel(x, y, draw_mode);
}

/**
 * Sets the pixel at the specified coordinates to the specified draw mode.
 *
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * @param draw_mode The draw mode to apply to the pixel.
 */
void Buffer::set_pixel(int16_t x, int16_t y, PixelColor draw_mode) {
    uint8_t bit = 1 << (x & 7);
    uint8_t *p = &data[pixel(x,y)];

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

/**
 * @brief Swaps the buffer with the provided buffer.
 *
 * This function swaps the internal buffer with the buffer passed as a parameter. After the swap,
 * the caller will have ownership of the current internal buffer.
 *
 * @param buffer A pointer to the buffer to swap with.
 */
void Buffer::swap(uint8_t *&buffer) {
    uint8_t *back = data;
    data = buffer;
    buffer = back;
}



uint16_t Buffer::pixel(uint8_t x, uint8_t y) {
    return (y*width()+x) / 8;
}

uint8_t *Buffer::decode(const Icon *icon) {
    uint8_t* p_icon_data;
    uint8_t width = icon_get_width(icon);
    uint8_t height = icon_get_height(icon);
    uint16_t size = (width/8)*height;

    // Create decoder instance and grab decoded data pointer
    // - Ideally re-use the CompressIcon and free on app exit
    CompressIcon* compress_icon = compress_icon_alloc();
    compress_icon_decode(compress_icon, icon_get_data(icon), &p_icon_data);

    // Copy the data since the decoder only holds on temporarily
    auto* icon_data = static_cast<uint8_t *>(malloc(size));
    memcpy(icon_data, p_icon_data, size);

    // Free decoder instance as we're done here
    compress_icon_free(compress_icon);

    return icon_data;
}
