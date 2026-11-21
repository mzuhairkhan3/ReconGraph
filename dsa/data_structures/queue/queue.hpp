#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>

namespace recongraph::dsa {

/**
 * Custom queue (FIFO) implemented with a linked list of nodes.
 * Primary use: BFS crawling and pending page processing.
 */
template <typename T>
class Queue {
private:
    struct Node {
        T data;
        Node* next;
        explicit Node(const T& value) : data(value), next(nullptr) {}
        explicit Node(T&& value) : data(std::move(value)), next(nullptr) {}
    };

    Node* front_;
    Node* back_;
    std::size_t size_;

public:
    Queue() : front_(nullptr), back_(nullptr), size_(0) {}

    ~Queue() { clear(); }

    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    Queue(Queue&& other) noexcept
        : front_(other.front_), back_(other.back_), size_(other.size_) {
        other.front_ = nullptr;
        other.back_ = nullptr;
        other.size_ = 0;
    }

    Queue& operator=(Queue&& other) noexcept {
        if (this != &other) {
            clear();
            front_ = other.front_;
            back_ = other.back_;
            size_ = other.size_;
            other.front_ = nullptr;
            other.back_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    void enqueue(const T& value) {
        Node* node = new Node(value);
        if (!back_) {
            front_ = back_ = node;
        } else {
            back_->next = node;
            back_ = node;
        }
        ++size_;
    }

    void enqueue(T&& value) {
        Node* node = new Node(std::move(value));
        if (!back_) {
            front_ = back_ = node;
        } else {
            back_->next = node;
            back_ = node;
        }
        ++size_;
    }

    void dequeue() {
        if (!front_) throw std::out_of_range("Queue::dequeue on empty queue");
        Node* temp = front_;
        front_ = front_->next;
        if (!front_) back_ = nullptr;
        delete temp;
        --size_;
    }

    T& front() {
        if (!front_) throw std::out_of_range("Queue::front on empty queue");
        return front_->data;
    }

    const T& front() const {
        if (!front_) throw std::out_of_range("Queue::front on empty queue");
        return front_->data;
    }

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    void clear() {
        while (front_) {
            Node* temp = front_;
            front_ = front_->next;
            delete temp;
        }
        back_ = nullptr;
        size_ = 0;
    }
};

} // namespace recongraph::dsa
