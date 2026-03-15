#pragma once
#include <QVector>
#include <functional>

template<typename T>
void bubbleSort(QVector<T>& arr,
    std::function<bool(const T&, const T&)> comp = [](const T& a, const T& b){ return a < b; },
    long long* swapCount = nullptr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i)
        for (int j = 0; j < n - i - 1; ++j)
            if (comp(arr[j+1], arr[j])) {
                qSwap(arr[j], arr[j+1]);
                if (swapCount) ++(*swapCount);
            }
}
