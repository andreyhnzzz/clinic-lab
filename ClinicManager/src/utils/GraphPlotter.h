#pragma once
#include <QChartView>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QChart>
#include <QLineSeries>
#include <QVector>
#include <QString>
#include "../utils/PerformanceMeter.h"

class GraphPlotter {
public:
    static QChartView* createBarChart(const QVector<BenchmarkResult>& results,
                                      const QString& title = "Comparación de Algoritmos") {
        auto* chart = new QChart();
        chart->setTitle(title);
        chart->setAnimationOptions(QChart::SeriesAnimations);

        auto* barSet = new QBarSet("Tiempo (ms)");
        QStringList categories;
        for (const auto& r : results) {
            if (r.timeMs >= 0.0) {
                *barSet << r.timeMs;
            } else {
                *barSet << 0.0;
            }
            categories << r.algorithmName;
        }

        auto* series = new QBarSeries();
        series->append(barSet);
        chart->addSeries(series);

        auto* axisX = new QBarCategoryAxis();
        axisX->append(categories);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

        auto* axisY = new QValueAxis();
        axisY->setTitleText("Tiempo (ms)");
        axisY->setMin(0);
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);

        chart->legend()->setVisible(false);
        chart->setTheme(QChart::ChartThemeDark);

        auto* view = new QChartView(chart);
        view->setRenderHint(QPainter::Antialiasing);
        return view;
    }

    static QChartView* createGrowthChart(const QVector<QVector<BenchmarkResult>>& allResults,
                                          const QString& title = "Curva de Crecimiento") {
        auto* chart = new QChart();
        chart->setTitle(title);
        chart->setTheme(QChart::ChartThemeDark);

        // Group by algorithm name
        QMap<QString, QLineSeries*> seriesMap;
        for (const auto& batch : allResults) {
            for (const auto& r : batch) {
                if (r.timeMs < 0.0) continue;
                if (!seriesMap.contains(r.algorithmName)) {
                    auto* s = new QLineSeries();
                    s->setName(r.algorithmName);
                    seriesMap[r.algorithmName] = s;
                }
                seriesMap[r.algorithmName]->append(r.dataSize, r.timeMs);
            }
        }

        for (auto* s : seriesMap) chart->addSeries(s);
        chart->createDefaultAxes();
        auto hAxes = chart->axes(Qt::Horizontal);
        if (!hAxes.isEmpty()) {
            if (auto* axisX = qobject_cast<QValueAxis*>(hAxes.first()))
                axisX->setTitleText("Tamaño de datos");
        }
        auto vAxes = chart->axes(Qt::Vertical);
        if (!vAxes.isEmpty()) {
            if (auto* axisY = qobject_cast<QValueAxis*>(vAxes.first()))
                axisY->setTitleText("Tiempo (ms)");
        }

        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);

        auto* view = new QChartView(chart);
        view->setRenderHint(QPainter::Antialiasing);
        return view;
    }
};
