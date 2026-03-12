#pragma once
#include <QVector>
#include <functional>
<<<<<<< HEAD
#include <random>

namespace detail {

// Median-of-three pivot selection to avoid worst-case on sorted/reverse input
template<typename T>
int medianOfThree(QVector<T>& arr, int low, int high,
                  std::function<bool(const T&, const T&)>& comp) {
    int mid = low + (high - low) / 2;
    if (comp(arr[mid], arr[low])) qSwap(arr[low], arr[mid]);
    if (comp(arr[high], arr[low])) qSwap(arr[low], arr[high]);
    if (comp(arr[high], arr[mid])) qSwap(arr[mid], arr[high]);
    // Median is now at mid; move it to high-1 for partitioning
    qSwap(arr[mid], arr[high]);
    return high;
}

template<typename T>
int partition(QVector<T>& arr, int low, int high,
              std::function<bool(const T&, const T&)>& comp) {
    if (high - low >= 3) {
        medianOfThree(arr, low, high, comp);
    }
=======

namespace detail {
template<typename T>
int partition(QVector<T>& arr, int low, int high,
              std::function<bool(const T&, const T&)>& comp) {
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    T pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; ++j)
        if (comp(arr[j], pivot)) { ++i; qSwap(arr[i], arr[j]); }
    qSwap(arr[i+1], arr[high]);
    return i + 1;
}

<<<<<<< HEAD
// Insertion sort for small subarrays (cutoff optimization)
template<typename T>
void insertionSortRange(QVector<T>& arr, int low, int high,
                        std::function<bool(const T&, const T&)>& comp) {
    for (int i = low + 1; i <= high; ++i) {
        T key = arr[i];
        int j = i - 1;
        while (j >= low && comp(key, arr[j])) {
            arr[j+1] = arr[j];
            --j;
        }
        arr[j+1] = key;
    }
}

template<typename T>
void quickSortHelper(QVector<T>& arr, int low, int high,
                     std::function<bool(const T&, const T&)>& comp) {
    // Cutoff to insertion sort for small subarrays: reduces overhead of recursive
    // calls and partitioning; empirically optimal in the range 10-20 for cache efficiency.
    if (high - low < 16) {
        if (low < high) insertionSortRange(arr, low, high, comp);
        return;
    }
    int pi = partition(arr, low, high, comp);
    quickSortHelper(arr, low, pi - 1, comp);
    quickSortHelper(arr, pi + 1, high, comp);
=======
template<typename T>
void quickSortHelper(QVector<T>& arr, int low, int high,
                     std::function<bool(const T&, const T&)>& comp) {
    if (low < high) {
        int pi = partition(arr, low, high, comp);
        quickSortHelper(arr, low, pi - 1, comp);
        quickSortHelper(arr, pi + 1, high, comp);
    }
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
}
} // namespace detail

template<typename T>
void quickSort(QVector<T>& arr,
    std::function<bool(const T&, const T&)> comp = [](const T& a, const T& b){ return a < b; }) {
    if (arr.size() > 1)
        detail::quickSortHelper(arr, 0, arr.size() - 1, comp);
}
