#pragma once

#include <QWidget>
#include <QVector>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QGroupBox>

#include "Patient.h"
#include "SortBenchmark.h"
#include "ChartWidget.h"

// Module 4 – Performance Lab
// Allows the user to run sorting benchmarks for different dataset sizes,
// view timing results in a table and on charts.
class PerformanceLabWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PerformanceLabWindow(QWidget *parent = nullptr);

    void setPatients(const QVector<Patient> &patients);

private slots:
    void onRunBenchmark();
    void onAlgorithmFinished(const SortBenchmarkResult &result);
    void onAllFinished(const QVector<SortBenchmarkResult> &results);
    void onExportCsv();

private:
    void setupUi();
    void addResultToHistory(const SortBenchmarkResult &r);

    QVector<Patient> m_patients;
    SortBenchmark    m_bench;

    // Controls
    QComboBox    *m_sizeCombo   = nullptr;
    QPushButton  *m_runBtn      = nullptr;
    QPushButton  *m_exportBtn   = nullptr;
    QProgressBar *m_progress    = nullptr;
    QLabel       *m_statusLabel = nullptr;

    // Results table
    QTableWidget *m_historyTable = nullptr;

    // Charts
    ChartWidget  *m_charts = nullptr;

    // Accumulated growth data: algorithm → list of (size, us)
    QMap<QString, QVector<QPair<int,qint64>>> m_growthData;
};
