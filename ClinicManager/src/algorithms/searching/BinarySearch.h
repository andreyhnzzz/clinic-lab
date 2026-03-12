#pragma once
#include <QVector>
#include <functional>

// Binary search on sorted array; compare(element, key) returns <0, 0, or >0
template<typename T, typename K>
int binarySearch(const QVector<T>& arr, const K& key,
                 std::function<int(const T&, const K&)> compare) {
    int lo = 0, hi = arr.size() - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        int cmp = compare(arr[mid], key);
        if (cmp == 0) return mid;
        else if (cmp < 0) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

// Range search on sorted array using lower/upper bound predicates
template<typename T>
QVector<T> binarySearchRange(const QVector<T>& arr,
                              std::function<bool(const T&)> lowerOk,
                              std::function<bool(const T&)> upperOk) {
    QVector<T> results;
    int lo = 0, hi = arr.size() - 1, start = arr.size();
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (lowerOk(arr[mid])) { start = mid; hi = mid - 1; }
        else lo = mid + 1;
    }
    for (int i = start; i < arr.size() && upperOk(arr[i]); ++i)
        results.push_back(arr[i]);
    return results;
}
