#pragma once
#include <QVector>
#include <functional>

template<typename T>
void selectionSort(QVector<T>& arr,
    std::function<bool(const T&, const T&)> comp = [](const T& a, const T& b){ return a < b; }) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        int minIdx = i;
        for (int j = i + 1; j < n; ++j)
            if (comp(arr[j], arr[minIdx])) minIdx = j;
        if (minIdx != i) qSwap(arr[i], arr[minIdx]);
    }
}
