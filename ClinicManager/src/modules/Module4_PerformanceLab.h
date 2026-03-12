#pragma once
#include <QObject>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QPair>
#include <QFuture>
#include <functional>
#include "../utils/PerformanceMeter.h"
<<<<<<< HEAD
#include "../models/Paciente.h"
#include "../models/Consulta.h"
=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31

class Module4_PerformanceLab : public QObject {
    Q_OBJECT
public:
    explicit Module4_PerformanceLab(QObject* parent = nullptr);

<<<<<<< HEAD
    // Benchmark using real data (patients or consultations)
    static QVector<BenchmarkResult> runBenchmarkBatch(
        const QVector<Paciente>& pacientes,
        const QVector<Consulta>& consultas,
        const QString& datasetType,   // "Pacientes" or "Consultas"
        const QString& sortField,
=======
    // Benchmark thread-safe, sin depender de QObject ni señales.
    static QVector<BenchmarkResult> runBenchmarkBatch(
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
        int dataSize,
        const QStringList& algorithms,
        const std::function<void(int)>& progressCb = {});

<<<<<<< HEAD
    // Compare linear vs binary search on real data
    static QPair<double, double> compareSearchMethods(
        const QVector<Paciente>& pacientes,
        const QVector<Consulta>& consultas,
        const QString& datasetType,
        const QString& sortField,
        int dataSize);
=======
    // Conveniencia legacy: llamar solo desde hilo GUI (emite señales y muta historial).
    QVector<BenchmarkResult> runBenchmark(int dataSize, const QStringList& algorithms);

    // Compare linear vs binary search
    QPair<double, double> compareSearchMethods(int dataSize);
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31

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
