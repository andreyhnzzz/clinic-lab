#pragma once

#include <QWidget>
#include <QVector>
#include <QMap>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QBarSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QBarSet>

#include "SortBenchmark.h"

// Displays benchmark results as a bar chart (time per algorithm)
// and a line chart (time vs dataset size).
class ChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChartWidget(QWidget *parent = nullptr);

    // Update the bar chart with new results for a single dataset size
    void setBarData(const QVector<SortBenchmarkResult> &results);

    // Append one data point to the growth curves
    void appendGrowthPoint(const QString &algorithm, int dataSize, qint64 elapsedUs);

    // Clear all chart data
    void clear();

private:
    void setupUi();
    void setupBarChart();
    void setupLineChart();

    QChart     *m_barChart  = nullptr;
    QChart     *m_lineChart = nullptr;
    QChartView *m_barView   = nullptr;
    QChartView *m_lineView  = nullptr;

    // One line series per algorithm
    QMap<QString, QLineSeries*> m_growthSeries;
};
