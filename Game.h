#pragma once


#include <gui/canvas.h>
#include <notification/notification.h>
#include "utils/List.h"
#include "utils/Card.h"
#include "utils/RenderBuffer.h"
#include "Deck.h"
#include "TableauColumn.h"

typedef enum {
    GameStateGameOver, GameStateStart, GameStatePlay, GameStateAnimate
} PlayState;

class Game {
    Deck *deck;

    Card *piles[4];
    TableauColumn tableau[7];

    TableauColumn hand;

    PlayState state = GameStateStart;
    uint8_t current_row;
    uint8_t current_column;

    uint8_t picked_from_row;
    uint8_t picked_from_column;

    int8_t column_selection;
    RenderBuffer *buffer;
    uint32_t round_start;
    bool can_auto_solve;
    bool had_change = true;

public:
    Game();
    void Render(Canvas* const canvas);

    void Reset();
    void NewRound();

    void LongPress(InputKey key);
    void Press(InputKey key);
    void Update(NotificationApp *app);

    ~Game();
};