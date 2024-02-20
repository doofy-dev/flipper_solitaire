#include <dolphin/helpers/dolphin_deed.h>
#include <dolphin/dolphin.h>
#include <notification/notification_messages.h>
#include "GameLogic.h"
#include "utils/Sprite.h"
#include "assets.h"

//#define SOLVE_TEST

static Sprite logo = Sprite(sprite_logo, BlackOnly);
static Sprite solve = Sprite(sprite_solve, BlackOnly);
static Sprite main_image = Sprite(sprite_main_image, BlackOnly);
static Sprite start = Sprite(sprite_start, BlackOnly);

GameLogic::GameLogic(RenderBuffer *b, InputEventHandler *inputHandler, NotificationApp *notification_app) : buffer(b),
                                                                                                            notification(
                                                                                                                notification_app) {
    inputHandler->subscribe(this, [](void *ctx, int key, InputType type) {
        auto *inst = (GameLogic *) ctx;
        inst->Input(key, type);
    });
}

void GameLogic::Input(int key, InputType type) {
    if (type == InputTypeShort) {
        if (key == InputKeyBack && state == Play) {
            state = Logo;
            target[0] = 0;
            target[1] = 0;
            return;
        } else if (key == InputKeyOk) {
            if (state == Logo) {
                state = Intro;
                dolphin_deed(DolphinDeedPluginGameStart);
                return;
            }
            if (state == Solve) {
                end = furi_get_tick();
                QuickSolve();
                return;
            }
            if (state == Finish) {
                dolphin_deed(DolphinDeedPluginGameWin);
                state = Logo;
                return;
            } else if (state == Intro) {
                for (int i = 0; i < 7; i++) {
                    while ((int) tableau[i].size() < (i + 1)) {
                        tableau[i].push_back(stock.pop_back());
                    }
                    tableau[i].peek_back()->exposed = true;
                }
                tempCard = nullptr;
                state = Play;
                startTime = furi_get_tick();
                buffer->clear();
                DrawPlayScene();
            } else if (state == Play) {
                if (selection[0] == 0 && selection[1] == 0) {
                    //Cycle waste and stock
                    if (hand.size() == 0) {
                        if (stock.size() > 0) {
                            //move from stock to waste
                            waste.push_back(stock.pop_back());
                            waste.peek_back()->exposed = true;
                        } else {
                            //move back all the card
                            while (waste.size() > 0) {
                                waste.peek_back()->exposed = false;
                                stock.push_back(waste.pop_back());
                            }
                        }
                        return;
                    }
                    return;
                } else if (selection[1] == 1 || selection[0] > 0) {
                    PickAndPlace();
                }
                return;
            }
            return;
        }
        if (state == Play)
            HandleNavigation(key);
    } else if (type == InputTypeLong) {
        switch (key) {
            case InputKeyLeft:
                selection[0] = 0;
                selectedCard = 1;
                break;
            case InputKeyRight:
                selection[0] = 6;
                selectedCard = 1;
                break;
            case InputKeyUp:
                selectedCard = 1;
                selection[1] = 0;
                break;
            case InputKeyOk:
                if (CanSolve()) {
                    state = Solve;
                    break;
                }
                //quick place
                if (state == Play && (selection[0] == 1 || selection[1] == 1) && selectedCard == 1) {
                    Card *c = selection[1] == 0 ? waste.peek_back() : tableau[selection[0]].peek_back();
                    if (!c->exposed) return;
                    for (int i = 0; i < 4; i++) {
                        if (c->CanPlaceFoundation(foundation[i].peek_back())) {
                            if (selection[1] == 0)
                                foundation[i].push_back(waste.pop_back());
                            else
                                foundation[i].push_back(tableau[selection[0]].pop_back());
                            return;
                        }
                    }
                    PlayError();
                }
                break;
            default:
                break;
        }
    }

    if (selection[1] == 0 && selection[0] == 2) { //skip empty space
        selection[0] += key == InputKeyRight ? 1 : -1;
    }
}

void GameLogic::PickAndPlace() {
    //pick and place waste
    if (selection[0] == 1 && selection[1] == 0) {
        if (waste.size() > 0 && hand.size() == 0) {
            hand.push_back(waste.pop_back());
            target[0] = 1;
            target[1] = 0;
        } else if (hand.size() == 1 && target[0] == 1 && target[1] == 0) {
            waste.push_back(hand.pop_back());
            target[0] = 0;
            target[1] = 0;
        } else {
            PlayError();
        }
    }
        //place to foundation
    else if (selection[1] == 0 && selection[0] > 2) {
        if (hand.size() == 1) {
            uint8_t id = selection[0] - 3;
            if (hand.peek_back()->CanPlaceFoundation(foundation[id].peek_back())) {
                foundation[id].push_back(hand.pop_back());
                target[0] = selection[0];
                target[1] = selection[1];
            } else {
                PlayError();
            }
        } else {
            PlayError();
        }
    }
        //pick and place columns
    else if (selection[1] == 1) {
        auto &tbl = tableau[selection[0]];
        if (hand.size() == 0) {
            if(tbl.peek_back()) {
                if (!tbl.peek_back()->exposed) {
                    tbl.peek_back()->exposed = true;
                } else {
                    uint8_t count = selectedCard;
                    while (count > 0) {
                        hand.push_front(tbl.pop_back());
                        count--;
                    }
                    selectedCard = 1;
                    target[0] = selection[0];
                    target[1] = selection[1];
                }
            } else {
                PlayError();
            }
        } else if ((target[0] == selection[0] && target[1] == selection[1]) ||
                   hand.peek_front()->CanPlaceColumn(tbl.peek_back())) {
            while (hand.size() > 0) {
                tbl.push_back(hand.pop_front());
            }
            target[0] = 0;
            target[1] = 0;
        } else {
            PlayError();
        }
    }

    for (uint8_t i = 0; i < 4; i++) {
        if (foundation[i].size() == 0 || foundation[i].peek_back()->value != KING)
            return;
    }

    buffer->clear();
    DrawPlayScene();
    selectedCard = 0;
    state = Finish;
}

void GameLogic::HandleNavigation(int key) {
    if (key == InputKeyLeft && selection[0] > 0) {
        selectedCard = 1;
        selection[0]--;
    } else if (key == InputKeyRight && selection[0] < 6) {
        selectedCard = 1;
        selection[0]++;
    } else if (key == InputKeyUp && selection[1] == 1) {
        auto &tbl = tableau[selection[0]];
        uint8_t first = FirstNonFlipped(tbl);
        if (tbl.size() > 0 && first < tbl.size() - selectedCard && hand.size() == 0) {
            selectedCard++;
        } else {
            selectedCard = 1;
            selection[1]--;
        }
    } else if (key == InputKeyDown && selection[1] == 0) {
        selectedCard = 1;
        selection[1]++;
    } else if (selection[1] == 1 && selectedCard > 1) {
        selectedCard--;
    }
}


void GameLogic::Update(float delta) {
    //keep the buffer for the falling animation to achieve the trailing effect
    if (state != Finish) {
        buffer->clear();
    }
    switch (state) {
        case Logo:
            Reset();
            buffer->draw(&logo, (Vector) {60, 30}, 0);
            buffer->draw(&main_image, (Vector) {115, 25}, 0);
            buffer->draw(&start, (Vector) {64, 55}, 0);
            break;
        case Intro:
            DoIntro(delta);
            dirty = true;
            end = 0;
            break;
        case Play:
            DrawPlayScene();
            if (CanSolve()) {
                buffer->draw_rbox(25, 53, 101, 64, Black);
                buffer->draw_rbox(26, 54, 100, 63, White);
                buffer->draw(&solve, (Vector) {64, 58}, 0);
                end = furi_get_tick();
            }
            break;
        case Solve:
            DrawPlayScene();
            HandleSolve(delta);
            dirty = true;
            break;
        case Finish:
            dirty = true;
            FallingCard(delta);
            break;
        default:
            break;
    }
    buffer->swap();
    readyToRender = true;
}

void GameLogic::Reset() {
    dolphin_deed(DolphinDeedPluginGameStart);
    delete tempCard;
    stock.deleteData();
    stock.clear();
    waste.deleteData();
    waste.clear();
    tempPos = {0, 0};
    selection[0] = 0;
    selection[1] = 0;
    target[0] = 0;
    selectedCard = 1;
    target[1] = -1;
    for (int i = 0; i < 7; i++) {
        tableau[i].deleteData();
        tableau[i].clear();
        if (i < 4) {
            foundation[i].deleteData();
            foundation[i].clear();
        }
    }
    GenerateDeck();
}

bool GameLogic::CanSolve() {
    for (uint8_t i = 0; i < 7; i++) {
        if (tableau[i].peek_front() && !tableau[i].peek_front()->exposed)
            return false;
    }
    return state == Play;
}

void GameLogic::GenerateDeck() {
    int cards_count = 52;
    uint8_t cards[cards_count];
    for (int i = 0; i < cards_count; i++) cards[i] = i % 52;
    srand(DWT->CYCCNT);
#ifndef SOLVE_TEST
    //reorder
    for (int i = 0; i < cards_count; i++) {
        int r = i + (rand() % (cards_count - i));
        uint8_t card = cards[i];
        cards[i] = cards[r];
        cards[r] = card;
    }
#endif
    //Init deck list
    for (int i = 0; i < cards_count; i++) {
        int letter = cards[i] % 13;
        int suit = cards[i] / 13;
        stock.push_back(new Card(suit, letter));
    }
}

GameLogic::~GameLogic() {
    stock.deleteData();
    stock.clear();
    waste.deleteData();
    waste.clear();
    hand.deleteData();
    hand.clear();
    delete tempCard;
    for (int i = 0; i < 7; i++) {
        tableau[i].deleteData();
        tableau[i].clear();
        if (i < 4) {
            foundation[i].deleteData();
            foundation[i].clear();
        }
    }
}

Vector pos, targetPos;

void GameLogic::DoIntro(float delta) {
    //render next after finish
#ifdef SOLVE_TEST
    startTime = furi_get_tick();
    state = Play;
    buffer->clear();
    DrawPlayScene();
    return;
#endif

    if (!buffer) return;
    buffer->clear();
    DrawPlayScene();


    if (tempCard) {
        targetPos = {
            2.0f + (float) target[0] * 18,
            MIN(25.0f + (float) target[1] * 4, 36)
        };
        tempTime += delta * 10;
        pos = Vector::Lerp(tempPos, targetPos, MIN(tempTime, 1));
        tempCard->Render((int) pos.x, (int) pos.y, false, buffer);
        float distance = targetPos.distance(pos);
        if (distance < 0.01) {
            tempCard = nullptr;
            tableau[target[0]].push_back(stock.pop_back());
            tableau[target[0]].peek_back()->exposed = target[0] == target[1];
            if (target[0] == 6 && target[1] == 6) {
                startTime = furi_get_tick();
                state = Play;
                buffer->clear();
                DrawPlayScene();
            }
        }
    } else {
        tempTime = 0;
        tempCard = stock.peek_back();
        tempPos.x = 2;
        tempPos.y = 1;
        if (target[0] == target[1]) {
            target[0]++;
            target[1] = 0;
        } else {
            target[1]++;
        }
    }
}

void GameLogic::DrawPlayScene() {
    if (stock.size() > 0) {
        if (stock.size() > 1) {
            stock.peek_back()->Render(2, 1, true, buffer);
            stock.peek_back()->Render(0, 0, selection[0] == 0 && selection[1] == 0, buffer);
        } else {
            stock.peek_back()->Render(2, 1, selection[0] == 0 && selection[1] == 0, buffer);
        }
    } else
        Card::TryRender(nullptr, 2, 1, selection[0] == 0 && selection[1] == 0, buffer);

    Card::TryRender(waste.peek_back(), 20, 1, selection[0] == 1 && selection[1] == 0, buffer);

    int i;
    for (i = 0; i < 4; i++) {
        Card::TryRender(foundation[i].peek_back(), 56 + i * 18, 1, selection[0] == i + 3 && selection[1] == 0, buffer);
    }
    for (i = 0; i < 7; i++) {
        DrawColumn(2 + i * 18, 25, (selection[0] == i && selection[1] == 1) ? selectedCard : 0, i);
    }

    if (hand.size() > 0) {
        if (selection[1] == 0)
            DrawColumn(10 + selection[0] * 18, 15 + selection[1] * 25, hand.size(), -1);
        else {
            int shift = MIN((int) tableau[selection[0]].size(), 4) * 4;
            DrawColumn(10 + selection[0] * 18, 30 + shift, hand.size(), -1);
        }
    }
}

void GameLogic::DrawColumn(uint8_t x, uint8_t y, uint8_t selected, int8_t column) {
    UNUSED(selected);
    auto &deck = column >= 0 ? tableau[column] : hand;
    if (deck.size() == 0 && column >= 0) {
        Card::RenderEmptyCard(x, y, buffer);
        if (selected == 1)
            buffer->draw_rbox(x + 1, y + 1, x + 16, y + 22, Flip);
        return;

    }
    uint8_t selection = deck.size() - selected;
//    if (selected != 0) selection--;
    uint8_t loop_end = deck.size();
    uint8_t loop_start = MAX(loop_end - 4, 0);
    uint8_t position = 0;
    uint8_t first_non_flipped = FirstNonFlipped(deck);
    bool had_top = false;
    bool showDark = column >= 0;

    // Draw the first flipped and non-flipped card with adjusted visibility
    if (first_non_flipped <= loop_start && selection != first_non_flipped) {
        // Draw a card back if it is not the first card
        if (first_non_flipped > 0) {
            Card::RenderBack(x, y + position, false, buffer, 5);
            // Increment loop start index and position
            position += 4;
            loop_start++;
            had_top = true;
        }
        // Draw the front side of the first non-flipped card
        deck[first_non_flipped]->Render(x, y + position, false, buffer, deck.size() == 1 ? 22 : 9);
        position += 8;
        loop_start++; // Increment loop start index
    }

    // Draw the selected card with adjusted visibility
    if (loop_start > selection) {
        if (!had_top && first_non_flipped > 0) {
            Card::RenderBack(x, y + position, false, buffer, 5);
            position += 4;
            loop_start++;
        }
        // Draw the front side of the selected card
        deck[selection]->Render(x, y + position, showDark, buffer, 9);
        position += 8;
        loop_start++; // Increment loop start index
    }

    int height = 5;
    uint8_t i = 0;
    for (auto *card: deck) {
        if (i >= loop_start && i < loop_end) {
            height = 5;
            if ((i + 1) == loop_end) height = 22;
            else if (i == selection || i == first_non_flipped) height = 9;
            card->Render(x, y + position, i == selection && showDark, buffer, height);
            if (i == selection || i == first_non_flipped)position += 4;
            position += 4;
        }
        i++;
    }
}

int8_t GameLogic::FirstNonFlipped(const List<Card> &deck) {
    int8_t index = 0;
    for (auto *card: deck) {
        if (card->exposed) return index;
        index++;
    }

    return -1;
}


void GameLogic::HandleSolve(float delta) {
    if (tempCard) {

        tempTime += delta * 8;
        Vector finalPos{56 + (float) target[0] * 18, 2};
        if (tempTime > 1) tempTime = 1;
        Vector localpos = Vector::Lerp(tempPos, finalPos, tempTime);
        tempCard->Render((uint8_t) localpos.x, (uint8_t) localpos.y, false, buffer);
        if (finalPos.distance(localpos) < 0.01) {
            foundation[target[0]].push_back(tempCard);
            tempCard = nullptr;
        }

        //check finish
        uint8_t size = 0;
        for (uint8_t i = 0; i < 4; i++) {
            if (foundation[i].size() > 0 && foundation[i].peek_back()->value == KING)
                size++;
        }

        if (size == 4) {
            buffer->clear();
            selectedCard = 0;
            DrawPlayScene();
            state = Finish;
            return;
        }
    } else {
        tempTime = 0;
        //search the lowest card
        int lowestSuit = -2, lowestValue = 13;
        for (int i = 0; i < 4; i++) {
            auto &fnd = foundation[i];
            if (foundation[i].size() == 0) {
                //find the missing suit
                int foundations[4] = {0, 0, 0, 0};
                for (int j = 0; j < 4; j++) {
                    if (foundation[j].size() > 0) {
                        foundations[foundation[j].peek_front()->suit] = 1;
                    }
                }
                for (int j = 0; j < 4; j++) {
                    if (foundations[j] == 0) {
                        lowestSuit = j;
                        lowestValue = -1;
                        target[0] = (float) j;
                        break;
                    }
                }
                break;
            }
            if (i == 0 || (lowestValue + 1) % 13 > (fnd.peek_back()->value + 1) % 13) {
                lowestSuit = fnd.peek_back()->suit;
                lowestValue = fnd.peek_back()->value;
                target[0] = (float) i;
            }
        }

        Card lowest(lowestSuit, lowestValue);

        //try to find it in tableau
        for (int i = 0; i < 7; i++) {
            auto &tbl = tableau[i];
            if (tbl.peek_back() && tbl.peek_back()->CanPlaceFoundation(&lowest)) {
                tempCard = tbl.pop_back();
                int y = MIN((int) tableau[i].size(), 4) * 4 + 25;
                tempPos.x = 2 + (float) i * 18;
                tempPos.y = (float) y;
                return;
            }
        }

        //try to find in waste
        int index = -1;
        for (auto *w: waste) {
            index++;
            if (w->CanPlaceFoundation(&lowest)) {
                tempCard = waste.extract(index);
                tempCard->exposed = true;
                tempPos.x = 20;
                tempPos.y = 1;
                return;
            }
        }

        index = -1;
        //try to find in stock
        for (auto *w: stock) {
            index++;
            if (w->CanPlaceFoundation(&lowest)) {
                tempCard = stock.extract(index);
                tempCard->exposed = true;
                tempPos.x = 2;
                tempPos.y = 1;
                return;
            }
        }
    }
}

void GameLogic::QuickSolve() {
    if (tempCard) {
        delete tempCard;
        tempCard = nullptr;
    }

    waste.deleteData();
    waste.clear();
    stock.deleteData();
    stock.clear();
    for (uint8_t i = 0; i < 7; i++) {
        tableau[i].deleteData();
        tableau[i].clear();
    }

    int foundations[4] = {0, 0, 0, 0};
    for (int j = 0; j < 4; j++) {
        if (foundation[j].size() > 0) {
            foundations[foundation[j].peek_front()->suit] = 1;
        }
    }

    for (int j = 0; j < 4; j++) {
        if (foundations[j] == 0) {
            //seed the foundation
            foundation[j].push_back(new Card(j, ACE));
            foundation[j].peek_back()->exposed = true;
        }
    }

    for (uint8_t i = 0; i < 4; i++) {
        auto &fnd = foundation[i];
        if (fnd.peek_back()->value == ACE) {
            fnd.push_back(new Card(fnd.peek_back()->suit, TWO));
            fnd.peek_back()->exposed = true;
        }

        while (fnd.peek_back()->value != KING) {
            fnd.push_back(new Card(fnd.peek_back()->suit, fnd.peek_back()->value + 1));
            fnd.peek_back()->exposed = true;
        }
    }
    buffer->clear();
    DrawPlayScene();
    selectedCard = 0;
    state = Finish;
}

void GameLogic::FallingCard(float delta) {
    UNUSED(delta);
    if (tempCard) {
        if ((furi_get_tick() - tempTime) > 12) {
            tempTime = furi_get_tick();
            tempPos.x += velocity.x;
            tempPos.y -= velocity.y;

            if (tempPos.y > 41) {
                velocity.y *= -0.8;
                tempPos.y = 41;
                PlayBounce();
            } else {
                velocity.y -= 1;
                if (velocity.y < -10) velocity.y = -10;
            }
            tempCard->Render((int8_t) tempPos.x, (int8_t) tempPos.y, false, buffer);
            if (tempPos.x < -18 || tempPos.x > 128) {
                delete tempCard;
                tempCard = nullptr;
            }
        }
    } else {
        float r1 = 2.0 * (float) (rand() % 2) - 1.0; // random number in range -1 to 1
        if (r1 == 0) r1 = 0.1;
        float r2 = inverse_tanh(r1);

        velocity.x = (float) (tanh(r2)) * (rand() % 3 + 1);

        if (velocity.x == 0) velocity.x = 1;
        velocity.y = (rand() % 3 + 1);
        if (foundation[selectedCard].size() > 0) {
            tempCard = foundation[selectedCard].pop_back();
            tempCard->exposed = true;
            tempPos.x = 56 + selectedCard * 18;
            tempPos.y = 2;
            selectedCard = (uint8_t) (selectedCard + 1) % 4;
        } else {
            state = Logo;
            return;
        }
    }
}


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
    NULL,
};

static const NotificationSequence sequence_bounce = {
    &message_vibro_on,
    &message_note_c4,
    &message_delay_10,
    &message_vibro_off,
    &message_sound_off,
    NULL,
};

void GameLogic::PlayError() {
    notification_message(notification, (const NotificationSequence *) &sequence_fail);
}

void GameLogic::PlayBounce() {
    notification_message(notification, (const NotificationSequence *) &sequence_bounce);
}
