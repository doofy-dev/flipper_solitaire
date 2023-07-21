#include "Game.h"
#include "assets.h"
#include <notification/notification.h>
#include <notification/notification_messages.h>

static Sprite logo = Sprite(sprite_logo, BlackOnly);
static Sprite main_image = Sprite(sprite_main_image, BlackOnly);
static Sprite start = Sprite(sprite_start, BlackOnly);

static const NotificationSequence sequence_fail = {
        &message_vibro_on,
        &message_note_c4,
        &message_delay_10,
        &message_vibro_off,
        &message_sound_off,
        &message_delay_10,

        &message_vibro_on,
        &message_note_a3,
        &message_delay_10,
        &message_vibro_off,
        &message_sound_off
};

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
            if (hand.Count() > 0)
                hand.Render(current_column * 18 + 10, current_row * 25 + 15, false, 0, buffer);
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
    hand.Clear();
    picked_from_column = -1;
    picked_from_row = -1;

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

void Game::Update() {
}

void Game::Press(InputKey key) {
//region Navigation
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

    if (key == InputKeyLeft && current_column > 0) {
        current_column--;
        had_change = true;
    } else if (key == InputKeyRight && current_column < 6) {
        current_column++;
        if (current_row == 0 && current_column == 2) current_column++;
        had_change = true;
    }

    if (key == InputKeyUp && current_row == 1) {
        uint8_t count = tableau[current_column].Count()-1;
        uint8_t first_non_flipped = tableau[current_column].FirstNonFlipped();
        if (count > first_non_flipped && (column_selection) < count) {
            column_selection++;
        } else
            current_row--;

        had_change = true;
    } else if (key == InputKeyDown && current_row == 0) {
        current_row++;
        column_selection = 0;
        had_change = true;
    } else if (key == InputKeyDown && current_row == 1 && column_selection > 0) {
        column_selection--;
        had_change = true;
    }

    //disable empty space selection
    if (current_row == 0 && current_column == 2) {
        current_column--;
        column_selection = 0;
        had_change = true;
    }
//endregion

    //Pick/cycle logic
    if (hand.Count() == 0) {
        if (key == InputKeyOk) {
            if (current_row == 0) {
                //cycle deck
                if (current_column == 0) {
                    deck->Cycle();
                    had_change = true;
                } else if (current_column == 1) {
                    //pick from deck
                    Card *c = deck->GetLastWaste();
                    if (c) {
                        hand.AddCard(c);
                        picked_from_column = current_column;
                        picked_from_row = current_row;
                        had_change = true;
                    }
                }
                return;
            } else {
                //Tableau logic
                auto *column = &tableau[current_column];
                //Flip last card if it is not exposed
                if (column->Count() > 0) {
                    if (!column->LastCard()->exposed) {
                        column->Reveal();
                        had_change = true;
                    } else {
                        //Pick selection
                        hand.AddCard(column->Pop());
                        had_change = true;
                        picked_from_row = current_row;
                        picked_from_column = current_column;
                    }
                } else {
                    ErrorMessage();
                }
                return;
            }
        }
    } else {
        //Place logic
        if (key == InputKeyOk) {
            if (current_column == picked_from_column && current_row == picked_from_row) {
                //add back to tableau
                if (picked_from_row == 0) {
                    had_change = true;
                    deck->AddToWaste(hand.Pop());
                }
            } else if (hand.Count() == 1 && current_row == 0 && current_column > 1) {
                Card *current = piles[current_column - 3];
                if (Card::CanPlace(current, hand.TopCard())) {
                    piles[current_column - 3] = hand.Pop();
                    had_change = true;
                } else {
                    ErrorMessage();
                }
            } else if (current_row == 1) {
                auto *column = &tableau[current_column];
                if ((current_row == picked_from_row && current_column == picked_from_column) ||
                    column->CanPlace(&hand)) {
                    column->Merge(&hand);
                    had_change = true;
                }
            } else {
                ErrorMessage();
            }
            return;
        }
    }
}

void Game::ErrorMessage() {

}
