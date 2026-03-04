#pragma once

#include <QVector>
#include <functional>
#include <stdexcept>

// Generic min-heap based priority queue (no Qt inheritance - template classes
// cannot use Q_OBJECT).
// Template parameter T must support operator< and operator>.
// For Patient, lower numeric priority value = higher urgency (1=critical, 3=normal).

template<typename T>
class PriorityQueue
{
public:
    PriorityQueue() = default;

    // Insert element - O(log n)
    void enqueue(const T &item)
    {
        m_heap.append(item);
        siftUp(m_heap.size() - 1);
    }

    // Remove and return top (highest-priority) element - O(log n)
    T dequeue()
    {
        if (m_heap.isEmpty())
            throw std::underflow_error("PriorityQueue is empty");

        T top = m_heap.first();
        int last = m_heap.size() - 1;
        m_heap[0] = m_heap[last];
        m_heap.removeLast();
        if (!m_heap.isEmpty()) siftDown(0);
        return top;
    }

    // Peek at top without removing - O(1)
    const T &peek() const
    {
        if (m_heap.isEmpty())
            throw std::underflow_error("PriorityQueue is empty");
        return m_heap.first();
    }

    bool isEmpty() const { return m_heap.isEmpty(); }
    int  size()    const { return m_heap.size(); }

    // Access underlying heap array (for display purposes)
    const QVector<T> &data() const { return m_heap; }

    // Build heap from an existing collection - O(n)
    void buildFromVector(const QVector<T> &items)
    {
        m_heap = items;
        for (int i = m_heap.size() / 2 - 1; i >= 0; --i)
            siftDown(i);
    }

    void clear() { m_heap.clear(); }

private:
    QVector<T> m_heap;

    int parent(int i) const { return (i - 1) / 2; }
    int left(int i)   const { return 2 * i + 1; }
    int right(int i)  const { return 2 * i + 2; }

    void siftUp(int i)
    {
        while (i > 0 && m_heap[parent(i)] > m_heap[i]) {
            qSwap(m_heap[parent(i)], m_heap[i]);
            i = parent(i);
        }
    }

    void siftDown(int i)
    {
        int n = m_heap.size();
        while (true) {
            int smallest = i;
            int l = left(i), r = right(i);
            if (l < n && m_heap[l] < m_heap[smallest]) smallest = l;
            if (r < n && m_heap[r] < m_heap[smallest]) smallest = r;
            if (smallest == i) break;
            qSwap(m_heap[i], m_heap[smallest]);
            i = smallest;
        }
    }
};

