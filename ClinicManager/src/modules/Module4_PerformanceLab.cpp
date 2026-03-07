#include "Module4_PerformanceLab.h"
#include "../algorithms/sorting/BubbleSort.h"
#include "../algorithms/sorting/SelectionSort.h"
#include "../algorithms/sorting/InsertionSort.h"
#include "../algorithms/sorting/QuickSort.h"
#include "../algorithms/searching/LinearSearch.h"
#include "../algorithms/searching/BinarySearch.h"
#include <algorithm>
#include <cstdlib>

Module4_PerformanceLab::Module4_PerformanceLab(QObject* parent)
    : QObject(parent) {}

QVector<BenchmarkResult> Module4_PerformanceLab::runBenchmark(int dataSize, const QStringList& algorithms) {
    emit benchmarkStarted(dataSize);

    QVector<int> base;
    base.reserve(dataSize);
    for (int i = 0; i < dataSize; ++i) base.push_back(rand() % 1000000);

    QVector<BenchmarkResult> results;
    PerformanceMeter meter;
    bool slow = (dataSize > 10000);

    int total = algorithms.size();
    int done = 0;

    for (const QString& alg : algorithms) {
        BenchmarkResult r;
        r.algorithmName = alg;
        r.dataSize = dataSize;
        r.dataType = "int";

        if (slow && (alg == "Bubble Sort" || alg == "Selection Sort" || alg == "Insertion Sort")) {
            r.timeMs = -1.0;
            results.push_back(r);
            ++done;
            emit progressUpdated(done * 100 / total);
            continue;
        }

        QVector<int> copy = base;
        meter.start();

        if (alg == "Bubble Sort") {
            bubbleSort<int>(copy);
        } else if (alg == "Selection Sort") {
            selectionSort<int>(copy);
        } else if (alg == "Insertion Sort") {
            insertionSort<int>(copy);
        } else if (alg == "Quick Sort") {
            quickSort<int>(copy);
        } else if (alg == "std::sort") {
            std::sort(copy.begin(), copy.end());
        }

        r.timeMs = meter.stop();
        results.push_back(r);
        ++done;
        emit progressUpdated(done * 100 / total);
    }

    history_.push_back(results);
    emit benchmarkFinished(results);
    return results;
}

QPair<double, double> Module4_PerformanceLab::compareSearchMethods(int dataSize) {
    return PerformanceMeter::benchmarkSearchComparison(dataSize);
}
