#include "MainWindow.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QProgressDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QLabel>
#include <QFrame>
#include <QFont>
#include <QFile>
#include <QTimer>

#include "PatientTableWidget.h"
#include "DiagnosisTreeWidget.h"
#include "PerformanceLabWindow.h"
#include "CSVExporter.h"

// ── Constructor ──────────────────────────────────────────────────────────────

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_generator(std::make_unique<DataGenerator>(this))
{
    setWindowTitle("ClinicManager – Sistema de Gestión de Clínica");
    setMinimumSize(1200, 750);
    resize(1440, 860);

    connect(m_generator.get(), &DataGenerator::progressChanged,
            this, &MainWindow::onDataProgress);

    setupUi();
    setupMenu();
    applyTheme();

    // Generate initial small dataset on startup
    QTimer::singleShot(0, this, &MainWindow::onGenerateData);
}

MainWindow::~MainWindow() = default;

// ── UI Setup ──────────────────────────────────────────────────────────────────

void MainWindow::setupUi()
{
    // ── Navigation list (left panel) ─────────────────────────────────────────
    m_navList = new QListWidget(this);
    m_navList->setObjectName("navList");
    m_navList->setMaximumWidth(220);
    m_navList->setMinimumWidth(180);
    m_navList->setIconSize(QSize(24, 24));

    struct ModuleEntry { QString icon; QString label; };
    const QList<ModuleEntry> modules = {
        { ":/icons/queue.png",   "Módulo 1 – Cola de Prioridad"        },
        { ":/icons/search.png",  "Módulo 2 – Búsqueda Lineal/Binaria"  },
        { ":/icons/tree.png",    "Módulo 3 – Árbol de Diagnósticos"    },
        { ":/icons/chart.png",   "Módulo 4 – Benchmark de Orden"       },
    };

    for (const auto &m : modules) {
        auto *item = new QListWidgetItem(QIcon(m.icon), m.label);
        item->setSizeHint(QSize(0, 44));
        m_navList->addItem(item);
    }
    m_navList->setCurrentRow(0);

    // ── Stacked widget (right panel) ─────────────────────────────────────────
    m_stack = new QStackedWidget(this);

    m_patientPage = new PatientTableWidget(this);
    m_diagPage    = new DiagnosisTreeWidget(this);
    m_perfPage    = new PerformanceLabWindow(this);

    // index 0 – Module 1 & 2  (PatientTableWidget handles both modes)
    m_stack->addWidget(m_patientPage);
    // index 1 – Module 3
    m_stack->addWidget(m_diagPage);
    // index 2 – Module 4
    m_stack->addWidget(m_perfPage);

    // ── Splitter ──────────────────────────────────────────────────────────────
    m_splitter = new QSplitter(Qt::Horizontal, this);
    m_splitter->addWidget(m_navList);
    m_splitter->addWidget(m_stack);
    m_splitter->setStretchFactor(0, 0);
    m_splitter->setStretchFactor(1, 1);
    m_splitter->setHandleWidth(2);

    setCentralWidget(m_splitter);

    // ── Status bar ────────────────────────────────────────────────────────────
    m_statusLabel = new QLabel("Listo", this);
    statusBar()->addPermanentWidget(m_statusLabel);

    connect(m_navList, &QListWidget::currentRowChanged,
            this, &MainWindow::onModuleSelected);
}

void MainWindow::setupMenu()
{
    // ── File menu ─────────────────────────────────────────────────────────────
    QMenu *fileMenu = menuBar()->addMenu("&Archivo");

    QAction *actGenSmall  = fileMenu->addAction("Generar 500 pacientes");
    QAction *actGenMedium = fileMenu->addAction("Generar 5,000 pacientes");
    QAction *actGenLarge  = fileMenu->addAction("Generar 50,000 pacientes");
    QAction *actGenXL     = fileMenu->addAction("Generar 200,000 pacientes");
    fileMenu->addSeparator();
    QAction *actExport    = fileMenu->addAction("&Exportar CSV...");
    fileMenu->addSeparator();
    QAction *actQuit      = fileMenu->addAction("&Salir");

    connect(actGenSmall,  &QAction::triggered, this,
            [this]{ m_generator->setSeed(42); onGenerateDataSize(DataGenerator::DatasetSize::Small); });
    connect(actGenMedium, &QAction::triggered, this,
            [this]{ m_generator->setSeed(42); onGenerateDataSize(DataGenerator::DatasetSize::Medium); });
    connect(actGenLarge,  &QAction::triggered, this,
            [this]{ m_generator->setSeed(42); onGenerateDataSize(DataGenerator::DatasetSize::Large); });
    connect(actGenXL,     &QAction::triggered, this,
            [this]{ m_generator->setSeed(42); onGenerateDataSize(DataGenerator::DatasetSize::XLarge); });

    connect(actExport, &QAction::triggered, this, [this]{
        QString path = QFileDialog::getSaveFileName(this, "Exportar CSV", "pacientes.csv",
                                                    "CSV (*.csv)");
        if (path.isEmpty()) return;
        QStringList headers = {"Cédula","Nombre","Edad","Fecha Registro",
                               "Prioridad","Tipo Sangre","Diagnóstico"};
        QList<QStringList> rows;
        for (const Patient &p : m_patients) {
            rows << QStringList{
                p.cedula(), p.nombre(), QString::number(p.edad()),
                p.fechaReg().toString("dd/MM/yyyy"),
                Patient::priorityLabel(p.priorityEnum()),
                p.tipoSangre(), p.diagnostico()
            };
        }
        QString err;
        if (!CSVExporter::exportData(headers, rows, path, err))
            QMessageBox::warning(this, "Error", err);
        else
            updateStatusBar("CSV exportado: " + path);
    });

    connect(actQuit, &QAction::triggered, qApp, &QApplication::quit);

    // ── Help menu ─────────────────────────────────────────────────────────────
    QMenu *helpMenu = menuBar()->addMenu("A&yuda");
    QAction *actAbout = helpMenu->addAction("&Acerca de ClinicManager");
    connect(actAbout, &QAction::triggered, this, [this]{
        QMessageBox::about(this, "Acerca de ClinicManager",
            "<h2>ClinicManager v1.0</h2>"
            "<p>Sistema de Gestión de Clínica – UTN Costa Rica</p>"
            "<p>Estructuras de Datos – IV Cuatrimestre</p>");
    });
}

void MainWindow::applyTheme()
{
    QFile qss(":/styles/theme.qss");
    if (qss.open(QFile::ReadOnly)) {
        qApp->setStyleSheet(QString::fromUtf8(qss.readAll()));
    }
}

// ── Slots ─────────────────────────────────────────────────────────────────────

void MainWindow::onModuleSelected(int row)
{
    switch (row) {
    case 0:
        m_stack->setCurrentWidget(m_patientPage);
        m_patientPage->setMode(PatientTableWidget::Mode::PriorityQueue);
        break;
    case 1:
        m_stack->setCurrentWidget(m_patientPage);
        m_patientPage->setMode(PatientTableWidget::Mode::Search);
        break;
    case 2:
        m_stack->setCurrentWidget(m_diagPage);
        break;
    case 3:
        m_stack->setCurrentWidget(m_perfPage);
        break;
    default:
        break;
    }
}

void MainWindow::onGenerateData()
{
    onGenerateDataSize(DataGenerator::DatasetSize::Small);
}

void MainWindow::onGenerateDataSize(DataGenerator::DatasetSize size)
{
    updateStatusBar("Generando datos...");

    m_patients  = m_generator->generatePatients(size);
    m_consultas = m_generator->generateConsultas(m_patients, size);
    m_diagTree  = m_generator->buildDiagnosisTree();

    m_patientPage->setPatients(m_patients);
    m_diagPage->setTree(m_diagTree);
    m_perfPage->setPatients(m_patients);

    updateStatusBar(QString("Datos cargados: %1 pacientes, %2 consultas")
                    .arg(m_patients.size())
                    .arg(m_consultas.size()));
}

void MainWindow::onDataProgress(int percent)
{
    updateStatusBar(QString("Generando... %1%").arg(percent));
}

void MainWindow::updateStatusBar(const QString &msg)
{
    if (m_statusLabel) m_statusLabel->setText(msg);
}

