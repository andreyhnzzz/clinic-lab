#pragma once
#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QProgressBar>
#include <QChartView>
#include <QChart>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QLineSeries>
#include <QFuture>
#include <QFutureWatcher>
#include "../modules/Module4_PerformanceLab.h"

class PerformanceLabWidget : public QWidget {
    Q_OBJECT
public:
    explicit PerformanceLabWidget(QWidget* parent = nullptr);

private slots:
    void onRunBenchmark();
<<<<<<< HEAD
    void onRunFullSeries();
=======
    void onBenchmarkDone(QVector<BenchmarkResult> results);
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    void onSearchComparison();

private:
    void setupUI();
    void updateBarChart(const QVector<BenchmarkResult>& results);
    void updateGrowthChart(const QVector<QVector<BenchmarkResult>>& history);
    void addResultsToTable(const QVector<BenchmarkResult>& results);
<<<<<<< HEAD
    QStringList getSelectedAlgorithms() const;
    int getCurrentDataSize() const;
    QString getDatasetType() const;
    QString getSortField() const;
=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31

    Module4_PerformanceLab* module_ = nullptr;

    QComboBox* cbSize_ = nullptr;
<<<<<<< HEAD
    QComboBox* cbDataset_ = nullptr;
    QComboBox* cbField_ = nullptr;
=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    QCheckBox* chkBubble_ = nullptr;
    QCheckBox* chkSelection_ = nullptr;
    QCheckBox* chkInsertion_ = nullptr;
    QCheckBox* chkQuick_ = nullptr;
    QCheckBox* chkStd_ = nullptr;
    QPushButton* btnRun_ = nullptr;
<<<<<<< HEAD
    QPushButton* btnRunSeries_ = nullptr;
=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    QPushButton* btnClear_ = nullptr;
    QProgressBar* progressBar_ = nullptr;
    QLabel* lblStatus_ = nullptr;

    QChartView* barChartView_ = nullptr;
    QChartView* growthChartView_ = nullptr;
    QTableWidget* historyTable_ = nullptr;

    // Search comparison
    QLabel* lblLinearTime_ = nullptr;
    QLabel* lblBinaryTime_ = nullptr;
    QPushButton* btnSearchComp_ = nullptr;

    QFutureWatcher<QVector<BenchmarkResult>>* watcher_ = nullptr;
<<<<<<< HEAD
    bool runningFullSeries_ = false;
    int fullSeriesIndex_ = 0;
=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
};
