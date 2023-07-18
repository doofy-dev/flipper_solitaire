#include "TableauColumn.h"
#include "utils/RenderBuffer.h"

#define max(a, b) a>b?a:b

void TableauColumn::Reset() {
    cards->empty();
}

void TableauColumn::AddCard(Card *c) {
    cards->add(c);
}

void TableauColumn::AddRange(List<Card> *hand) {
    for (auto *item: *hand) {
        cards->add(item);
    }
}

void TableauColumn::Render(uint8_t x, uint8_t y, bool selected, uint8_t selection_from_end, RenderBuffer *buffer) {
    // If the hand is empty
    if (cards->count == 0) {
        Card::RenderEmptyCard(x, y, buffer);
        if (selected)
            buffer->draw_rbox(x, y, x + 16, y + 22, Flip);
        return;
    }
    uint8_t selection = cards->count - selection_from_end;
    if (selected) selection--;

    uint8_t loop_end = cards->count;
    uint8_t loop_start = max(loop_end - 4, 0);
    uint8_t position = 0;
    uint8_t first_non_flipped = FirstNonFlipped();
    bool had_top = false;

    // Draw the first flipped and non-flipped card with adjusted visibility
    if (first_non_flipped <= loop_start && selection != first_non_flipped) {
        // Draw a card back if it is not the first card
        if (first_non_flipped > 0) {
            Card::RenderBack(x, y + position, selection == first_non_flipped, buffer, 4);
            // Increment loop start index and position
            position += 4;
            loop_start++;
            had_top = true;
        }
        // Draw the front side of the first non-flipped card
        (*cards)[first_non_flipped]->Render(x, y + position, selection == first_non_flipped, buffer, cards->count == 1 ? 22 : 8);
        position += 8;
        loop_start++; // Increment loop start index
    }

    // Draw the selected card with adjusted visibility
    if (loop_start > selection) {
        if (!had_top && first_non_flipped > 0) {
            Card::RenderBack(x, y + position, selection == first_non_flipped, buffer,4);
            position += 2;
            loop_start++;
        }
        // Draw the front side of the selected card
        (*cards)[selection]->Render(x, y + position, true, buffer, 8);
        position += 8;
        loop_start++; // Increment loop start index
    }

    //Draw the rest
    for (uint8_t i = loop_start; i < loop_end; i++, position += 4) {
        (*cards)[i]->Render(x, y + position, i == selection, buffer, (i+1)==loop_end ? 22 : 4);

        if (i == selection || i == first_non_flipped) position += 4;
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

void TableauColumn::AddTo(TableauColumn *other) {
    for (auto *item: *(other->cards)) {
        cards->add(item);
    }
}

Card *TableauColumn::TopCard() {
    return (*cards)[0];
}

uint8_t TableauColumn::Count() {
    return cards->count;
}

List<Card>* TableauColumn::ExtractEnd(uint8_t count) {
    return cards->splice(cards->count - count, count);
}
