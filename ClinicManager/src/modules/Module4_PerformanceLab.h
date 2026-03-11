#pragma once
#include <QObject>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QPair>
#include <QFuture>
#include <functional>
#include "../utils/PerformanceMeter.h"
#include "../models/Paciente.h"
#include "../models/Consulta.h"

class Module4_PerformanceLab : public QObject {
    Q_OBJECT
public:
    explicit Module4_PerformanceLab(QObject* parent = nullptr);

    // Benchmark using real data (patients or consultations)
    static QVector<BenchmarkResult> runBenchmarkBatch(
        const QVector<Paciente>& pacientes,
        const QVector<Consulta>& consultas,
        const QString& datasetType,   // "Pacientes" or "Consultas"
        const QString& sortField,
        int dataSize,
        const QStringList& algorithms,
        const std::function<void(int)>& progressCb = {});

    // Compare linear vs binary search on real data
    static QPair<double, double> compareSearchMethods(
        const QVector<Paciente>& pacientes,
        const QVector<Consulta>& consultas,
        const QString& datasetType,
        const QString& sortField,
        int dataSize);

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
