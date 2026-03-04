#pragma once

#include <QObject>
#include <QVector>
#include <QString>
#include "Patient.h"

// Result for one sort algorithm run
struct SortBenchmarkResult {
    QString algorithm;      // "Bubble", "Selection", "Insertion", "Quick"
    int     dataSize;
    qint64  elapsedUs;      // microseconds
    qint64  comparisons;
    qint64  swaps;
};

// Runs four classic sorting algorithms on copies of the input data
// and records timing metrics.
//
// Large sorts (>50k) should be run in a QThread; emit progress signals.
class SortBenchmark : public QObject
{
    Q_OBJECT

public:
    explicit SortBenchmark(QObject *parent = nullptr);

    // Run all four algorithms on a copy of `data`.
    // Returns results in the order: Bubble, Selection, Insertion, Quick.
    QVector<SortBenchmarkResult> runAll(const QVector<Patient> &data);

    // Individual algorithm runners (operate on a mutable copy)
    SortBenchmarkResult runBubble(QVector<Patient> data);
    SortBenchmarkResult runSelection(QVector<Patient> data);
    SortBenchmarkResult runInsertion(QVector<Patient> data);
    SortBenchmarkResult runQuick(QVector<Patient> data);

signals:
    void progressChanged(int percent);
    void algorithmFinished(const SortBenchmarkResult &result);
    void allFinished(const QVector<SortBenchmarkResult> &results);

private:
    void quickSortHelper(QVector<Patient> &data, int low, int high,
                         qint64 &comparisons, qint64 &swaps);
    int  partition(QVector<Patient> &data, int low, int high,
                   qint64 &comparisons, qint64 &swaps);
};
