#include "PerformanceMeter.h"
#include "DataGenerator.h"
#include "../algorithms/sorting/BubbleSort.h"
#include "../algorithms/sorting/SelectionSort.h"
#include "../algorithms/sorting/InsertionSort.h"
#include "../algorithms/sorting/QuickSort.h"
#include "../algorithms/searching/LinearSearch.h"
#include "../algorithms/searching/BinarySearch.h"
#include "../models/Paciente.h"
#include <algorithm>
#include <random>

static std::mt19937& rngPM() {
    static std::mt19937 gen(std::random_device{}());
    return gen;
}

void PerformanceMeter::start() {
    running_ = true;
    startTime_ = std::chrono::high_resolution_clock::now();
}

double PerformanceMeter::stop() {
    endTime_ = std::chrono::high_resolution_clock::now();
    running_ = false;
    return std::chrono::duration<double, std::milli>(endTime_ - startTime_).count();
}

double PerformanceMeter::elapsed() const {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(now - startTime_).count();
}

QVector<BenchmarkResult> PerformanceMeter::benchmarkSorts(int dataSize) {
    QVector<BenchmarkResult> results;
    PerformanceMeter meter;

    // Generate random int array once
    QVector<int> base;
    base.reserve(dataSize);
    std::uniform_int_distribution<int> dist(0, 999999);
    for (int i = 0; i < dataSize; ++i) base.push_back(dist(rngPM()));

    auto bench = [&](const QString& name, auto sortFn) -> BenchmarkResult {
        QVector<int> copy = base;
        meter.start();
        sortFn(copy);
        double ms = meter.stop();
        BenchmarkResult r;
        r.algorithmName = name;
        r.dataSize = dataSize;
        r.timeMs = ms;
        r.dataType = "int";
        return r;
    };

    bool slow = (dataSize > 10000);

    if (!slow) {
        results.push_back(bench("Bubble Sort", [](QVector<int>& arr){
            bubbleSort<int>(arr);
        }));
        results.push_back(bench("Selection Sort", [](QVector<int>& arr){
            selectionSort<int>(arr);
        }));
        results.push_back(bench("Insertion Sort", [](QVector<int>& arr){
            insertionSort<int>(arr);
        }));
    } else {
        // For large datasets, only run quick sort and std::sort; estimate others
        BenchmarkResult skipped;
        skipped.dataSize = dataSize;
        skipped.dataType = "int (omitido - muy lento)";
        skipped.timeMs = -1.0;

        skipped.algorithmName = "Bubble Sort";
        results.push_back(skipped);
        skipped.algorithmName = "Selection Sort";
        results.push_back(skipped);
        skipped.algorithmName = "Insertion Sort";
        results.push_back(skipped);
    }

    results.push_back(bench("Quick Sort", [](QVector<int>& arr){
        quickSort<int>(arr);
    }));

    {
        QVector<int> copy = base;
        meter.start();
        std::sort(copy.begin(), copy.end());
        double ms = meter.stop();
        BenchmarkResult r;
        r.algorithmName = "std::sort";
        r.dataSize = dataSize;
        r.timeMs = ms;
        r.dataType = "int";
        results.push_back(r);
    }

    return results;
}

QPair<double, double> PerformanceMeter::benchmarkSearchComparison(int dataSize) {
    QVector<int> arr;
    arr.reserve(dataSize);
    for (int i = 0; i < dataSize; ++i) arr.push_back(i * 2); // sorted even numbers

    int target = (dataSize > 0) ? arr[dataSize / 2] : 0;

    PerformanceMeter meter;

    // Linear search
    meter.start();
    for (int trial = 0; trial < 1000; ++trial) {
        linearSearch<int>(arr, [&](const int& v){ return v == target; });
    }
    double linearMs = meter.stop() / 1000.0;

    // Binary search
    meter.start();
    for (int trial = 0; trial < 1000; ++trial) {
        binarySearch<int, int>(arr, target, [](const int& elem, const int& key) -> int {
            return elem - key;
        });
    }
    double binaryMs = meter.stop() / 1000.0;

    return {linearMs, binaryMs};
}
