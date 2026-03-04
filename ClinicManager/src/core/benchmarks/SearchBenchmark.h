#pragma once

#include <QObject>
#include <QVector>
#include <QString>
#include "Patient.h"
#include "SearchAlgorithms.h"

struct SearchBenchmarkResult {
    QString algorithm;       // "Linear" or "Binary"
    int     dataSize;
    qint64  elapsedUs;
    int     comparisons;
    bool    found;
    QString searchTerm;
};

// Compares linear vs binary search performance across multiple dataset sizes.
class SearchBenchmark : public QObject
{
    Q_OBJECT

public:
    explicit SearchBenchmark(QObject *parent = nullptr);

    // Run both algorithms searching for `cedula` in the provided dataset.
    // Binary search will first sort the data (sort time excluded from result).
    QVector<SearchBenchmarkResult> runAll(const QVector<Patient> &data,
                                          const QString &cedula);

    SearchBenchmarkResult runLinear(const QVector<Patient> &data,
                                    const QString &cedula);

    SearchBenchmarkResult runBinary(const QVector<Patient> &sortedData,
                                    const QString &cedula);

signals:
    void allFinished(const QVector<SearchBenchmarkResult> &results);
};
