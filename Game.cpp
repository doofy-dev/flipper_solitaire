#include "Game.h"

Game::~Game() {
    delete deck;
}

void Game::Render(Canvas *const canvas) {
    if (!buffer) return;
    if (had_change) {
        deck->Render(buffer);
        for (int i = 0; i < 7; i++) {
            FURI_LOG_D("Game", "%i", i);
            tableau[i].Render(i * 18 + 1, 25, false, 0, buffer);
        }
        for (int i = 0; i < 4; i++) {
            if (piles[i])
                piles[i]->Render(i * 18 + 55, 1, false, buffer);
            else
                Card::RenderEmptyCard(i * 18 + 55, 1, buffer);
        }

    }
    had_change = false;
    buffer->render(canvas);
}

void Game::Press(InputKey key) {
    if (key == InputKeyOk && state == GameStateStart) {
        Reset();
        NewRound();
        return;
    }
    had_change = true;

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
    deck->Cycle();

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
