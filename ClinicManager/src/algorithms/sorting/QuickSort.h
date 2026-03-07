#pragma once
#include <QVector>
#include <functional>

namespace detail {
template<typename T>
int partition(QVector<T>& arr, int low, int high,
              std::function<bool(const T&, const T&)>& comp) {
    T pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; ++j)
        if (comp(arr[j], pivot)) { ++i; qSwap(arr[i], arr[j]); }
    qSwap(arr[i+1], arr[high]);
    return i + 1;
}

template<typename T>
void quickSortHelper(QVector<T>& arr, int low, int high,
                     std::function<bool(const T&, const T&)>& comp) {
    if (low < high) {
        int pi = partition(arr, low, high, comp);
        quickSortHelper(arr, low, pi - 1, comp);
        quickSortHelper(arr, pi + 1, high, comp);
    }
}
} // namespace detail

template<typename T>
void quickSort(QVector<T>& arr,
    std::function<bool(const T&, const T&)> comp = [](const T& a, const T& b){ return a < b; }) {
    if (arr.size() > 1)
        detail::quickSortHelper(arr, 0, arr.size() - 1, comp);
}
