#include "PerformanceLabWindow.h"

#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>

#include "CSVExporter.h"
#include "DataGenerator.h"

PerformanceLabWindow::PerformanceLabWindow(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    connect(&m_bench, &SortBenchmark::algorithmFinished,
            this, &PerformanceLabWindow::onAlgorithmFinished);
    connect(&m_bench, &SortBenchmark::allFinished,
            this, &PerformanceLabWindow::onAllFinished);
}

// ── UI Setup ──────────────────────────────────────────────────────────────────

void PerformanceLabWindow::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(6);

    // ── Control bar ───────────────────────────────────────────────────────────
    auto *ctrlGroup = new QGroupBox("Configuración del Benchmark", this);
    auto *ctrlLayout = new QHBoxLayout(ctrlGroup);

    ctrlLayout->addWidget(new QLabel("Tamaño:"));
    m_sizeCombo = new QComboBox(this);
    m_sizeCombo->addItem("500",    static_cast<int>(DataGenerator::DatasetSize::Small));
    m_sizeCombo->addItem("5,000",  static_cast<int>(DataGenerator::DatasetSize::Medium));
    m_sizeCombo->addItem("50,000", static_cast<int>(DataGenerator::DatasetSize::Large));
    m_sizeCombo->addItem("200,000",static_cast<int>(DataGenerator::DatasetSize::XLarge));
    ctrlLayout->addWidget(m_sizeCombo);

    m_runBtn = new QPushButton("▶  Ejecutar Benchmark", this);
    m_runBtn->setObjectName("primaryButton");
    ctrlLayout->addWidget(m_runBtn);

    m_exportBtn = new QPushButton("Exportar CSV", this);
    ctrlLayout->addWidget(m_exportBtn);

    ctrlLayout->addStretch();

    m_statusLabel = new QLabel("Listo.", this);
    ctrlLayout->addWidget(m_statusLabel);

    mainLayout->addWidget(ctrlGroup);

    // ── Progress bar ──────────────────────────────────────────────────────────
    m_progress = new QProgressBar(this);
    m_progress->setRange(0, 100);
    m_progress->setValue(0);
    m_progress->setVisible(false);
    mainLayout->addWidget(m_progress);

    // ── Splitter: history table | charts ─────────────────────────────────────
    auto *splitter = new QSplitter(Qt::Vertical, this);

    // History table
    m_historyTable = new QTableWidget(this);
    m_historyTable->setColumnCount(5);
    m_historyTable->setHorizontalHeaderLabels(
        {"Algoritmo", "Tamaño", "Tiempo (µs)", "Comparaciones", "Intercambios"});
    m_historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_historyTable->setAlternatingRowColors(true);
    splitter->addWidget(m_historyTable);

    // Charts
    m_charts = new ChartWidget(this);
    splitter->addWidget(m_charts);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);

    mainLayout->addWidget(splitter, 1);

    // ── Connections ───────────────────────────────────────────────────────────
    connect(m_runBtn,    &QPushButton::clicked, this, &PerformanceLabWindow::onRunBenchmark);
    connect(m_exportBtn, &QPushButton::clicked, this, &PerformanceLabWindow::onExportCsv);
    connect(&m_bench, &SortBenchmark::progressChanged, m_progress, &QProgressBar::setValue);
}

// ── Public API ────────────────────────────────────────────────────────────────

void PerformanceLabWindow::setPatients(const QVector<Patient> &patients)
{
    m_patients = patients;
}

// ── Slots ─────────────────────────────────────────────────────────────────────

void PerformanceLabWindow::onRunBenchmark()
{
    if (m_patients.isEmpty()) {
        m_statusLabel->setText("Sin datos. Genere pacientes primero.");
        return;
    }

    int targetSize = m_sizeCombo->currentData().toInt();

    // If we have more data than requested, use a subset; if less, use all
    QVector<Patient> subset = (m_patients.size() > targetSize)
        ? m_patients.mid(0, targetSize)
        : m_patients;

    m_runBtn->setEnabled(false);
    m_progress->setValue(0);
    m_progress->setVisible(true);
    m_statusLabel->setText("Ejecutando benchmark...");

    // For sizes > 5000 run in a background thread to keep UI responsive
    if (subset.size() > 5000) {
        auto *watcher = new QFutureWatcher<QVector<SortBenchmarkResult>>(this);
        connect(watcher, &QFutureWatcher<QVector<SortBenchmarkResult>>::finished,
                this, [this, watcher]{
                    onAllFinished(watcher->result());
                    watcher->deleteLater();
                });
        watcher->setFuture(QtConcurrent::run([this, subset]() mutable {
            return m_bench.runAll(subset);
        }));
    } else {
        m_bench.runAll(subset);  // synchronous for small datasets
    }
}

void PerformanceLabWindow::onAlgorithmFinished(const SortBenchmarkResult &result)
{
    addResultToHistory(result);
    m_statusLabel->setText(QString("Completado: %1 – %2 µs")
                           .arg(result.algorithm)
                           .arg(result.elapsedUs));
}

void PerformanceLabWindow::onAllFinished(const QVector<SortBenchmarkResult> &results)
{
    m_runBtn->setEnabled(true);
    m_progress->setVisible(false);
    m_statusLabel->setText(QString("Benchmark finalizado – %1 algoritmos").arg(results.size()));

    // Update bar chart
    m_charts->setBarData(results);

    // Update growth curves
    for (const auto &r : results) {
        m_charts->appendGrowthPoint(r.algorithm, r.dataSize, r.elapsedUs);
        m_growthData[r.algorithm].append({r.dataSize, r.elapsedUs});
    }

    // Also add any results not yet in history (for async path)
    for (const auto &r : results) {
        bool found = false;
        for (int row = 0; row < m_historyTable->rowCount(); ++row) {
            if (m_historyTable->item(row, 0) &&
                m_historyTable->item(row, 0)->text() == r.algorithm &&
                m_historyTable->item(row, 1) &&
                m_historyTable->item(row, 1)->text() == QString::number(r.dataSize)) {
                found = true;
                break;
            }
        }
        if (!found) addResultToHistory(r);
    }
}

void PerformanceLabWindow::addResultToHistory(const SortBenchmarkResult &r)
{
    int row = m_historyTable->rowCount();
    m_historyTable->insertRow(row);
    m_historyTable->setItem(row, 0, new QTableWidgetItem(r.algorithm));
    m_historyTable->setItem(row, 1, new QTableWidgetItem(QString::number(r.dataSize)));
    m_historyTable->setItem(row, 2, new QTableWidgetItem(QString::number(r.elapsedUs)));
    m_historyTable->setItem(row, 3, new QTableWidgetItem(QString::number(r.comparisons)));
    m_historyTable->setItem(row, 4, new QTableWidgetItem(QString::number(r.swaps)));
}

void PerformanceLabWindow::onExportCsv()
{
    QString path = QFileDialog::getSaveFileName(this, "Exportar benchmark", "benchmark.csv",
                                                "CSV (*.csv)");
    if (path.isEmpty()) return;

    QStringList headers = {"Algoritmo", "Tamaño", "Tiempo (µs)", "Comparaciones", "Intercambios"};
    QList<QStringList> rows;
    for (int r = 0; r < m_historyTable->rowCount(); ++r) {
        QStringList row;
        for (int c = 0; c < m_historyTable->columnCount(); ++c)
            row << (m_historyTable->item(r, c) ? m_historyTable->item(r, c)->text() : "");
        rows << row;
    }
    QString err;
    if (!CSVExporter::exportData(headers, rows, path, err))
        QMessageBox::warning(this, "Error", err);
    else
        m_statusLabel->setText("CSV exportado: " + path);
}
