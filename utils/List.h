#pragma once

#include <furi.h>
#include <cstdio>
#include "Helpers.h"

template<typename T>
class List {
private:
    struct Node {
        T *data;
        Node *next;
        Node *prev;
    };
    size_t count = 0;
    Node *head;
    Node *tail;

    class Iterator {
        Node *node;

    public:
        explicit Iterator(Node *node) : node(node) {}

        T *operator*() {
            return node->data;
        }

        Iterator &operator++() {
            if (node) {
                node = node->next;
            }
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp(*this);
            operator++();
            return tmp;
        }

        Iterator &operator--() {
            if (node) {
                node = node->prev;
            }
            return *this;
        }

        Iterator operator--(int) {
            Iterator tmp(*this);
            operator--();
            return tmp;
        }

        bool operator==(const Iterator &rhs) const {
            return node == rhs.node;
        }

        bool operator!=(const Iterator &rhs) const {
            return node != rhs.node;
        }
    };

public:
    explicit List() : head(nullptr), tail(nullptr) {}

    Iterator begin() const {
        return Iterator(head);
    }

    Iterator end() const {
        return Iterator(nullptr);
    }

    Iterator last() const {
        return Iterator(tail);
    }

    ~List() {
        clear();
    }

    void clear() {
        while (head) {
            Node *toDelete = head;
            head = head->next;
            delete toDelete;
        }
        tail = nullptr;
        count = 0;
    }

    void deleteData() {
        Node *current = head;
        while (current) {
            delete current->data;
            current = current->next;
        }
    }

    void remove(T *value) {
        Node *current = head;
        while (current) {
            if (current->data == value) {
                if (current->prev)
                    current->prev->next = current->next;
                else
                    head = current->next;

                if (current->next)
                    current->next->prev = current->prev;
                else
                    tail = current->prev;

                delete current;
                --count;
            }
            current = current->next;
        }
    }

    void push_back(T *value) {
        Node *newNode = new Node{value, nullptr, tail};
        if (tail)
            tail->next = newNode;
        else
            head = newNode;

        tail = newNode;
        ++count;
    }

    void push_front(T *value) {
        Node *newNode = new Node{value, head, nullptr};
        if (head)
            head->prev = newNode;
        else
            tail = newNode;

        head = newNode;
        ++count;
    }

    T *pop_back() {
        if (count == 0)
            return nullptr;

        Node *toDelete = tail;
        tail = tail->prev;
        if (tail)
            tail->next = nullptr;
        else
            head = nullptr;

        T *data = toDelete->data;
        delete toDelete;
        --count;
        return data;
    }

    T *pop_front() {
        if (count == 0)
            return nullptr;

        Node *toDelete = head; // Change this to head instead of tail
        head = head->next;
        if (head)
            head->prev = nullptr;
        else
            tail = nullptr;  // Setting tail to nullptr if list is now empty
        T *data = toDelete->data;
        delete toDelete;  // Deleting the old head of the list
        --count;
        return data;  // return the data
    }

    void unset(size_t index) {
        Node *current = head;
        for (size_t i = 0; i != index && current; ++i) {
            current = current->next;
        }
        if (current) {
            if (current->prev)
                current->prev->next = current->next;
            else
                head = current->next;

            if (current->next)
                current->next->prev = current->prev;
            else
                tail = current->prev;

            delete current;
            --count;
        }
    }

    List<T> *splice(size_t index, size_t numItems) {
        if (index >= count || numItems == 0 || numItems > (count - index)) {
            FURI_LOG_E("LIST", "Invalid index or number of items.");
            return nullptr;
        }

        List<T> *new_list = new List<T>();
        Node *current = head;  //It will point to node to remove.
        Node *prev_node = nullptr; //It will point to node prior to node to remove.

        //Positioning current and previous node pointers to right nodes.
        for (size_t i = 0; i < index; ++i) {
            prev_node = current;
            current = current->next;
        }

        int i = 0;
        Node *last_in_segment = current;
        while (i < numItems - 1 && last_in_segment->next) {
            last_in_segment = last_in_segment->next;
            i++;
        }

        //Avoiding over-splicing if numItems is larger than available items from index
        numItems = i + 1;

        //Update original list new head and tail after splicing
        if (prev_node) {
            prev_node->next = last_in_segment->next;
        } else {
            head = last_in_segment->next;
        }

        if (last_in_segment->next) {
            last_in_segment->next->prev = prev_node;
        } else {
            tail = prev_node;
        }

        //Update new list head and tail
        new_list->head = current;
        new_list->tail = last_in_segment;

        //Update new and original list counts
        new_list->count = numItems;
        count -= numItems;

        last_in_segment->next = nullptr;
        current->prev = nullptr;

        return new_list;
    }

    T *operator[](size_t index) {
        Node *current = head;
        for (size_t i = 0; i != index && current; ++i)
            current = current->next;

        return current ? current->data : nullptr;
    }

    const T *operator[](size_t index) const {
        Node *current = head;
        for (size_t i = 0; i != index && current; ++i)
            current = current->next;

        return current->data;
    }

    T *extract(size_t index) {
        if (index > size()) {
            FURI_LOG_E("List", "Out of range");
            return nullptr;
        }

        Node *current = head;
        for (size_t i = 0; i != index && current; ++i)
            current = current->next;

        if (current->prev)
            current->prev->next = current->next;
        else
            head = current->next;
        if (current->next)
            current->next->prev = current->prev;
        else
            tail = current->prev;

        T *data = current->data;
        delete current;
        count--;
        return data;
    }

    T *peek_front() const {
        if (head) return head->data;
        else return nullptr;
    }

    T *peek_back() const {
        if (tail) return tail->data;
        else return nullptr;
    }

    size_t size() const {
        return count;
    }
};