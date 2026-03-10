#pragma once
#include <QObject>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QPair>
#include <QFuture>
#include <functional>
#include "../utils/PerformanceMeter.h"

class Module4_PerformanceLab : public QObject {
    Q_OBJECT
public:
    explicit Module4_PerformanceLab(QObject* parent = nullptr);

    // Benchmark thread-safe, sin depender de QObject ni señales.
    static QVector<BenchmarkResult> runBenchmarkBatch(
        int dataSize,
        const QStringList& algorithms,
        const std::function<void(int)>& progressCb = {});

    // Conveniencia legacy: llamar solo desde hilo GUI (emite señales y muta historial).
    QVector<BenchmarkResult> runBenchmark(int dataSize, const QStringList& algorithms);

    // Compare linear vs binary search
    QPair<double, double> compareSearchMethods(int dataSize);

    // Historial
    const QVector<QVector<BenchmarkResult>>& getHistory() const { return history_; }
    void clearHistory() { history_.clear(); }
    void appendToHistory(const QVector<BenchmarkResult>& batch);

signals:
    void benchmarkStarted(int dataSize);
    void benchmarkFinished(QVector<BenchmarkResult> results);
    void progressUpdated(int percent);

private:
    QVector<QVector<BenchmarkResult>> history_;
};
