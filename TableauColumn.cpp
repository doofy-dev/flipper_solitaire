#include "TableauColumn.h"
#include "utils/RenderBuffer.h"

#define max(a, b) a>b?a:b

void TableauColumn::Reset() {
    cards->clear();
}

void TableauColumn::AddCard(Card *c) {
    check_pointer(c);
    cards->add(c);
}

void TableauColumn::AddRange(List<Card> *hand) {
    check_pointer(hand);
    for (auto *item: *hand) {
        check_pointer(item);
        cards->add(item);
    }
}

void TableauColumn::Render(uint8_t x, uint8_t y, bool selected, uint8_t selection_from_end, RenderBuffer *buffer) {
    // If the hand is empty
    if (cards->count == 0) {
        Card::RenderEmptyCard(x, y, buffer);
        if (selected)
            buffer->draw_rbox(x+1, y+1, x + 16, y + 22, Flip);
        return;
    }
    uint8_t selection = cards->count - selection_from_end;
    if (selected) selection--;

    uint8_t loop_end = cards->count;
    uint8_t loop_start = max(loop_end - 4, 0);
    uint8_t position = 0;
    uint8_t first_non_flipped = FirstNonFlipped();
    bool had_top = false;
    check_pointer(cards);

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
        check_pointer((*cards)[first_non_flipped]);
        (*cards)[first_non_flipped]->Render(x, y + position, false, buffer,
                                            cards->count == 1 ? 22 : 9);
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
        check_pointer((*cards)[selection]);
        // Draw the front side of the selected card
        (*cards)[selection]->Render(x, y + position, true, buffer, 9);
        position += 8;
        loop_start++; // Increment loop start index
    }

    //Draw the rest
    for (uint8_t i = loop_start; i < loop_end; i++, position += 4) {
        int height = 5;
        if((i + 1) == loop_end) height = 22;
        else if(i == selection || i == first_non_flipped) height = 9;
        check_pointer((*cards)[i]);
        if((*cards)[i]) {
            (*cards)[i]->Render(x, y + position, i == selection, buffer, height);
            if (i == selection || i == first_non_flipped) position += 4;
        }
    }
}

int8_t TableauColumn::FirstNonFlipped() {
    int8_t index = -1;
    if (cards->count > 0) {
        for (auto *card: *cards) {
            index++;
            if (card && card->exposed) {
                break;
            }
        }
    }
    return index;
}

TableauColumn::TableauColumn() {
    cards = new List<Card>();
}

TableauColumn::~TableauColumn() {
    delete cards;
}

void TableauColumn::Merge(TableauColumn *other) {
    Card *prev = nullptr;
    for (auto *item: *(other->cards)) {
        if(prev == item) {
            FURI_LOG_E("PLACE", "Possible circular dependency!");
            break;
        }
        FURI_LOG_D("PLACE", "ADDING %i %i", item->suit, item->value);
        if(!item){
            FURI_LOG_E("PLACE", "nullptr in merge!");
            break;
        }
        cards->add(item);
    }
    FURI_LOG_D("PLACE", "clear");
    other->cards->soft_clear();
}

Card *TableauColumn::TopCard() {
    return (*cards)[0];
}

uint8_t TableauColumn::Count() {
    return cards->count;
}

Card *TableauColumn::Pop() {
    return cards->pop();
}

void TableauColumn::Reveal() {
    (*cards)[cards->count - 1]->exposed = true;
}

Card *TableauColumn::LastCard() {
    if (cards->count > 0) {
        return (*cards)[cards->count - 1];
    }
    return nullptr;
}

void TableauColumn::Clear() {
    cards->soft_clear();
}

bool TableauColumn::CanPlace(TableauColumn *other) {
    if (cards->count == 0) {
        if(other->TopCard()->value == 11) FURI_LOG_D("TBLC", "placing first");
        return other->TopCard()->value == 11;
    }
    Card *last = LastCard();
    Card *top = other->TopCard();
    int current_suit = last->suit / 2;
    int other_suit = top->suit / 2;
    if((current_suit + 1) % 2 == other_suit && (last->value + 1) % 13 == (top->value + 2) % 13) FURI_LOG_D("TBLC", "Adding at end");
    else
        FURI_LOG_D("TBLC", "CANT ADD, suit check %i, label check %i", (current_suit + 1) % 2 == other_suit, (last->value + 1) % 13 == (top->value + 2) % 13);

    return (current_suit + 1) % 2 == other_suit && (last->value + 1) % 13 == (top->value + 2) % 13;
}

List<Card> *TableauColumn::splice(uint32_t selection) {
    FURI_LOG_D("TBLC", "%li,  %li", cards->count-selection, selection);
    return cards->splice(cards->count-selection-1, selection+1);
}
