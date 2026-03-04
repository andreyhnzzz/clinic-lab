#include "ChartWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>

// ── Constructor ───────────────────────────────────────────────────────────────

ChartWidget::ChartWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void ChartWidget::setupUi()
{
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    setupBarChart();
    setupLineChart();

    layout->addWidget(m_barView,  1);
    layout->addWidget(m_lineView, 1);
}

void ChartWidget::setupBarChart()
{
    m_barChart = new QChart;
    m_barChart->setTitle("Tiempo por Algoritmo (µs)");
    m_barChart->setAnimationOptions(QChart::SeriesAnimations);
    m_barChart->legend()->setVisible(true);
    m_barChart->legend()->setAlignment(Qt::AlignBottom);

    m_barView = new QChartView(m_barChart, this);
    m_barView->setRenderHint(QPainter::Antialiasing);
}

void ChartWidget::setupLineChart()
{
    m_lineChart = new QChart;
    m_lineChart->setTitle("Curvas de Crecimiento (Tamaño vs Tiempo µs)");
    m_lineChart->setAnimationOptions(QChart::AllAnimations);
    m_lineChart->legend()->setVisible(true);
    m_lineChart->legend()->setAlignment(Qt::AlignBottom);

    m_lineView = new QChartView(m_lineChart, this);
    m_lineView->setRenderHint(QPainter::Antialiasing);
}

// ── Public API ────────────────────────────────────────────────────────────────

void ChartWidget::setBarData(const QVector<SortBenchmarkResult> &results)
{
    m_barChart->removeAllSeries();

    // One BarSet per algorithm, one category per algorithm
    auto *barSeries = new QBarSeries;
    QStringList categories;

    for (const auto &r : results) {
        auto *set = new QBarSet(r.algorithm);
        *set << static_cast<double>(r.elapsedUs);
        barSeries->append(set);
        categories << r.algorithm;
    }

    m_barChart->addSeries(barSeries);

    auto *axisX = new QBarCategoryAxis;
    axisX->append(categories);
    m_barChart->addAxis(axisX, Qt::AlignBottom);
    barSeries->attachAxis(axisX);

    auto *axisY = new QValueAxis;
    axisY->setTitleText("µs");
    m_barChart->addAxis(axisY, Qt::AlignLeft);
    barSeries->attachAxis(axisY);
}

void ChartWidget::appendGrowthPoint(const QString &algorithm, int dataSize, qint64 elapsedUs)
{
    if (!m_growthSeries.contains(algorithm)) {
        auto *series = new QLineSeries;
        series->setName(algorithm);
        m_growthSeries[algorithm] = series;
        m_lineChart->addSeries(series);
        m_lineChart->createDefaultAxes();
    }
    m_growthSeries[algorithm]->append(dataSize, static_cast<double>(elapsedUs));
    m_lineChart->createDefaultAxes();
}

void ChartWidget::clear()
{
    m_barChart->removeAllSeries();
    m_lineChart->removeAllSeries();
    m_growthSeries.clear();
}
