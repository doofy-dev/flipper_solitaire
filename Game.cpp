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
        &message_sound_off,
        NULL
};

Game::~Game() {
    furi_record_close(RECORD_NOTIFICATION);
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
//                check_pointer(piles[i]);
                if (piles[i]) {
                    piles[i]->Render(i * 18 + 55, 1, pileSelected, buffer, 22);
                } else {
                    Card::RenderEmptyCard(i * 18 + 55, 1, buffer);
                    if (pileSelected) {
                        buffer->draw_rbox(i * 18 + 56, 2, i * 18 + 71, 23, Flip);
                    }
                }
            }
            if (hand.Count() > 0) {
                hand.Render(current_column * 18 + 10, current_row * 25 + 15, false, 0, buffer);
            }
        }
    }
    had_change = false;
    buffer->render(canvas);
    if (state == GameStatePlay && can_auto_solve) {
        canvas_set_font(canvas,FontSecondary);

        canvas_set_color(canvas, ColorBlack);
        canvas_draw_frame(canvas, 8, 52, 112, 12);
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_box(canvas, 9, 53, 110, 10);

        canvas_set_color(canvas, ColorBlack);
        canvas_draw_str_aligned(canvas, 64, 58, AlignCenter, AlignCenter, "Long press > to auto solve!");
    }
}

Game::Game() {
    deck = new Deck(1);
    buffer = new RenderBuffer(128, 64);

    notifications = static_cast<NotificationApp *>(furi_record_open(RECORD_NOTIFICATION));
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
    if (key == InputKeyOk) {
        if (hand.Count() == 1) {
            for (uint8_t i = 0; i < 4; i++) {
                Card *current = piles[i];
                if (Card::CanPlace(current, hand.TopCard())) {
                    check_pointer(piles[i]);
                    piles[i] = hand.Pop();
                    had_change = true;
                    return;
                }
            }
        }
    } else if (key == InputKeyUp) {
        current_row = 0;
        had_change = true;
        if (current_column == 2) current_column--;
    }
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
        column_selection = 0;
    } else if (key == InputKeyRight && current_column < 6) {
        current_column++;
        column_selection = 0;
        if (current_row == 0 && current_column == 2) current_column++;
        had_change = true;
    }

    if (key == InputKeyUp && current_row == 1) {
        uint8_t count = tableau[current_column].Count();
        uint8_t first_non_flipped = tableau[current_column].FirstNonFlipped();
        if (hand.Count() == 0) {
            if ((count - column_selection - 1) > first_non_flipped && (column_selection) < count) {
                column_selection++;
            } else
                current_row--;
        } else {
            current_row--;
        }

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
            } else {
                //Tableau logic
                auto *column = &tableau[current_column];
                //Flip last card if it is not exposed
                if (column->Count() > 0) {
                    if (!column->LastCard()->exposed) {
                        column->Reveal();
                        had_change = true;
                    } else {
//                        hand.AddCard(column->Pop());
                        //Pick selection
                        auto *data = column->splice(column_selection);
                        hand.AddRange(data);
                        data->soft_clear();
                        delete data;
                        column_selection = 0;
                        had_change = true;
                        picked_from_row = current_row;
                        picked_from_column = current_column;
                    }
                } else {
                    ErrorMessage();
                }
            }
        }
    } else {
        //Place logic
        if (key == InputKeyOk) {
            //Place back to where it was picked up
            if (current_column == picked_from_column && current_row == picked_from_row) {
                //add back to tableau
                if (picked_from_row == 0) {
                    FURI_LOG_D("PLACE", "reinsert top");
                    had_change = true;
                    deck->AddToWaste(hand.Pop());
                } else {
                    FURI_LOG_D("PLACE", "reinsert tableau %i %i", hand.Count(), current_column);
                    auto *column = &tableau[current_column];
                    if (column) {
                        FURI_LOG_D("PLACE", "BEFORE MERGE %i", hand.Count());
                        column->Merge(&hand);
                        had_change = true;
                        FURI_LOG_D("PLACE", "AFTER MERGE");
                    } else {
                        FURI_LOG_E("PLACE", "TABLEAU ERROR");
                    }
                }
                //Place to the top piles
            } else if (hand.Count() == 1 && current_row == 0 && current_column > 1) {
                FURI_LOG_D("PLACE", "place top");
                check_pointer(piles[current_column - 3]);
                Card *current = piles[current_column - 3];
                if (Card::CanPlace(current, hand.TopCard())) {
                    piles[current_column - 3] = hand.Pop();
                    had_change = true;
                } else {
                    ErrorMessage();
                }
                //Place to the tableau columns
            } else if (current_row == 1) {
                FURI_LOG_D("PLACE", "place bottom");
                auto *column = &tableau[current_column];
                if (column) {
                    if ((current_row == picked_from_row && current_column == picked_from_column) ||
                        column->CanPlace(&hand)) {
                        FURI_LOG_D("PLACE", "canplace");
                        column->Merge(&hand);
                        had_change = true;
                    }
                } else {
                    FURI_LOG_E("PLACE", "TABLEAU ERROR in bottom place");
                }
            } else {
                ErrorMessage();
            }
        }
    }

    CheckCanAutoSolve();
}

void Game::ErrorMessage() {
    notification_message(notifications, &sequence_fail);
}

void Game::CheckCanAutoSolve() {
    uint8_t ok = 0;
    for (uint8_t i = 0; i < 7; i++) {
        if (tableau[i].Count() == 0 || tableau[i].TopCard()->exposed)
            ok++;
    }
    can_auto_solve = ok == 7;
    if (can_auto_solve)
        FURI_LOG_D("Solve", "can auto solve");
}
