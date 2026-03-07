#pragma once
#include <QObject>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QPair>
#include <QFuture>
#include "../utils/PerformanceMeter.h"

class Module4_PerformanceLab : public QObject {
    Q_OBJECT
public:
    explicit Module4_PerformanceLab(QObject* parent = nullptr);

    // Run sort benchmarks for given size and algorithms
    QVector<BenchmarkResult> runBenchmark(int dataSize, const QStringList& algorithms);

    // Compare linear vs binary search
    QPair<double, double> compareSearchMethods(int dataSize);

    // Get all accumulated results
    const QVector<QVector<BenchmarkResult>>& getHistory() const { return history_; }
    void clearHistory() { history_.clear(); }

signals:
    void benchmarkStarted(int dataSize);
    void benchmarkFinished(QVector<BenchmarkResult> results);
    void progressUpdated(int percent);

private:
    QVector<QVector<BenchmarkResult>> history_;
};
