#include "PerformanceLabWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QFont>
#include <QHeaderView>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>

PerformanceLabWidget::PerformanceLabWidget(QWidget* parent)
    : QWidget(parent) {
    module_ = new Module4_PerformanceLab(this);
    watcher_ = new QFutureWatcher<QVector<BenchmarkResult>>(this);
    setupUI();

    connect(watcher_, &QFutureWatcher<QVector<BenchmarkResult>>::finished, this, [this](){
        onBenchmarkDone(watcher_->result());
        btnRun_->setEnabled(true);
        progressBar_->setValue(100);
        lblStatus_->setText("Benchmark completado.");
    });

    connect(module_, &Module4_PerformanceLab::progressUpdated, this, [this](int p){
        progressBar_->setValue(p);
    });
}

void PerformanceLabWidget::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    auto* title = new QLabel("📊 Laboratorio de Rendimiento de Algoritmos");
    QFont f = title->font(); f.setPointSize(14); f.setBold(true);
    title->setFont(f);
    title->setObjectName("sectionTitle");
    mainLayout->addWidget(title);

    // Controls group
    auto* controlGroup = new QGroupBox("Configuración del Benchmark");
    auto* controlLayout = new QHBoxLayout(controlGroup);

    // Dataset size
    auto* sizeLayout = new QVBoxLayout();
    sizeLayout->addWidget(new QLabel("Tamaño del dataset:"));
    cbSize_ = new QComboBox();
    cbSize_->addItems({"500", "5,000", "50,000", "200,000"});
    cbSize_->setCurrentIndex(1);
    sizeLayout->addWidget(cbSize_);
    controlLayout->addLayout(sizeLayout);

    // Algorithm checkboxes
    auto* algGroup = new QGroupBox("Algoritmos");
    auto* algLayout = new QVBoxLayout(algGroup);
    chkBubble_    = new QCheckBox("Bubble Sort");
    chkSelection_ = new QCheckBox("Selection Sort");
    chkInsertion_ = new QCheckBox("Insertion Sort");
    chkQuick_     = new QCheckBox("Quick Sort");
    chkStd_       = new QCheckBox("std::sort");
    chkBubble_->setChecked(true);
    chkSelection_->setChecked(true);
    chkInsertion_->setChecked(true);
    chkQuick_->setChecked(true);
    chkStd_->setChecked(true);
    for (auto* c : {chkBubble_, chkSelection_, chkInsertion_, chkQuick_, chkStd_})
        algLayout->addWidget(c);
    controlLayout->addWidget(algGroup);

    // Run buttons
    auto* btnLayout = new QVBoxLayout();
    btnRun_   = new QPushButton("▶  Ejecutar Comparación");
    btnClear_ = new QPushButton("🗑  Limpiar Historial");
    btnRun_->setObjectName("primaryButton");
    btnRun_->setMinimumHeight(40);
    btnClear_->setMinimumHeight(35);
    btnLayout->addWidget(btnRun_);
    btnLayout->addWidget(btnClear_);
    btnLayout->addStretch();
    controlLayout->addLayout(btnLayout);

    mainLayout->addWidget(controlGroup);

    // Progress
    auto* progressLayout = new QHBoxLayout();
    lblStatus_   = new QLabel("Listo.");
    progressBar_ = new QProgressBar();
    progressBar_->setRange(0, 100);
    progressBar_->setValue(0);
    progressBar_->setFixedHeight(18);
    progressLayout->addWidget(lblStatus_);
    progressLayout->addWidget(progressBar_, 1);
    mainLayout->addLayout(progressLayout);

    // Charts + Table splitter
    auto* splitter = new QSplitter(Qt::Vertical, this);

    auto* chartsWidget = new QWidget();
    auto* chartsLayout = new QHBoxLayout(chartsWidget);

    // Bar chart
    auto* barChart = new QChart();
    barChart->setTitle("Tiempos por Algoritmo");
    barChart->setTheme(QChart::ChartThemeDark);
    barChart->legend()->setVisible(true);
    barChartView_ = new QChartView(barChart);
    barChartView_->setRenderHint(QPainter::Antialiasing);
    barChartView_->setMinimumHeight(250);

    // Growth chart
    auto* growthChart = new QChart();
    growthChart->setTitle("Curva de Crecimiento");
    growthChart->setTheme(QChart::ChartThemeDark);
    growthChart->legend()->setVisible(true);
    growthChartView_ = new QChartView(growthChart);
    growthChartView_->setRenderHint(QPainter::Antialiasing);
    growthChartView_->setMinimumHeight(250);

    chartsLayout->addWidget(barChartView_);
    chartsLayout->addWidget(growthChartView_);
    splitter->addWidget(chartsWidget);

    // History table
    historyTable_ = new QTableWidget(0, 4);
    historyTable_->setHorizontalHeaderLabels({"Algoritmo", "Tamaño", "Tiempo (ms)", "Tipo"});
    historyTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    historyTable_->setAlternatingRowColors(true);
    historyTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    historyTable_->verticalHeader()->setVisible(false);
    historyTable_->setMinimumHeight(150);
    splitter->addWidget(historyTable_);

    mainLayout->addWidget(splitter, 1);

    // Search comparison section
    auto* searchGroup = new QGroupBox("Comparación: Búsqueda Lineal vs Binaria");
    auto* searchLayout = new QHBoxLayout(searchGroup);
    lblLinearTime_ = new QLabel("Lineal: —");
    lblBinaryTime_ = new QLabel("Binaria: —");
    btnSearchComp_ = new QPushButton("Comparar Búsquedas");
    btnSearchComp_->setObjectName("primaryButton");
    searchLayout->addWidget(btnSearchComp_);
    searchLayout->addWidget(lblLinearTime_);
    searchLayout->addWidget(new QLabel("|"));
    searchLayout->addWidget(lblBinaryTime_);
    searchLayout->addStretch();
    mainLayout->addWidget(searchGroup);

    connect(btnRun_,  &QPushButton::clicked, this, &PerformanceLabWidget::onRunBenchmark);
    connect(btnClear_, &QPushButton::clicked, this, [this](){
        module_->clearHistory();
        historyTable_->setRowCount(0);
        // Reset growth chart
        auto* chart = new QChart();
        chart->setTitle("Curva de Crecimiento");
        chart->setTheme(QChart::ChartThemeDark);
        chart->legend()->setVisible(true);
        growthChartView_->setChart(chart);
    });
    connect(btnSearchComp_, &QPushButton::clicked, this, &PerformanceLabWidget::onSearchComparison);
}

void PerformanceLabWidget::onRunBenchmark() {
    QStringList selectedAlgs;
    if (chkBubble_->isChecked())    selectedAlgs << "Bubble Sort";
    if (chkSelection_->isChecked()) selectedAlgs << "Selection Sort";
    if (chkInsertion_->isChecked()) selectedAlgs << "Insertion Sort";
    if (chkQuick_->isChecked())     selectedAlgs << "Quick Sort";
    if (chkStd_->isChecked())       selectedAlgs << "std::sort";

    if (selectedAlgs.isEmpty()) {
        QMessageBox::warning(this, "Sin algoritmos", "Seleccione al menos un algoritmo.");
        return;
    }

    static const int sizes[] = {500, 5000, 50000, 200000};
    int dataSize = sizes[cbSize_->currentIndex()];

    if (dataSize > 10000) {
        bool hasSlow = chkBubble_->isChecked() || chkSelection_->isChecked() || chkInsertion_->isChecked();
        if (hasSlow) {
            lblStatus_->setText("⚠ Nota: Bubble/Selection/Insertion omitidos para n > 10,000");
        }
    }

    btnRun_->setEnabled(false);
    progressBar_->setValue(0);
    lblStatus_->setText(QString("Ejecutando benchmark para n=%1...").arg(dataSize));

    // Run in background thread
    auto future = QtConcurrent::run([this, dataSize, selectedAlgs]() -> QVector<BenchmarkResult> {
        return module_->runBenchmark(dataSize, selectedAlgs);
    });
    watcher_->setFuture(future);
}

void PerformanceLabWidget::onBenchmarkDone(QVector<BenchmarkResult> results) {
    updateBarChart(results);
    updateGrowthChart(module_->getHistory());
    addResultsToTable(results);
}

void PerformanceLabWidget::updateBarChart(const QVector<BenchmarkResult>& results) {
    auto* barSet = new QBarSet("Tiempo (ms)");
    QStringList categories;
    double maxVal = 0.0;

    for (const auto& r : results) {
        double val = r.timeMs >= 0.0 ? r.timeMs : 0.0;
        *barSet << val;
        categories << r.algorithmName;
        if (val > maxVal) maxVal = val;
    }

    auto* series = new QBarSeries();
    series->append(barSet);

    auto* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(QString("Tiempos por Algoritmo (n=%1)").arg(results.isEmpty() ? 0 : results[0].dataSize));
    chart->setTheme(QChart::ChartThemeDark);

    auto* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto* axisY = new QValueAxis();
    axisY->setTitleText("Tiempo (ms)");
    axisY->setMin(0);
    axisY->setMax(maxVal * 1.1 + 0.1);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(false);
    barChartView_->setChart(chart);
}

void PerformanceLabWidget::updateGrowthChart(const QVector<QVector<BenchmarkResult>>& history) {
    QMap<QString, QLineSeries*> seriesMap;

    for (const auto& batch : history) {
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

    auto* chart = new QChart();
    chart->setTitle("Curva de Crecimiento");
    chart->setTheme(QChart::ChartThemeDark);
    for (auto* s : seriesMap) chart->addSeries(s);
    chart->createDefaultAxes();

    if (!chart->axes(Qt::Horizontal).isEmpty())
        if (auto* ax = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).first()))
            ax->setTitleText("Tamaño de datos (n)");
    if (!chart->axes(Qt::Vertical).isEmpty())
        if (auto* ay = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first()))
            ay->setTitleText("Tiempo (ms)");

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    growthChartView_->setChart(chart);
}

void PerformanceLabWidget::addResultsToTable(const QVector<BenchmarkResult>& results) {
    for (const auto& r : results) {
        int row = historyTable_->rowCount();
        historyTable_->insertRow(row);
        historyTable_->setItem(row, 0, new QTableWidgetItem(r.algorithmName));
        historyTable_->setItem(row, 1, new QTableWidgetItem(QString::number(r.dataSize)));
        if (r.timeMs >= 0.0) {
            historyTable_->setItem(row, 2, new QTableWidgetItem(
                QString::number(r.timeMs, 'f', 3)));
        } else {
            auto* item = new QTableWidgetItem("Omitido");
            item->setForeground(QColor("#FF9800"));
            historyTable_->setItem(row, 2, item);
        }
        historyTable_->setItem(row, 3, new QTableWidgetItem(r.dataType));
    }
    historyTable_->scrollToBottom();
}

void PerformanceLabWidget::onSearchComparison() {
    static const int sizes[] = {500, 5000, 50000, 200000};
    int dataSize = sizes[cbSize_->currentIndex()];
    auto [linearMs, binaryMs] = module_->compareSearchMethods(dataSize);
    lblLinearTime_->setText(QString("Lineal: %1 ms").arg(linearMs, 0, 'f', 4));
    lblBinaryTime_->setText(QString("Binaria: %1 ms").arg(binaryMs, 0, 'f', 4));
}
