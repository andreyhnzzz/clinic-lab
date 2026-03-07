#include "MainWindow.h"
#include "AttentionQueueWidget.h"
#include "RecordsSearchWidget.h"
#include "DiagnosisTreeWidget.h"
#include "PerformanceLabWidget.h"
#include <QApplication>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>
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

    // Update status every 30 seconds
    auto* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateStatusBar);
    timer->start(30000);
}

void MainWindow::setupUI() {
    tabs_ = new QTabWidget(this);
    tabs_->setDocumentMode(false);

    queueWidget_  = new AttentionQueueWidget(this);
    searchWidget_ = new RecordsSearchWidget(this);
    treeWidget_   = new DiagnosisTreeWidget(this);
    perfWidget_   = new PerformanceLabWidget(this);

    tabs_->addTab(queueWidget_,  "🏥  Cola de Atención");
    tabs_->addTab(searchWidget_, "🔍  Búsqueda de Expedientes");
    tabs_->addTab(treeWidget_,   "🌳  Árbol de Diagnósticos");
    tabs_->addTab(perfWidget_,   "📊  Laboratorio de Rendimiento");

    setCentralWidget(tabs_);

    connect(queueWidget_, &AttentionQueueWidget::queueSizeChanged,
            this, &MainWindow::updateStatusBar);
}

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
    statusQueue_->setText(QString("En cola: %1").arg(queueWidget_->queueSize()));
}

void MainWindow::onLoadSampleData() {
    searchWidget_->loadSampleData(500);
    updateStatusBar();
    statusBar()->showMessage("Datos de muestra cargados (500 pacientes)", 3000);
}

void MainWindow::onAbout() {
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
        "<p>Proyecto universitario — Estructuras de Datos</p>");
}
