#pragma once

#include <input/input.h>
#include "List.h"

class InputEventHandler {
    struct Subscription {
        void *ctx;
        size_t id;

        void (*callback)(void *ctx, int, InputType);
    };

    List<Subscription> events;
    size_t _id = 0;
public:
    InputEventHandler() {
        events = List<Subscription>();
    }

    ~InputEventHandler() {
        events.deleteData();
        events.clear();
    }

    int subscribe(void *caller, void(*p)(void *, int, InputType)) {
        size_t currID=_id;
        _id++;
        events.push_back(new Subscription{.ctx = caller, .id=currID, .callback=p});
        return currID;
    }

    void unsubscribe(size_t id) {
        for(auto item : events){
            if (item->id == id) {
                events.remove(item);
                return;
            }
        }
    }

    void Set(int key, InputType type) {
        for (auto *evt: events) {
            evt->callback(evt->ctx, key, type);
        }
    }
};