#include "RenderBuffer.h"
#include "Sprite.h"
#include "Helpers.h"
#include "Vector.h"
#include "cmath"

RenderBuffer::RenderBuffer(uint8_t w, uint8_t h, bool doubleBuffered) : Buffer(w, h) {
    FURI_LOG_D("App", "New renderBuffer");
    if (doubleBuffered)
        frontBuffer = new Buffer(w, h);
}

/**
 * Renders the contents of the RenderBuffer onto the given Canvas.
 * This function iterates over each pixel in the RenderBuffer and checks
 * if it should be drawn onto the canvas. If a pixel satisfies the test_pixel
 * condition, it calls the canvas_draw_dot function to draw the pixel onto
 * the canvas.
 *
 * @param canvas A pointer to the Canvas object where the RenderBuffer will be rendered.
 */
void RenderBuffer::render(Canvas *const canvas) {
    if (!doubleBuffer)
        canvas_draw_xbm(canvas, 0, 0, width(), height(), data);
    else
        canvas_draw_xbm(canvas, 0, 0, width(), height(), frontBuffer->data);
}

/**
 * @brief Draws a line from (x0, y0) to (x1, y1) on the render buffer.
 *
 * @param x0 The x-coordinate of the starting point of the line.
 * @param y0 The y-coordinate of the starting point of the line.
 * @param x1 The x-coordinate of the ending point of the line.
 * @param y1 The y-coordinate of the ending point of the line.
 * @param draw_mode The color to draw the line with.
 */
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

/**
 * @brief Draws a circle on the render buffer.
 *
 * This function draws a circle using Bresenham's circle drawing algorithm. The circle is centered at the given x and y coordinates,
 * with the given radius, and using the specified color. The circle is drawn by plotting symmetrical points in the eight octants
 * of the circle.
 *
 * @param x The x-coordinate of the center of the circle.
 * @param y The y-coordinate of the center of the circle.
 * @param r The radius of the circle.
 * @param color The color of the circle.
 */
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

/**
 * @brief RenderBuffer class handles rendering operations on a buffer.
 */
void RenderBuffer::draw(Sprite *const sprite, Vector position, float rotation) {
    draw(sprite, position, sprite->width(), sprite->height(), rotation);
}


/**
 * Draws a sprite on the render buffer.
 *
 * @param sprite A pointer to the sprite to be drawn.
 * @param position The position where the sprite should be drawn.
 * @param x_cap The horizontal limit for the drawing area.
 * @param y_cap The vertical limit for the drawing area.
 * @param rotation The rotation angle for the sprite.
 */
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

/**
 * @brief Draws a sprite on the render buffer.
 *
 * This function takes a sprite and draws it on the render buffer at the specified position, with optional rotation,
 * and limited to a maximum width and height cap. The draw color and whether to consider certain pixels as black can
 * also be specified.
 *
 * @warning This function does not implement proper scaling.
 *
 * @param sprite The sprite to be drawn.
 * @param is_black Whether to consider certain pixels in the sprite as black.
 * @param draw_color The color to use for drawing.
 * @param position The position at which to draw the sprite.
 * @param x_cap The maximum width cap for drawing.
 * @param y_cap The maximum height cap for drawing.
 * @param rotation The rotation angle, in radians, for the sprite. Positive values rotate the sprite clockwise.
 */
//TODO: proper scaling
void RenderBuffer::draw_sprite(Sprite *const sprite, bool is_black, PixelColor draw_color, const Vector &position,
                               uint8_t x_cap, uint8_t y_cap, float rotation) {
    Vector center = sprite->get_offset();
    Vector transform;
    int max_w = fmin(sprite->width(), x_cap);
    int max_h = fmin(sprite->height(), y_cap);
    bool isOn;
    int16_t finalX, finalY;
    for (int y = 0; y < max_h; y++) {
        for (int x = 0; x < max_w; x++) {
            transform = Vector(x, y) - center;
            transform.rotate(rotation);
            transform += position;
            finalX = (int16_t) round(transform.x);
            finalY = (int16_t) round(transform.y);
            if (test_coordinate(finalX, finalY)) {
                isOn = sprite->test_pixel(x, y) == is_black;
                if (isOn)
                    set_pixel(finalX, finalY, draw_color);
            }
        }
    }
}

RenderBuffer::~RenderBuffer() {
    if (doubleBuffer)
        delete frontBuffer;
    delete data;
    FURI_LOG_I("App", "RenderBuffer end");
}

/**
 * @brief Draws a filled rectangle on the render buffer.
 *
 * This function draws a filled rectangle on the render buffer defined by the top-left corner (x0, y0) and the
 * bottom-right corner (x1, y1) coordinates. The draw_mode parameter specifies the color to use for filling the rectangle.
 *
 * The rectangle is drawn by iterating through each pixel within the given coordinates and setting its color to the draw_mode.
 * However, the corners with coordinates (x0, y0) and (x1, y1) are not filled, resulting in an unfilled rectangle shape.
 *
 * If a pixel falls on the corners of the rectangle or if it is outside the boundaries of the render buffer, it is skipped.
 *
 * @param x0 The x-coordinate of the top-left corner of the rectangle.
 * @param y0 The y-coordinate of the top-left corner of the rectangle.
 * @param x1 The x-coordinate of the bottom-right corner of the rectangle.
 * @param y1 The y-coordinate of the bottom-right corner of the rectangle.
 * @param draw_mode The color to use for filling the rectangle.
 */
void RenderBuffer::draw_rbox(int16_t x0, int16_t y0, int16_t x1, int16_t y1, PixelColor draw_mode) {
    for (int16_t x = x0; x < x1; x++) {
        for (int16_t y = y0; y < y1; y++) {
            if (((x == x0 || x == x1 - 1) && (y == y0 || y == y1 - 1)) || !test_coordinate(x, y)) continue;
            set_pixel(x, y, draw_mode);
        }
    }
}

/**
 * @brief Draws a rectangular frame on the RenderBuffer using the given coordinates and draw mode.
 *
 * This function draws a rectangular frame on the RenderBuffer defined by the top-left corner (x0, y0)
 * and the bottom-right corner (x1, y1). The frame consists of four lines that form the outline of the rectangle.
 * The drawing mode is specified by the draw_mode parameter.
 *
 * @param x0 The x-coordinate of the top-left corner of the rectangle.
 * @param y0 The y-coordinate of the top-left corner of the rectangle.
 * @param x1 The x-coordinate of the bottom-right corner of the rectangle.
 * @param y1 The y-coordinate of the bottom-right corner of the rectangle.
 * @param draw_mode The drawing mode to be used for rendering the lines.
 *
 * @see PixelColor
 * @see RenderBuffer::draw_line
 *
 * @note This function assumes that the RenderBuffer instance has been properly initialized and allocated.
 *       The given coordinates must be within the valid range of the RenderBuffer dimensions.
 *       The draw_mode should be one of the specified PixelColor enumeration values.
 */
void RenderBuffer::draw_rbox_frame(int16_t x0, int16_t y0, int16_t x1, int16_t y1, PixelColor draw_mode) {
    draw_line(x0 + 1, y0, x1 - 1, y0, draw_mode);
    draw_line(x0 + 1, y1, x1 - 1, y1, draw_mode);

    draw_line(x0, y0 + 1, x0, y1 - 1, draw_mode);
    draw_line(x1, y0 + 1, x1, y1 - 1, draw_mode);
}

/**
 * @brief Draws a filled box on the render buffer.
 *
 * This function draws a filled box with the specified coordinates and color on the render buffer.
 * The box is drawn by setting each pixel within the specified coordinates to the specified color.
 *
 * @param x0 The x-coordinate of the top-left corner of the box.
 * @param y0 The y-coordinate of the top-left corner of the box.
 * @param x1 The x-coordinate of the bottom-right corner of the box.
 * @param y1 The y-coordinate of the bottom-right corner of the box.
 * @param draw_mode The color to be used for drawing the box.
 *
 * @note The coordinates (x0, y0) are inclusive, while (x1, y1) are exclusive.
 */
void RenderBuffer::draw_box(int16_t x0, int16_t y0, int16_t x1, int16_t y1, PixelColor draw_mode) {
    for (int16_t x = x0; x < x1; x++) {
        for (int16_t y = y0; y < y1; y++) {
            set_pixel(x, y, draw_mode);
        }
    }
}

void RenderBuffer::swap() {
    if (doubleBuffer)
        frontBuffer->swap(data);
}
