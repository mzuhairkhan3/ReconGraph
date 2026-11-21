#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>

namespace recongraph::dsa {

/**
 * Custom stack (LIFO) implemented with a linked list of nodes.
 * Primary use: DFS traversal and navigation history.
 */
template <typename T>
class Stack {
private:
    struct Node {
        T data;
        Node* next;
        explicit Node(const T& value) : data(value), next(nullptr) {}
        explicit Node(T&& value) : data(std::move(value)), next(nullptr) {}
    };

    Node* top_;
    std::size_t size_;

public:
    Stack() : top_(nullptr), size_(0) {}

    ~Stack() { clear(); }

    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    Stack(Stack&& other) noexcept : top_(other.top_), size_(other.size_) {
        other.top_ = nullptr;
        other.size_ = 0;
    }

    Stack& operator=(Stack&& other) noexcept {
        if (this != &other) {
            clear();
            top_ = other.top_;
            size_ = other.size_;
            other.top_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    void push(const T& value) {
        Node* node = new Node(value);
        node->next = top_;
        top_ = node;
        ++size_;
    }

    void push(T&& value) {
        Node* node = new Node(std::move(value));
        node->next = top_;
        top_ = node;
        ++size_;
    }

    void pop() {
        if (!top_) throw std::out_of_range("Stack::pop on empty stack");
        Node* temp = top_;
        top_ = top_->next;
        delete temp;
        --size_;
    }

    T& top() {
        if (!top_) throw std::out_of_range("Stack::top on empty stack");
        return top_->data;
    }

    const T& top() const {
        if (!top_) throw std::out_of_range("Stack::top on empty stack");
        return top_->data;
    }

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    void clear() {
        while (top_) {
            Node* temp = top_;
            top_ = top_->next;
            delete temp;
        }
        size_ = 0;
    }
};

} // namespace recongraph::dsa
