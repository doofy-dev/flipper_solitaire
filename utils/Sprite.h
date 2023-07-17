#pragma once

#include "RenderBuffer.h"
#include "Vector.h"

struct SpriteData {
    uint8_t width = 0;
    uint8_t height = 0;
    uint8_t *data = NULL;
};

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