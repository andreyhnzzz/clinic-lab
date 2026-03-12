#include "MainWindow.h"
#include "AttentionQueueWidget.h"
#include "RecordsSearchWidget.h"
#include "DiagnosisTreeWidget.h"
#include "PerformanceLabWidget.h"
<<<<<<< HEAD
#include "../core/ClinicDataStore.h"
=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
#include <QApplication>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>
<<<<<<< HEAD
#include <QHBoxLayout>
#include <QTimer>
#include <QFile>
#include <QFrame>
#include <QPushButton>
#include <QSplitter>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setWindowTitle("Clinica Management System");
    setMinimumSize(1100, 750);
    resize(1360, 860);
    setupUI();
    setupMenuBar();
    setupStatusBar();
=======
#include <QTimer>
#include <QFile>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setWindowTitle("Clínica Management System — Costa Rica");
    setMinimumSize(1100, 750);
    resize(1280, 800);
    setupUI();
    setupMenuBar();
    setupStatusBar();
    loadStyleSheet();
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31

    // Update status every 30 seconds
    auto* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateStatusBar);
    timer->start(30000);
<<<<<<< HEAD

    // Select first module
    switchModule(0);
}

void MainWindow::setupUI() {
    auto* centralWidget = new QWidget(this);
    auto* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Sidebar
    auto* sidebar = new QFrame();
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(220);
    auto* sideLayout = new QVBoxLayout(sidebar);
    sideLayout->setSpacing(2);
    sideLayout->setContentsMargins(8, 12, 8, 12);

    // Sidebar title
    auto* logoLabel = new QLabel("Clinica CR");
    logoLabel->setObjectName("sidebarTitle");
    logoLabel->setAlignment(Qt::AlignCenter);
    sideLayout->addWidget(logoLabel);

    auto* subtitleLabel = new QLabel("Sistema de Gestion");
    subtitleLabel->setObjectName("sidebarSubtitle");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    sideLayout->addWidget(subtitleLabel);
    sideLayout->addSpacing(16);

    // Navigation buttons
    struct NavItem { QString label; };
    NavItem items[] = {
        {"Cola de Atencion"},
        {"Expedientes"},
        {"Arbol de Diagnosticos"},
        {"Lab. de Rendimiento"}
    };

    for (int i = 0; i < 4; ++i) {
        auto* btn = new QPushButton(items[i].label);
        btn->setObjectName("navButton");
        btn->setCheckable(true);
        btn->setMinimumHeight(40);
        connect(btn, &QPushButton::clicked, this, [this, i]() { switchModule(i); });
        sideLayout->addWidget(btn);
        navButtons_.push_back(btn);
    }

    sideLayout->addStretch();

    // Sidebar footer info
    auto* footerLabel = new QLabel("v1.0.0 | C++17 / Qt6");
    footerLabel->setObjectName("sidebarFooter");
    footerLabel->setAlignment(Qt::AlignCenter);
    sideLayout->addWidget(footerLabel);

    mainLayout->addWidget(sidebar);

    // Content area
    auto* contentFrame = new QFrame();
    contentFrame->setObjectName("contentArea");
    auto* contentLayout = new QVBoxLayout(contentFrame);
    contentLayout->setSpacing(0);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    // Header
    auto* header = new QFrame();
    header->setObjectName("header");
    header->setFixedHeight(48);
    auto* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(16, 0, 16, 0);
    headerTitle_ = new QLabel("Cola de Atencion");
    headerTitle_->setObjectName("headerTitle");
    headerLayout->addWidget(headerTitle_);
    headerLayout->addStretch();
    contentLayout->addWidget(header);

    // Stacked widget for module content
    stack_ = new QStackedWidget(this);
=======
}

void MainWindow::setupUI() {
    tabs_ = new QTabWidget(this);
    tabs_->setDocumentMode(false);

>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    queueWidget_  = new AttentionQueueWidget(this);
    searchWidget_ = new RecordsSearchWidget(this);
    treeWidget_   = new DiagnosisTreeWidget(this);
    perfWidget_   = new PerformanceLabWidget(this);

<<<<<<< HEAD
    stack_->addWidget(queueWidget_);
    stack_->addWidget(searchWidget_);
    stack_->addWidget(treeWidget_);
    stack_->addWidget(perfWidget_);
    contentLayout->addWidget(stack_, 1);

    mainLayout->addWidget(contentFrame, 1);

    setCentralWidget(centralWidget);
=======
    tabs_->addTab(queueWidget_,  "🏥  Cola de Atención");
    tabs_->addTab(searchWidget_, "🔍  Búsqueda de Expedientes");
    tabs_->addTab(treeWidget_,   "🌳  Árbol de Diagnósticos");
    tabs_->addTab(perfWidget_,   "📊  Laboratorio de Rendimiento");

    setCentralWidget(tabs_);
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31

    connect(queueWidget_, &AttentionQueueWidget::queueSizeChanged,
            this, &MainWindow::updateStatusBar);
}

<<<<<<< HEAD
void MainWindow::switchModule(int index) {
    if (index < 0 || index >= stack_->count()) return;
    stack_->setCurrentIndex(index);

    // Update nav buttons
    for (int i = 0; i < navButtons_.size(); ++i)
        navButtons_[i]->setChecked(i == index);

    // Update header
    static const char* titles[] = {
        "Cola de Atencion",
        "Expedientes Clinicos",
        "Arbol de Diagnosticos",
        "Laboratorio de Rendimiento"
    };
    headerTitle_->setText(titles[index]);
}

=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
void MainWindow::setupMenuBar() {
    QMenu* archivo = menuBar()->addMenu("&Archivo");

    QAction* actLoad = archivo->addAction("Cargar datos de muestra");
    connect(actLoad, &QAction::triggered, this, &MainWindow::onLoadSampleData);

    archivo->addSeparator();
    QAction* actExit = archivo->addAction("Salir");
    actExit->setShortcut(QKeySequence::Quit);
    connect(actExit, &QAction::triggered, qApp, &QApplication::quit);

    QMenu* ayuda = menuBar()->addMenu("&Ayuda");
    QAction* actAbout = ayuda->addAction("Acerca de...");
    connect(actAbout, &QAction::triggered, this, &MainWindow::onAbout);
}

void MainWindow::setupStatusBar() {
    statusPatients_ = new QLabel("Pacientes: 0");
    statusQueue_    = new QLabel("En cola: 0");
    statusTime_     = new QLabel(QDateTime::currentDateTime().toString("hh:mm:ss"));

    statusBar()->addWidget(statusPatients_);
    statusBar()->addWidget(new QLabel(" | "));
    statusBar()->addWidget(statusQueue_);
    statusBar()->addWidget(new QLabel(" | "));
    statusBar()->addPermanentWidget(statusTime_);

    auto* clockTimer = new QTimer(this);
    connect(clockTimer, &QTimer::timeout, this, [this](){
        statusTime_->setText(QDateTime::currentDateTime().toString("hh:mm:ss"));
    });
    clockTimer->start(1000);
<<<<<<< HEAD

    updateStatusBar();
}

void MainWindow::updateStatusBar() {
    statusPatients_->setText(QString("Pacientes: %1").arg(
        ClinicDataStore::instance().pacienteCount()));
=======
}

void MainWindow::loadStyleSheet() {
    QFile f(":/styles.qss");
    if (!f.exists()) {
        // Try relative path
        f.setFileName("resources/styles.qss");
    }
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qApp->setStyleSheet(QString::fromUtf8(f.readAll()));
        f.close();
    }
}

void MainWindow::updateStatusBar() {
    statusPatients_->setText(QString("Pacientes: %1").arg(searchWidget_->pacienteCount()));
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    statusQueue_->setText(QString("En cola: %1").arg(queueWidget_->queueSize()));
}

void MainWindow::onLoadSampleData() {
<<<<<<< HEAD
    ClinicDataStore::instance().generateSampleData(500);
=======
    searchWidget_->loadSampleData(500);
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    updateStatusBar();
    statusBar()->showMessage("Datos de muestra cargados (500 pacientes)", 3000);
}

void MainWindow::onAbout() {
<<<<<<< HEAD
    QMessageBox::about(this, "Acerca de Clinica Management System",
        "<h2>Clinica Management System</h2>"
        "<p>Sistema de gestion para clinica privada costarricense.</p>"
        "<p><b>Version:</b> 1.0.0</p>"
        "<p><b>Tecnologias:</b> C++17, Qt6, Estructuras de datos propias</p>"
        "<p><b>Modulos:</b></p>"
        "<ul>"
        "<li>Cola de Atencion con Min-Heap y desempate por orden de llegada</li>"
        "<li>Expedientes Clinicos con HashTable y Busqueda Binaria</li>"
        "<li>Arbol de Diagnosticos N-ario (ICD-10)</li>"
        "<li>Laboratorio de Rendimiento con datos reales</li>"
        "</ul>"
        "<p><b>Repositorio central de datos:</b> ClinicDataStore (singleton)</p>"
=======
    QMessageBox::about(this, "Acerca de Clínica Management System",
        "<h2>Clínica Management System</h2>"
        "<p>Sistema de gestión para clínica privada costarricense.</p>"
        "<p><b>Versión:</b> 1.0.0</p>"
        "<p><b>Tecnologías:</b> C++17, Qt6, Estructuras de datos propias</p>"
        "<p><b>Módulos:</b></p>"
        "<ul>"
        "<li>Cola de Atención con Min-Heap</li>"
        "<li>Búsqueda de Expedientes con HashTable y Búsqueda Binaria</li>"
        "<li>Árbol de Diagnósticos N-ario</li>"
        "<li>Laboratorio de Rendimiento de Algoritmos</li>"
        "</ul>"
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
        "<p>Proyecto universitario — Estructuras de Datos</p>");
}
