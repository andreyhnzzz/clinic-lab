#pragma once
#include <QString>
#include <QVector>
#include <functional>
#include <optional>
#include <string>

template<typename K, typename V>
class HashTable {
    struct Entry {
        K key;
        V value;
        bool occupied = false;
        bool deleted = false;
    };

    static const int DEFAULT_SIZE = 16384;

public:
    explicit HashTable(int capacity = DEFAULT_SIZE)
        : table_(capacity), capacity_(capacity), size_(0) {}

    void insert(const K& key, const V& value) {
        if (size_ * 2 >= capacity_) resize();
        int idx = findSlot(key);
        if (!table_[idx].occupied || table_[idx].deleted) {
            table_[idx] = {key, value, true, false};
            ++size_;
        } else {
            table_[idx].value = value;
        }
    }

    std::optional<V> find(const K& key) const {
        int idx = probe(key);
        if (idx >= 0) return table_[idx].value;
        return std::nullopt;
    }

    bool contains(const K& key) const { return probe(key) >= 0; }
    int size() const { return size_; }
    void clear() { table_.fill(Entry{}); size_ = 0; }

    QVector<V> values() const {
        QVector<V> result;
        for (const auto& e : table_)
            if (e.occupied && !e.deleted)
                result.push_back(e.value);
        return result;
    }

private:
    QVector<Entry> table_;
    int capacity_;
    int size_;

    size_t hash(const K& key) const {
        return std::hash<K>{}(key);
    }

    int findSlot(const K& key) {
        int idx = static_cast<int>(hash(key) % static_cast<size_t>(capacity_));
        while (table_[idx].occupied && !table_[idx].deleted && table_[idx].key != key) {
            idx = (idx + 1) % capacity_;
        }
        return idx;
    }

    int probe(const K& key) const {
        int idx = static_cast<int>(hash(key) % static_cast<size_t>(capacity_));
        int count = 0;
        while (count < capacity_) {
            if (!table_[idx].occupied) return -1;
            if (!table_[idx].deleted && table_[idx].key == key) return idx;
            idx = (idx + 1) % capacity_;
            ++count;
        }
        return -1;
    }

    void resize() {
        QVector<Entry> old = std::move(table_);
        capacity_ *= 2;
        table_ = QVector<Entry>(capacity_);
        size_ = 0;
        for (auto& e : old) {
            if (e.occupied && !e.deleted) insert(e.key, e.value);
        }
    }
};
