#pragma once
#include <QVector>
#include <functional>

template<typename T>
void insertionSort(QVector<T>& arr,
    std::function<bool(const T&, const T&)> comp = [](const T& a, const T& b){ return a < b; },
    long long* moveCount = nullptr) {
    int n = arr.size();
    for (int i = 1; i < n; ++i) {
        T key = arr[i];
        int j = i - 1;
        while (j >= 0 && comp(key, arr[j])) {
            arr[j+1] = arr[j];
            if (moveCount) ++(*moveCount);
            --j;
        }
        arr[j+1] = key;
        if (moveCount) ++(*moveCount); // placing key counts as a move
    }
}
