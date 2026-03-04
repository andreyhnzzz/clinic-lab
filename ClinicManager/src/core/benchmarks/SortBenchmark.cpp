#include "SortBenchmark.h"
#include "TimerUtils.h"

SortBenchmark::SortBenchmark(QObject *parent)
    : QObject(parent) {}

// ── Run all ───────────────────────────────────────────────────────────────────

QVector<SortBenchmarkResult> SortBenchmark::runAll(const QVector<Patient> &data)
{
    QVector<SortBenchmarkResult> results;

    auto emitAndStore = [&](SortBenchmarkResult r) {
        results.append(r);
        emit algorithmFinished(r);
    };

    emitAndStore(runBubble(data));
    emit progressChanged(25);
    emitAndStore(runSelection(data));
    emit progressChanged(50);
    emitAndStore(runInsertion(data));
    emit progressChanged(75);
    emitAndStore(runQuick(data));
    emit progressChanged(100);

    emit allFinished(results);
    return results;
}

// ── Bubble sort ───────────────────────────────────────────────────────────────

SortBenchmarkResult SortBenchmark::runBubble(QVector<Patient> data)
{
    SortBenchmarkResult res;
    res.algorithm = "Bubble Sort";
    res.dataSize  = data.size();
    res.comparisons = 0;
    res.swaps       = 0;

    TimerUtils::ScopedTimer t;
    int n = data.size();
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; ++j) {
            ++res.comparisons;
            if (data[j] > data[j + 1]) {
                qSwap(data[j], data[j + 1]);
                ++res.swaps;
                swapped = true;
            }
        }
        if (!swapped) break; // early termination
    }
    res.elapsedUs = t.elapsedMicroseconds();
    return res;
}

// ── Selection sort ────────────────────────────────────────────────────────────

SortBenchmarkResult SortBenchmark::runSelection(QVector<Patient> data)
{
    SortBenchmarkResult res;
    res.algorithm = "Selection Sort";
    res.dataSize  = data.size();
    res.comparisons = 0;
    res.swaps       = 0;

    TimerUtils::ScopedTimer t;
    int n = data.size();
    for (int i = 0; i < n - 1; ++i) {
        int minIdx = i;
        for (int j = i + 1; j < n; ++j) {
            ++res.comparisons;
            if (data[j] < data[minIdx]) minIdx = j;
        }
        if (minIdx != i) {
            qSwap(data[i], data[minIdx]);
            ++res.swaps;
        }
    }
    res.elapsedUs = t.elapsedMicroseconds();
    return res;
}

// ── Insertion sort ────────────────────────────────────────────────────────────

SortBenchmarkResult SortBenchmark::runInsertion(QVector<Patient> data)
{
    SortBenchmarkResult res;
    res.algorithm = "Insertion Sort";
    res.dataSize  = data.size();
    res.comparisons = 0;
    res.swaps       = 0;

    TimerUtils::ScopedTimer t;
    int n = data.size();
    for (int i = 1; i < n; ++i) {
        Patient key = data[i];
        int j = i - 1;
        while (j >= 0) {
            ++res.comparisons;
            if (data[j] > key) {
                data[j + 1] = data[j];
                ++res.swaps;
                --j;
            } else {
                break;
            }
        }
        data[j + 1] = key;
    }
    res.elapsedUs = t.elapsedMicroseconds();
    return res;
}

// ── Quick sort ────────────────────────────────────────────────────────────────

SortBenchmarkResult SortBenchmark::runQuick(QVector<Patient> data)
{
    SortBenchmarkResult res;
    res.algorithm = "Quick Sort";
    res.dataSize  = data.size();
    res.comparisons = 0;
    res.swaps       = 0;

    TimerUtils::ScopedTimer t;
    quickSortHelper(data, 0, data.size() - 1, res.comparisons, res.swaps);
    res.elapsedUs = t.elapsedMicroseconds();
    return res;
}

void SortBenchmark::quickSortHelper(QVector<Patient> &data, int low, int high,
                                    qint64 &comparisons, qint64 &swaps)
{
    if (low < high) {
        int pivot = partition(data, low, high, comparisons, swaps);
        quickSortHelper(data, low,      pivot - 1, comparisons, swaps);
        quickSortHelper(data, pivot + 1, high,     comparisons, swaps);
    }
}

int SortBenchmark::partition(QVector<Patient> &data, int low, int high,
                             qint64 &comparisons, qint64 &swaps)
{
    // Median-of-three pivot selection to avoid worst-case on sorted input
    int mid = low + (high - low) / 2;
    if (data[mid] < data[low])  { qSwap(data[low],  data[mid]);  ++swaps; }
    if (data[high] < data[low]) { qSwap(data[low],  data[high]); ++swaps; }
    if (data[mid] < data[high]) { qSwap(data[mid],  data[high]); ++swaps; }
    // data[high] is now the median pivot
    Patient pivot = data[high];

    int i = low - 1;
    for (int j = low; j < high; ++j) {
        ++comparisons;
        if (data[j] < pivot) {
            ++i;
            qSwap(data[i], data[j]);
            ++swaps;
        }
    }
    qSwap(data[i + 1], data[high]);
    ++swaps;
    return i + 1;
}
