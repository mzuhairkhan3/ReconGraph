#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>

namespace recongraph::dsa {

template <typename T>
class DynamicArray {
public:
    DynamicArray() : data_(nullptr), size_(0), capacity_(0) {}

    explicit DynamicArray(std::size_t initial_capacity)
        : data_(nullptr), size_(0), capacity_(0) {
        if (initial_capacity > 0) {
            reserve(initial_capacity);
        }
    }

    ~DynamicArray() {
        delete[] data_;
    }

    // Copy
    DynamicArray(const DynamicArray& other)
        : data_(nullptr), size_(0), capacity_(0) {
        reserve(other.capacity_);
        for (std::size_t i = 0; i < other.size_; ++i) {
            data_[i] = other.data_[i];
        }
        size_ = other.size_;
    }

    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            DynamicArray temp(other);
            swap(temp);
        }
        return *this;
    }

    // Move
    DynamicArray(DynamicArray&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    void push_back(const T& value) {
        if (size_ >= capacity_) {
            reserve(capacity_ == 0 ? 4 : capacity_ * 2);
        }
        data_[size_++] = value;
    }

    void push_back(T&& value) {
        if (size_ >= capacity_) {
            reserve(capacity_ == 0 ? 4 : capacity_ * 2);
        }
        data_[size_++] = std::move(value);
    }

    void pop_back() {
        if (size_ == 0) throw std::out_of_range("DynamicArray::pop_back on empty array");
        --size_;
    }

    T& operator[](std::size_t index) {
        if (index >= size_) throw std::out_of_range("DynamicArray index out of range");
        return data_[index];
    }

    const T& operator[](std::size_t index) const {
        if (index >= size_) throw std::out_of_range("DynamicArray index out of range");
        return data_[index];
    }

    T& front() {
        if (size_ == 0) throw std::out_of_range("DynamicArray::front on empty array");
        return data_[0];
    }

    const T& front() const {
        if (size_ == 0) throw std::out_of_range("DynamicArray::front on empty array");
        return data_[0];
    }

    T& back() {
        if (size_ == 0) throw std::out_of_range("DynamicArray::back on empty array");
        return data_[size_ - 1];
    }

    const T& back() const {
        if (size_ == 0) throw std::out_of_range("DynamicArray::back on empty array");
        return data_[size_ - 1];
    }

    std::size_t size() const { return size_; }
    std::size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }

    void clear() { size_ = 0; }

    void reserve(std::size_t new_capacity) {
        if (new_capacity <= capacity_) return;
        T* new_data = new T[new_capacity];
        for (std::size_t i = 0; i < size_; ++i) {
            new_data[i] = std::move(data_[i]);
        }
        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }

    // Iterator support (basic)
    T* begin() { return data_; }
    T* end() { return data_ + size_; }
    const T* begin() const { return data_; }
    const T* end() const { return data_ + size_; }

private:
    T* data_;
    std::size_t size_;
    std::size_t capacity_;

    void swap(DynamicArray& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }
};

} // namespace recongraph::dsa
