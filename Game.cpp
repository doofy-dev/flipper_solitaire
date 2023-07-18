#include "Game.h"
#include "assets.h"

static Sprite logo = Sprite(sprite_logo, BlackOnly);
static Sprite main_image = Sprite(sprite_main_image, BlackOnly);
static Sprite start = Sprite(sprite_start, BlackOnly);

Game::~Game() {
    delete deck;
    for (int i = 0; i < 4; i++)
        delete piles[i];
    delete buffer;
}

void Game::Render(Canvas *const canvas) {
    if (!buffer) return;
    if (had_change) {
        if (state == GameStateStart) {
            buffer->clear();
            buffer->draw(&logo, (Vector) {60, 30}, 0);
            buffer->draw(&main_image, (Vector) {110, 25}, 0);
            buffer->draw(&start, (Vector) {64, 55}, 0);

        } else if (state == GameStatePlay) {
            buffer->clear();
            if (deck)
                deck->Render(buffer, current_row == 0 && current_column == 0, current_row == 0 && current_column == 1);
            for (int i = 0; i < 7; i++) {
                bool selected = current_row == 1 && current_column == i;
                tableau[i].Render(i * 18 + 1, 25, selected, selected ? column_selection : 0, buffer);
            }
            for (int i = 0; i < 4; i++) {
                bool pileSelected = current_row == 0 && (current_column - 3) == i;
                if (piles[i]) {
                    piles[i]->Render(i * 18 + 55, 1, pileSelected, buffer, 22);
                } else {
                    Card::RenderEmptyCard(i * 18 + 55, 1, buffer);
                    if (pileSelected) {
                        buffer->draw_rbox(i * 18 + 56, 2, i * 18 + 71, 23, Flip);
                    }
                }
            }
        }
    }
    had_change = false;
    buffer->render(canvas);
}

Game::Game() {
    deck = new Deck(1);
    buffer = new RenderBuffer(128, 64);
}

void Game::NewRound() {
    round_start = furi_get_tick();
    state = GameStatePlay;
    had_change = true;
}

void Game::Reset() {
    can_auto_solve = false;
    current_column = 0;
    current_row = 0;
    column_selection = -1;

    deck->Generate();
    //Reset foundations
    for (int i = 0; i < 4; i++) {
        if (piles[i]) delete piles[i];
        piles[i] = nullptr;
    }

    //Populate columns
    for (int i = 0; i < 7; i++) {
        tableau[i].Reset();
        for (int j = 0; j <= i; j++) {
            Card *card = deck->Extract();
            if (j >= i) card->exposed = true;
            tableau[i].AddCard(card);
        }
    }
}

void Game::LongPress(InputKey key) {
    UNUSED(key);
}

void Game::Update(NotificationApp *app) {
    UNUSED(app);
}

void Game::Press(InputKey key) {
    if (key == InputKeyOk && state == GameStateStart) {
        Reset();
        NewRound();
        had_change = true;
        return;
    }

    if (key == InputKeyBack && state == GameStatePlay) {
        state = GameStateStart;
        had_change = true;
        return;
    }

    if(key == InputKeyOk && current_column == 0 && current_row == 0){
        deck->Cycle();
        had_change = true;
    }

    if (key == InputKeyLeft && current_column > 0) {
        current_column--;
        had_change = true;
    } else if (key == InputKeyRight && current_column < 6) {
        current_column++;
        if (current_row == 0 && current_column == 2) current_column++;
        had_change = true;
    }

    if (key == InputKeyUp && current_row == 1) {
        uint8_t count = tableau[current_column].Count() - 1;
        uint8_t first_non_flipped = tableau[current_column].FirstNonFlipped();
        if (count > first_non_flipped && (column_selection + 1) > count) {
            column_selection++;
        } else
            current_row--;

        had_change = true;
    } else if (key == InputKeyDown && current_row == 0) {
        current_row++;
        column_selection = 0;
        had_change = true;
    }

    //disable empty space selection
    if (current_row == 0 && current_column == 2) {
        current_column--;
        had_change = true;
    }
}