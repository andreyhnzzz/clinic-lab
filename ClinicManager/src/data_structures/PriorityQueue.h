#pragma once
#include <QVector>
#include <functional>

template<typename T>
class PriorityQueue {
public:
    using Comparator = std::function<bool(const T&, const T&)>;

    PriorityQueue() : comp_([](const T& a, const T& b){ return a < b; }) {}
    explicit PriorityQueue(Comparator comp) : comp_(comp) {}

    void push(const T& item) {
        heap_.push_back(item);
        siftUp(heap_.size() - 1);
    }

    T pop() {
        T top = heap_.front();
        heap_[0] = heap_.back();
        heap_.pop_back();
        if (!heap_.isEmpty()) siftDown(0);
        return top;
    }

    const T& peek() const { return heap_.front(); }
    bool isEmpty() const { return heap_.isEmpty(); }
    int size() const { return heap_.size(); }
    const QVector<T>& data() const { return heap_; }
    void clear() { heap_.clear(); }

private:
    QVector<T> heap_;
    Comparator comp_;

    void siftUp(int idx) {
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (comp_(heap_[idx], heap_[parent])) {
                qSwap(heap_[idx], heap_[parent]);
                idx = parent;
            } else break;
        }
    }

    void siftDown(int idx) {
        int n = heap_.size();
        while (true) {
            int smallest = idx;
            int left = 2*idx+1, right = 2*idx+2;
            if (left < n && comp_(heap_[left], heap_[smallest])) smallest = left;
            if (right < n && comp_(heap_[right], heap_[smallest])) smallest = right;
            if (smallest == idx) break;
            qSwap(heap_[idx], heap_[smallest]);
            idx = smallest;
        }
    }
};
