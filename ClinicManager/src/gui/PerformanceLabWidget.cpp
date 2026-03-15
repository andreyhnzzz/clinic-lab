#include "PerformanceLabWidget.h"
#include "../core/ClinicDataStore.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QFont>
#include <QHeaderView>
#include <QMessageBox>
#include <QMap>
#include <QtConcurrent/QtConcurrent>

// Matches the threshold in Module4_PerformanceLab.cpp
static constexpr int SLOW_ALGORITHM_THRESHOLD = 10000;

PerformanceLabWidget::PerformanceLabWidget(QWidget* parent)
    : QWidget(parent) {
    module_ = new Module4_PerformanceLab(this);
    watcher_ = new QFutureWatcher<QVector<BenchmarkResult>>(this);
    setupUI();

    connect(watcher_, &QFutureWatcher<QVector<BenchmarkResult>>::finished, this, [this](){
        auto results = watcher_->result();
        module_->appendToHistory(results);
        updateBarChart(results);
        updateGrowthChart(module_->getHistory());
        addResultsToTable(results);
        progressBar_->setValue(100);

        // If running full series, continue with next size
        if (runningFullSeries_) {
            static const int series[] = {500, 5000, 50000, 200000};
            fullSeriesIndex_++;
            if (fullSeriesIndex_ < 4) {
                lblStatus_->setText(QString("Serie completa: ejecutando n=%1...").arg(series[fullSeriesIndex_]));
                auto& store = ClinicDataStore::instance();
                QVector<Paciente> pacs = store.pacientes();
                QVector<Consulta> cons = store.consultas();
                QString dsType = getDatasetType();
                QString field = getSortField();
                QStringList algs = getSelectedAlgorithms();
                int sz = series[fullSeriesIndex_];
                auto future = QtConcurrent::run([pacs, cons, dsType, field, sz, algs, this]() {
                    return Module4_PerformanceLab::runBenchmarkBatch(
                        pacs, cons, dsType, field, sz, algs,
                        [this](int p){
                            QMetaObject::invokeMethod(this, [this, p](){
                                progressBar_->setValue(p);
                            }, Qt::QueuedConnection);
                        });
                });
                watcher_->setFuture(future);
                return;
            }
            runningFullSeries_ = false;
            lblStatus_->setText("Serie completa finalizada.");
            emit benchmarkCompleted("Serie completa");
        } else {
            lblStatus_->setText("Benchmark completado.");
            if (!results.isEmpty()) {
                emit benchmarkCompleted(QString("n=%1").arg(results[0].dataSize));
            }
        }
        btnRun_->setEnabled(true);
        btnRunSeries_->setEnabled(true);
        btnClear_->setEnabled(true);
        btnSearchComp_->setEnabled(true);
    });
}

void PerformanceLabWidget::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(12, 12, 12, 12);

    // Controls group
    auto* controlGroup = new QGroupBox("Configuracion del Benchmark");
    auto* controlLayout = new QHBoxLayout(controlGroup);

    // Dataset type and field selectors
    auto* dataLayout = new QVBoxLayout();
    dataLayout->addWidget(new QLabel("Tipo de datos:"));
    cbDataset_ = new QComboBox();
    cbDataset_->addItems({"Pacientes", "Consultas"});
    dataLayout->addWidget(cbDataset_);
    dataLayout->addWidget(new QLabel("Campo de ordenamiento:"));
    cbField_ = new QComboBox();
    cbField_->addItems({"Nombre", "Edad", "Fecha de Registro", "Prioridad"});
    dataLayout->addWidget(cbField_);
    controlLayout->addLayout(dataLayout);

    // Update field options when dataset changes
    connect(cbDataset_, &QComboBox::currentTextChanged, this, [this](const QString& ds) {
        cbField_->clear();
        if (ds == "Pacientes")
            cbField_->addItems({"Nombre", "Edad", "Fecha de Registro", "Prioridad"});
        else
            cbField_->addItems({"Fecha", "Gravedad", "Costo"});
    });

    // Dataset size
    auto* sizeLayout = new QVBoxLayout();
    sizeLayout->addWidget(new QLabel("Tamano del dataset:"));
    cbSize_ = new QComboBox();
    cbSize_->addItems({"500", "5,000", "50,000", "200,000"});
    cbSize_->setCurrentIndex(1);
    sizeLayout->addWidget(cbSize_);
    sizeLayout->addStretch();
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
    btnRun_   = new QPushButton("Ejecutar Comparacion");
    btnRunSeries_ = new QPushButton("Serie Completa (500-200K)");
    btnClear_ = new QPushButton("Limpiar Historial");
    btnRun_->setObjectName("primaryButton");
    btnRunSeries_->setObjectName("primaryButton");
    btnRun_->setMinimumHeight(38);
    btnRunSeries_->setMinimumHeight(38);
    btnClear_->setMinimumHeight(32);
    btnLayout->addWidget(btnRun_);
    btnLayout->addWidget(btnRunSeries_);
    btnLayout->addWidget(btnClear_);
    btnLayout->addStretch();
    controlLayout->addLayout(btnLayout);

    mainLayout->addWidget(controlGroup);

    // Progress
    auto* progressLayout = new QHBoxLayout();
    lblStatus_   = new QLabel("Listo. Los datos provienen del repositorio central del sistema.");
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
    barChartView_->setMinimumHeight(220);

    // Growth chart
    auto* growthChart = new QChart();
    growthChart->setTitle("Curva de Crecimiento");
    growthChart->setTheme(QChart::ChartThemeDark);
    growthChart->legend()->setVisible(true);
    growthChartView_ = new QChartView(growthChart);
    growthChartView_->setRenderHint(QPainter::Antialiasing);
    growthChartView_->setMinimumHeight(220);

    chartsLayout->addWidget(barChartView_);
    chartsLayout->addWidget(growthChartView_);
    splitter->addWidget(chartsWidget);

    // History table
    historyTable_ = new QTableWidget(0, 9);
    historyTable_->setHorizontalHeaderLabels({
        "Algoritmo", "Tamano", "Tiempo (ms)", "Comparaciones", "Swaps/Movimientos",
        "Complejidad", "Estabilidad", "Memoria", "Tipo de Datos"});
    historyTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    historyTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    historyTable_->verticalHeader()->setVisible(false);
    historyTable_->setMinimumHeight(130);
    splitter->addWidget(historyTable_);

    mainLayout->addWidget(splitter, 1);

    // Search comparison section
    auto* searchGroup = new QGroupBox("Comparacion: Busqueda Lineal vs Binaria (sobre conjunto real)");
    auto* searchLayout = new QHBoxLayout(searchGroup);
    lblLinearTime_ = new QLabel("Lineal: --");
    lblBinaryTime_ = new QLabel("Binaria: --");
    lblSortCost_ = new QLabel("Costo orden: --");
    btnSearchComp_ = new QPushButton("Comparar Busquedas");
    btnSearchComp_->setObjectName("primaryButton");
    searchLayout->addWidget(btnSearchComp_);
    searchLayout->addWidget(lblLinearTime_);
    searchLayout->addWidget(new QLabel("|"));
    searchLayout->addWidget(lblBinaryTime_);
    searchLayout->addWidget(new QLabel("|"));
    searchLayout->addWidget(lblSortCost_);
    searchLayout->addStretch();
    mainLayout->addWidget(searchGroup);

    connect(btnRun_,  &QPushButton::clicked, this, &PerformanceLabWidget::onRunBenchmark);
    connect(btnRunSeries_, &QPushButton::clicked, this, &PerformanceLabWidget::onRunFullSeries);
    connect(btnClear_, &QPushButton::clicked, this, [this](){
        module_->clearHistory();
        historyTable_->setRowCount(0);
        auto* chart = new QChart();
        chart->setTitle("Curva de Crecimiento");
        chart->setTheme(QChart::ChartThemeDark);
        chart->legend()->setVisible(true);
        growthChartView_->setChart(chart);
    });
    connect(btnSearchComp_, &QPushButton::clicked, this, &PerformanceLabWidget::onSearchComparison);

    // Benchmark analysis section
    auto* analysisGroup = new QGroupBox("Analisis Automatico del Benchmark");
    auto* analysisLayout = new QVBoxLayout(analysisGroup);
    lblAnalysis_ = new QLabel("Ejecute una serie completa para generar conclusiones automaticas.");
    lblAnalysis_->setWordWrap(true);
    lblAnalysis_->setStyleSheet("color: #A7B1BC; font-size: 12px; padding: 8px;");
    analysisLayout->addWidget(lblAnalysis_);
    mainLayout->addWidget(analysisGroup);
}

QStringList PerformanceLabWidget::getSelectedAlgorithms() const {
    QStringList selectedAlgs;
    if (chkBubble_->isChecked())    selectedAlgs << "Bubble Sort";
    if (chkSelection_->isChecked()) selectedAlgs << "Selection Sort";
    if (chkInsertion_->isChecked()) selectedAlgs << "Insertion Sort";
    if (chkQuick_->isChecked())     selectedAlgs << "Quick Sort";
    if (chkStd_->isChecked())       selectedAlgs << "std::sort";
    return selectedAlgs;
}

int PerformanceLabWidget::getCurrentDataSize() const {
    static const int sizes[] = {500, 5000, 50000, 200000};
    return sizes[cbSize_->currentIndex()];
}

QString PerformanceLabWidget::getDatasetType() const {
    return cbDataset_->currentText();
}

QString PerformanceLabWidget::getSortField() const {
    return cbField_->currentText();
}

void PerformanceLabWidget::onRunBenchmark() {
    QStringList selectedAlgs = getSelectedAlgorithms();
    if (selectedAlgs.isEmpty()) {
        QMessageBox::warning(this, "Sin algoritmos", "Seleccione al menos un algoritmo.");
        return;
    }

    int dataSize = getCurrentDataSize();

    if (dataSize > 10000) {
        bool hasSlow = chkBubble_->isChecked() || chkSelection_->isChecked() || chkInsertion_->isChecked();
        if (hasSlow)
            lblStatus_->setText("Nota: O(n^2) omitidos para n > 10,000 por inviabilidad practica");
    }

    btnRun_->setEnabled(false);
    btnRunSeries_->setEnabled(false);
    btnClear_->setEnabled(false);
    btnSearchComp_->setEnabled(false);
    progressBar_->setValue(0);

    auto& store = ClinicDataStore::instance();
    QVector<Paciente> pacs = store.pacientes();
    QVector<Consulta> cons = store.consultas();
    QString dsType = getDatasetType();
    QString field = getSortField();

    lblStatus_->setText(QString("Ejecutando benchmark: %1/%2, n=%3...")
        .arg(dsType).arg(field).arg(dataSize));

    auto future = QtConcurrent::run([pacs, cons, dsType, field, dataSize, selectedAlgs, this]() {
        return Module4_PerformanceLab::runBenchmarkBatch(
            pacs, cons, dsType, field, dataSize, selectedAlgs,
            [this](int p){
                QMetaObject::invokeMethod(this, [this, p](){
                    progressBar_->setValue(p);
                }, Qt::QueuedConnection);
            });
    });
    watcher_->setFuture(future);
}

void PerformanceLabWidget::onRunFullSeries() {
    QStringList selectedAlgs = getSelectedAlgorithms();
    if (selectedAlgs.isEmpty()) {
        QMessageBox::warning(this, "Sin algoritmos", "Seleccione al menos un algoritmo.");
        return;
    }

    btnRun_->setEnabled(false);
    btnRunSeries_->setEnabled(false);
    btnClear_->setEnabled(false);
    btnSearchComp_->setEnabled(false);

    runningFullSeries_ = true;
    fullSeriesIndex_ = 0;

    static const int series[] = {500, 5000, 50000, 200000};
    int sz = series[0];

    auto& store = ClinicDataStore::instance();
    QVector<Paciente> pacs = store.pacientes();
    QVector<Consulta> cons = store.consultas();
    QString dsType = getDatasetType();
    QString field = getSortField();

    lblStatus_->setText(QString("Serie completa: ejecutando n=%1...").arg(sz));
    progressBar_->setValue(0);

    auto future = QtConcurrent::run([pacs, cons, dsType, field, sz, selectedAlgs, this]() {
        return Module4_PerformanceLab::runBenchmarkBatch(
            pacs, cons, dsType, field, sz, selectedAlgs,
            [this](int p){
                QMetaObject::invokeMethod(this, [this, p](){
                    progressBar_->setValue(p);
                }, Qt::QueuedConnection);
            });
    });
    watcher_->setFuture(future);
}

void PerformanceLabWidget::updateBarChart(const QVector<BenchmarkResult>& results) {
    auto* barSet = new QBarSet("Tiempo (ms)");
    QStringList categories;
    double maxVal = 0.0;

    for (const auto& r : results) {
        if (r.timeMs < 0.0) continue;
        double val = r.timeMs;
        *barSet << val;
        categories << r.algorithmName;
        if (val > maxVal) maxVal = val;
    }

    auto* chart = new QChart();
    chart->setTheme(QChart::ChartThemeDark);

    if (!categories.isEmpty()) {
        auto* series = new QBarSeries();
        series->append(barSet);
        chart->addSeries(series);
        chart->setTitle(QString("Tiempos por Algoritmo (n=%1)").arg(results.isEmpty() ? 0 : results[0].dataSize));

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
    } else {
        chart->setTitle("Tiempos por Algoritmo (todos omitidos)");
    }

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

    for (auto* s : seriesMap) {
        auto pts = s->points();
        std::sort(pts.begin(), pts.end(), [](const QPointF& a, const QPointF& b){
            return a.x() < b.x();
        });
        s->replace(pts);
        chart->addSeries(s);
    }
    chart->createDefaultAxes();

    if (!chart->axes(Qt::Horizontal).isEmpty())
        if (auto* ax = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).first()))
            ax->setTitleText("Tamano de datos (n)");
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
            historyTable_->setItem(row, 3, new QTableWidgetItem(
                QString::number(r.comparisons)));
            historyTable_->setItem(row, 4, new QTableWidgetItem(
                QString::number(r.swaps)));
        } else {
            auto* item = new QTableWidgetItem("Omitido");
            item->setForeground(QColor("#F59E0B"));
            historyTable_->setItem(row, 2, item);
            historyTable_->setItem(row, 3, new QTableWidgetItem("--"));
            historyTable_->setItem(row, 4, new QTableWidgetItem("--"));
        }
        historyTable_->setItem(row, 5, new QTableWidgetItem(r.theoreticalComplexity));
        historyTable_->setItem(row, 6, new QTableWidgetItem(r.stability));
        historyTable_->setItem(row, 7, new QTableWidgetItem(r.extraMemory));
        historyTable_->setItem(row, 8, new QTableWidgetItem(r.dataType));
    }
    historyTable_->scrollToBottom();

    // Generate analysis if we have enough data
    generateAnalysis();
}

void PerformanceLabWidget::onSearchComparison() {
    int dataSize = getCurrentDataSize();
    auto& store = ClinicDataStore::instance();
    QString dsType = getDatasetType();
    QString field = getSortField();

    lblStatus_->setText("Ejecutando comparacion de busquedas...");
    auto res = Module4_PerformanceLab::compareSearchMethods(
        store.pacientes(), store.consultas(), dsType, field, dataSize);
    lblLinearTime_->setText(QString("Lineal: %1 ms").arg(res.linearMs, 0, 'f', 4));
    lblBinaryTime_->setText(QString("Binaria: %1 ms").arg(res.binaryMs, 0, 'f', 4));
    lblSortCost_->setText(QString("Costo orden: %1 ms").arg(res.sortMs, 0, 'f', 3));
    lblStatus_->setText("Comparacion de busquedas completada.");
}

void PerformanceLabWidget::generateAnalysis() {
    const auto& history = module_->getHistory();
    if (history.size() < 2) {
        lblAnalysis_->setText("Ejecute al menos 2 benchmarks con diferentes tamanos para generar conclusiones.");
        return;
    }

    // Gather results by algorithm
    QMap<QString, QVector<QPair<int, double>>> byAlg;
    for (const auto& batch : history) {
        for (const auto& r : batch) {
            if (r.timeMs >= 0.0)
                byAlg[r.algorithmName].push_back({r.dataSize, r.timeMs});
        }
    }

    if (byAlg.isEmpty()) {
        lblAnalysis_->setText("Sin datos suficientes para analisis.");
        return;
    }

    // Find fastest algorithm
    QString fastest;
    double fastestAvg = 1e18;
    for (auto it = byAlg.begin(); it != byAlg.end(); ++it) {
        double total = 0;
        for (const auto& p : it.value()) total += p.second;
        double avg = total / it.value().size();
        if (avg < fastestAvg) {
            fastestAvg = avg;
            fastest = it.key();
        }
    }

    QString analysis;
    analysis += "<b style='color:#1DBF73'>Conclusiones automaticas del benchmark:</b><br><br>";
    analysis += QString("Algoritmo mas rapido (promedio global): <b>%1</b> (%2 ms promedio)<br><br>")
        .arg(fastest).arg(fastestAvg, 0, 'f', 3);

    // Size-based analysis
    analysis += "<b>Comportamiento por tamano:</b><br>";
    QMap<int, QString> fastestBySize;
    QMap<int, double> fastestTimeBySize;
    for (const auto& batch : history) {
        for (const auto& r : batch) {
            if (r.timeMs < 0) continue;
            if (!fastestTimeBySize.contains(r.dataSize) || r.timeMs < fastestTimeBySize[r.dataSize]) {
                fastestTimeBySize[r.dataSize] = r.timeMs;
                fastestBySize[r.dataSize] = r.algorithmName;
            }
        }
    }
    for (auto it = fastestBySize.begin(); it != fastestBySize.end(); ++it) {
        analysis += QString("  n=%1: %2 (%3 ms)<br>")
            .arg(it.key()).arg(it.value()).arg(fastestTimeBySize[it.key()], 0, 'f', 3);
    }

    // QuickSort dominance analysis
    if (byAlg.contains("Quick Sort") && byAlg["Quick Sort"].size() >= 2) {
        analysis += "<br><b>QuickSort:</b> Con pivot median-of-three y cutoff a insercion (n&lt;16), ";
        analysis += "domina consistentemente a partir de n&gt;100. ";
        analysis += "Su complejidad O(n log n) promedio lo hace optimo para datos medianos y grandes.<br>";
    }

    // O(n^2) viability
    analysis += "<br><b>Algoritmos O(n^2):</b> Bubble Sort, Selection Sort e Insertion Sort ";
    analysis += "dejan de ser viables para n&gt;" + QString::number(SLOW_ALGORITHM_THRESHOLD) + ". ";
    analysis += "Insertion Sort es el mejor de los tres para datos parcialmente ordenados.<br>";

    // Binary search analysis
    analysis += "<br><b>Busqueda binaria vs lineal:</b> La busqueda binaria requiere costo previo de ordenamiento O(n log n). ";
    analysis += "Se justifica cuando se realizan multiples busquedas sobre el mismo dataset ordenado, ";
    analysis += "amortizando el costo del sort entre las consultas posteriores.";

    lblAnalysis_->setText(analysis);
}
