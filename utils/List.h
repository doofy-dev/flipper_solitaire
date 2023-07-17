#pragma once

#include <furi.h>
#include <iterator>
#include "Helpers.h"

template<typename T>
struct ListItem {
    ListItem *next;
    T *data;
};

template<typename T>
struct ListIterator {
    using iterator_category = std::forward_iterator_tag;

    ListItem<T> *current;

    explicit ListIterator(ListItem<T> *node) : current(node) {}

    ListIterator &operator++() {
        current = current->next;
        return *this;
    }

    ListIterator operator++(int) {
        ListIterator iterator = *this;
        ++(*this);
        return iterator;
    }

    bool operator==(const ListIterator &other) const {
        return current == other.current;
    }

    bool operator!=(const ListIterator &other) const {
        return current != other.current;
    }

    T *operator*() const {
        return (current->data);
    }

    T *operator->() const {
        return current->data;
    }
};


template<typename T>
struct List {
    uint32_t count;
    ListItem<T> *start = nullptr;

    List() : count(0) {}

    ~List() {
        FURI_LOG_D("App", "List emptied");

        empty();
    }

    void soft_clear() {
        auto *item = start;
        ListItem<T> *t;
        while (item) {
            t = item;
            item = item->next;
            delete t;
        }
        count = 0;
    }

    void clear() {
        auto *item = start;
        ListItem<T> *t;
        while (item) {
            t = item;
            item = item->next;
            if (t->data) {
                check_pointer(t->data);
                delete t->data;
            }
            check_pointer(t);
            delete t;
        }
        count = 0;
    }

    void empty() {
        clear();
        if (start) {
            check_pointer(start);
            delete start;
        }
    }

    void add(T *data) {
        count++;
        if (count > 1) {
            ListItem<T> *c = start;
            while (c->next) {
                c = c->next;
            }
            c->next = new ListItem<T>();
            c->next->data = data;
        } else {
            start = new ListItem<T>();
            start->data = data;
        }
    }

    void remove(T *data) {
        if (!start || !data) return;

        ListItem<T> *s = start;
        if (s->data == data) {
            check_pointer(s->data);
            delete s->data;
            start = start->next;
            count--;
        } else {
            while (s) {
                if (s->next && s->next->data == data) {
                    auto n = s->next->next;
                    check_pointer(s->next->data);
                    check_pointer(s->next);
                    delete s->next->data;
                    delete s->next;
                    s->next = n;
                    count--;
                    return;
                }

                s = s->next;
            }
        }
    }

    void soft_remove(T *data) {
        if (!start || !data) return;

        ListItem<T> *s = start;
        if (s->data == data) {
            auto tmp = start;
            start = start->next;
            delete tmp;
            count--;
        } else {
            while (s) {
                if (s->next && s->next->data == data) {
                    auto n = s->next->next;
                    check_pointer(s->next);
                    delete s->next;
                    s->next = n;
                    count--;
                    return;
                }

                s = s->next;
            }
        }
    }

    void remove(uint32_t index, uint32_t amount) {
        auto *result = splice(index, amount);
        delete result;
    }

    List<T> *splice(uint32_t index, uint32_t amount) {
        auto *removedElements = new List<T>();

        if (index < count) {
            uint32_t m = (index + amount) > count ? count - index : amount;
            uint32_t curr_id = 0;
            auto s = start;
            while (curr_id < index) {
                s = s->next;
                if (!s) return removedElements;
                curr_id++;
            }

            ListItem<T> *t;
            for (uint32_t i = 0; i < m; i++) {
                t = s->next;
                if (s->data) {
                    removedElements->add(s->data);
                }
                delete s;
                s = t->next;
                count--;
            }
            if (index == 0) {
                start = s;
            }
        }

        return removedElements;
    }

    T *pop_front() {
        if (!start) {
            // List is empty, nothing to remove
            return nullptr;
        }

        ListItem<T> *front = start;
        start = start->next; // Update the start pointer to the next element

        T *data = front->data; // Store the data of the front element
        delete front; // Delete the front element
        count--;
        return data; // Return the data of the removed element
    }

    T *last() {
        if (!start) {
            return nullptr;
        }

        if (!start->next) {
            return start->data;
        }

        ListItem<T> *current = start;
        while (current->next) {
            current = current->next;
        }

        return current->data; // Return the data
    }

    T *pop() {
        if (!start) {
            // List is empty, nothing to remove
            return nullptr;
        }

        if (!start->next) {
            // Only one element in the list
            T *data = start->data;
            delete start;
            start = nullptr;
            count--;
            return data;
        }

        ListItem<T> *previous = nullptr;
        ListItem<T> *current = start;

        while (current->next) {
            previous = current;
            current = current->next;
        }

        previous->next = nullptr; // Remove the last element from the list
        T *data = current->data; // Store the data of the last element
        count--;

        delete current; // Delete the last element
        return data; // Return the data of the removed element
    }

    ListIterator<T> begin() {
        return ListIterator<T>(start);
    }

    ListIterator<T> end() {
        return ListIterator<T>(nullptr);
    }

    T *operator[](int i) {
        int index = 0;
        auto *item = start;
        while (item) {
            if (index == i) return item->data;

            item = item->next;
            index++;
        }

        return nullptr;
    }
};