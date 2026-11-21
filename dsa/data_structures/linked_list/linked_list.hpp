#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>

namespace recongraph::dsa {

template <typename T>
class LinkedList {
private:
    struct Node {
        T data;
        Node* next;
        explicit Node(const T& value) : data(value), next(nullptr) {}
        explicit Node(T&& value) : data(std::move(value)), next(nullptr) {}
    };

    Node* head_;
    Node* tail_;
    std::size_t size_;

public:
    LinkedList() : head_(nullptr), tail_(nullptr), size_(0) {}

    ~LinkedList() { clear(); }

    LinkedList(const LinkedList& other) : head_(nullptr), tail_(nullptr), size_(0) {
        for (Node* curr = other.head_; curr; curr = curr->next) {
            push_back(curr->data);
        }
    }

    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            LinkedList temp(other);
            std::swap(head_, temp.head_);
            std::swap(tail_, temp.tail_);
            std::swap(size_, temp.size_);
        }
        return *this;
    }

    LinkedList(LinkedList&& other) noexcept
        : head_(other.head_), tail_(other.tail_), size_(other.size_) {
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    }

    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            head_ = other.head_;
            tail_ = other.tail_;
            size_ = other.size_;
            other.head_ = nullptr;
            other.tail_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    void push_front(const T& value) {
        Node* node = new Node(value);
        node->next = head_;
        head_ = node;
        if (!tail_) tail_ = head_;
        ++size_;
    }

    void push_back(const T& value) {
        Node* node = new Node(value);
        if (!tail_) {
            head_ = tail_ = node;
        } else {
            tail_->next = node;
            tail_ = node;
        }
        ++size_;
    }

    void push_back(T&& value) {
        Node* node = new Node(std::move(value));
        if (!tail_) {
            head_ = tail_ = node;
        } else {
            tail_->next = node;
            tail_ = node;
        }
        ++size_;
    }

    void pop_front() {
        if (!head_) throw std::out_of_range("LinkedList::pop_front on empty list");
        Node* temp = head_;
        head_ = head_->next;
        if (!head_) tail_ = nullptr;
        delete temp;
        --size_;
    }

    T& front() {
        if (!head_) throw std::out_of_range("LinkedList::front on empty list");
        return head_->data;
    }

    const T& front() const {
        if (!head_) throw std::out_of_range("LinkedList::front on empty list");
        return head_->data;
    }

    T& back() {
        if (!tail_) throw std::out_of_range("LinkedList::back on empty list");
        return tail_->data;
    }

    const T& back() const {
        if (!tail_) throw std::out_of_range("LinkedList::back on empty list");
        return tail_->data;
    }

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    void clear() {
        while (head_) {
            Node* temp = head_;
            head_ = head_->next;
            delete temp;
        }
        tail_ = nullptr;
        size_ = 0;
    }

    // Simple forward iterator
    class Iterator {
        Node* current_;
    public:
        explicit Iterator(Node* node) : current_(node) {}
        T& operator*() { return current_->data; }
        Iterator& operator++() { current_ = current_->next; return *this; }
        bool operator!=(const Iterator& other) const { return current_ != other.current_; }
    };

    Iterator begin() { return Iterator(head_); }
    Iterator end() { return Iterator(nullptr); }
};

} // namespace recongraph::dsa
