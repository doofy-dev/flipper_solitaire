#include "Deck.h"
#include "utils/Buffer.h"
#include "utils/RenderBuffer.h"

Deck::Deck(uint8_t count) : deck_count(count) {

}

void Deck::Generate() {
    waste_pile.clear();
    stock_pile.clear();
    //generate and shuffle deck
    int cards_count = 52 * deck_count;
    uint8_t cards[cards_count];
    for (int i = 0; i < cards_count; i++) cards[i] = i % 52;
    srand(DWT->CYCCNT);

    for (int i = 0; i < cards_count; i++) {
        int r = i + (rand() % (cards_count - i));
        uint8_t card = cards[i];
        cards[i] = cards[r];
        cards[r] = card;
    }

    //Init deck list
    for (int i = 0; i < 52; i++) {
        int letter = cards[i] % 13;
        int suit = cards[i] / 13;
        stock_pile.add(new Card(suit, letter));
    }
    FURI_LOG_D("Deck count", "%li", stock_pile.count);
}

void Deck::Cycle() {
    if (stock_pile.count > 0) {
        auto *c = stock_pile.pop();
        check_pointer(c);
        c->exposed = true;
        waste_pile.add(c);
    } else {
        while (waste_pile.count > 0) {
            auto *c = waste_pile.pop();
            check_pointer(c);
            c->exposed = false;
            stock_pile.add(c);
        }
    }
}

Card *Deck::GetLastWaste() {
    return waste_pile.pop();
}

void Deck::AddToWaste(Card *c) {
    check_pointer(c);
    waste_pile.add(c);
}

Card *Deck::Extract() {
    return stock_pile.pop();
}

void Deck::Render(RenderBuffer *buffer, bool stockpileSelect, bool wasteSelect) {
    if (stock_pile.count == 0) {
        Card::RenderEmptyCard(1, 1, buffer);
        if (stockpileSelect) {
            buffer->draw_rbox(2, 2, 17, 23, Flip);
        }
    } else {
        if (stock_pile.count > 1) {
            buffer->draw_rbox_frame(1, 1, 17, 23, Black);
            stock_pile.last()->Render(0, 0, stockpileSelect, buffer, 22);
        } else {
            stock_pile.last()->Render(1, 1, stockpileSelect, buffer, 22);
        }
    }

    if (waste_pile.count == 0) {
        Card::RenderEmptyCard(19, 1, buffer);
        if (wasteSelect) {
            buffer->draw_rbox(20, 2, 35, 23, Flip);
        }
    } else {
        waste_pile.last()->Render(19, 1, wasteSelect, buffer, 22);
    }
}
