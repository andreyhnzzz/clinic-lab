#pragma once
#include <QVector>
#include <functional>

template<typename T>
int linearSearch(const QVector<T>& arr, std::function<bool(const T&)> predicate) {
    for (int i = 0; i < arr.size(); ++i)
        if (predicate(arr[i])) return i;
    return -1;
}

template<typename T>
QVector<T> linearSearchAll(const QVector<T>& arr, std::function<bool(const T&)> predicate) {
    QVector<T> results;
    for (const auto& item : arr)
        if (predicate(item)) results.push_back(item);
    return results;
}
