#pragma once

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace recongraph::dsa {

/**
 * Custom hash table with separate chaining for collision resolution.
 * Used primarily for URL / endpoint deduplication and fast lookup.
 */
template <typename K, typename V, typename Hash = std::hash<K>>
class HashTable {
private:
    struct Entry {
        K key;
        V value;
        Entry* next;

        Entry(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
        Entry(K&& k, V&& v) : key(std::move(k)), value(std::move(v)), next(nullptr) {}
    };

    std::vector<Entry*> buckets_;
    std::size_t size_;
    float max_load_factor_;
    Hash hasher_;

    std::size_t bucket_index(const K& key) const {
        return hasher_(key) % buckets_.size();
    }

    void rehash(std::size_t new_bucket_count) {
        std::vector<Entry*> old_buckets = std::move(buckets_);
        buckets_.assign(new_bucket_count, nullptr);
        size_ = 0;

        for (Entry* head : old_buckets) {
            while (head) {
                Entry* next = head->next;
                head->next = nullptr;
                insert_entry(head);
                head = next;
            }
        }
    }

    void insert_entry(Entry* entry) {
        std::size_t idx = bucket_index(entry->key);
        entry->next = buckets_[idx];
        buckets_[idx] = entry;
        ++size_;
    }

public:
    explicit HashTable(std::size_t initial_buckets = 16, float max_load = 0.75f)
        : buckets_(initial_buckets, nullptr),
          size_(0),
          max_load_factor_(max_load) {}

    ~HashTable() { clear(); }

    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    HashTable(HashTable&& other) noexcept
        : buckets_(std::move(other.buckets_)),
          size_(other.size_),
          max_load_factor_(other.max_load_factor_) {
        other.size_ = 0;
    }

    bool insert(const K& key, const V& value) {
        if (contains(key)) return false;

        if (static_cast<float>(size_ + 1) / buckets_.size() > max_load_factor_) {
            rehash(buckets_.size() * 2);
        }

        Entry* entry = new Entry(key, value);
        insert_entry(entry);
        return true;
    }

    bool insert(K&& key, V&& value) {
        if (contains(key)) return false;

        if (static_cast<float>(size_ + 1) / buckets_.size() > max_load_factor_) {
            rehash(buckets_.size() * 2);
        }

        Entry* entry = new Entry(std::move(key), std::move(value));
        insert_entry(entry);
        return true;
    }

    bool contains(const K& key) const {
        std::size_t idx = bucket_index(key);
        for (Entry* curr = buckets_[idx]; curr; curr = curr->next) {
            if (curr->key == key) return true;
        }
        return false;
    }

    V* find(const K& key) {
        std::size_t idx = bucket_index(key);
        for (Entry* curr = buckets_[idx]; curr; curr = curr->next) {
            if (curr->key == key) return &curr->value;
        }
        return nullptr;
    }

    const V* find(const K& key) const {
        std::size_t idx = bucket_index(key);
        for (Entry* curr = buckets_[idx]; curr; curr = curr->next) {
            if (curr->key == key) return &curr->value;
        }
        return nullptr;
    }

    bool erase(const K& key) {
        std::size_t idx = bucket_index(key);
        Entry* curr = buckets_[idx];
        Entry* prev = nullptr;

        while (curr) {
            if (curr->key == key) {
                if (prev) prev->next = curr->next;
                else buckets_[idx] = curr->next;
                delete curr;
                --size_;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    std::size_t bucket_count() const { return buckets_.size(); }

    void clear() {
        for (Entry*& head : buckets_) {
            while (head) {
                Entry* next = head->next;
                delete head;
                head = next;
            }
        }
        size_ = 0;
    }
};

// Convenience alias for string keys (most common in ReconGraph)
using StringHashTable = HashTable<std::string, std::size_t>;

} // namespace recongraph::dsa
