#include "Deck.h"
#include "utils/Buffer.h"
#include "utils/RenderBuffer.h"

Deck::Deck(uint8_t count) : deck_count(count) {

}

void Deck::Generate() {
    waste_pile.empty();
    stock_pile.empty();
    //generate and shuffle deck
    uint8_t cards[52];
    for (int i = 0; i < 52; i++) cards[i] = i;
    srand(DWT->CYCCNT);
    for (int i = 0; i < 52; i++) {
        int r = i + (rand() % (52 - i));
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
}

void Deck::Cycle() {
    if (stock_pile.count > 0) {
        auto *c = stock_pile.pop();
        c->exposed= true;
        waste_pile.add(c);
    }
    else {
        while (waste_pile.count > 0) {
            auto *c = waste_pile.pop();
            c->exposed= false;
            stock_pile.add(c);
        }
    }
}

Card *Deck::GetLastWaste() {
    return waste_pile.pop();
}

void Deck::AddToWaste(Card *c) {
    waste_pile.add(c);
}

Card *Deck::Extract() {
    return stock_pile.pop();
}

void Deck::Render(RenderBuffer *buffer) {
    if (stock_pile.count == 0)
        Card::RenderEmptyCard(1, 1, buffer);
    else {
        if(stock_pile.count>1) {
            buffer->draw_rbox_frame(1, 1, 17, 23, Black);
            stock_pile.last()->Render(0, 0, false, buffer);
        }else{
            stock_pile.last()->Render(1, 1, false, buffer);
        }
    }

    if (waste_pile.count == 0) {
        FURI_LOG_D("DECK", "Rendering empty card");
        Card::RenderEmptyCard(19, 1, buffer);
    }
    else
        waste_pile.last()->Render(19, 1, false, buffer);
}
