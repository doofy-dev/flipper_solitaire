#include "Sprite.h"
#include "Buffer.h"
#include "../assets.h"

Sprite::Sprite(const SpriteData *spriteData, DrawMode d) : Buffer(spriteData->data, spriteData->width,spriteData->height), _icon(spriteData), draw_mode(d) {
}

void Sprite::set_anchor(float x, float y) {
    anchor.x = x;
    anchor.y = y;
}

Vector Sprite::get_offset() {
    return {
            anchor.x * (float) width(),
            anchor.y * (float) height()
    };
}

Sprite::Sprite(const SpriteData &spriteData, DrawMode d) : Buffer(spriteData.data, spriteData.width,spriteData.height), _icon(&spriteData), draw_mode(d) {
}
