#pragma once

#include "RenderBuffer.h"
#include "Vector.h"

/**
 * @class Sprite
 * @brief The Sprite class represents a graphical sprite that can be drawn on a Buffer.
 *
 * The Sprite class inherits from the Buffer class and adds functionality specific to sprites.
 * It contains information about the sprite's icon, anchor point, and draw mode.
 */
class Sprite : public Buffer {
    const SpriteData *_icon;
    Vector anchor = {0.5, 0.5};
public:
    DrawMode draw_mode;

    Sprite(const SpriteData *icon, DrawMode draw_mode);
    Sprite(const SpriteData &icon, DrawMode draw_mode);

    virtual ~Sprite() {
        FURI_LOG_D("App", "Sprite cleared");
    }

    const SpriteData *get_data() { return _icon; }

    void set_anchor(float x, float y);

    Vector get_offset();
};