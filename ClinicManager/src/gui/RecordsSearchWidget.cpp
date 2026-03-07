#include "RecordsSearchWidget.h"
#include "../utils/DataGenerator.h"
#include "../algorithms/searching/LinearSearch.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QFont>
#include <QLabel>
#include <chrono>

RecordsSearchWidget::RecordsSearchWidget(QWidget* parent)
    : QWidget(parent) {
    module_ = new Module2_RecordsSearch(this);
    setupUI();
    loadSampleData(500);
}

void RecordsSearchWidget::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // Title
    auto* title = new QLabel("🔍 Búsqueda de Expedientes Médicos");
    QFont f = title->font(); f.setPointSize(14); f.setBold(true);
    title->setFont(f);
    title->setObjectName("sectionTitle");
    mainLayout->addWidget(title);

    // Search tabs
    searchTabs_ = new QTabWidget(this);

    // --- Tab: Cédula ---
    auto* cedulaWidget = new QWidget();
    auto* cedulaLayout = new QHBoxLayout(cedulaWidget);
    cedulaLayout->setContentsMargins(10, 10, 10, 10);
    edCedula_ = new QLineEdit();
    edCedula_->setPlaceholderText("Ej: 1-1234-5678");
    btnCedula_ = new QPushButton("Buscar por Cédula (HashTable O(1))");
    btnCedula_->setObjectName("primaryButton");
    cedulaLayout->addWidget(new QLabel("Cédula:"));
    cedulaLayout->addWidget(edCedula_, 1);
    cedulaLayout->addWidget(btnCedula_);
    searchTabs_->addTab(cedulaWidget, "Por Cédula");

    // --- Tab: Nombre ---
    auto* nameWidget = new QWidget();
    auto* nameLayout = new QHBoxLayout(nameWidget);
    nameLayout->setContentsMargins(10, 10, 10, 10);
    edName_ = new QLineEdit();
    edName_->setPlaceholderText("Ej: María González");
    btnName_ = new QPushButton("Buscar por Nombre (Búsqueda Lineal)");
    btnName_->setObjectName("primaryButton");
    nameLayout->addWidget(new QLabel("Nombre:"));
    nameLayout->addWidget(edName_, 1);
    nameLayout->addWidget(btnName_);
    searchTabs_->addTab(nameWidget, "Por Nombre");

    // --- Tab: Fecha ---
    auto* dateWidget = new QWidget();
    auto* dateLayout = new QHBoxLayout(dateWidget);
    dateLayout->setContentsMargins(10, 10, 10, 10);
    dateFrom_ = new QDateEdit(QDate(2022, 1, 1));
    dateFrom_->setCalendarPopup(true);
    dateFrom_->setDisplayFormat("yyyy-MM-dd");
    dateTo_   = new QDateEdit(QDate::currentDate());
    dateTo_->setCalendarPopup(true);
    dateTo_->setDisplayFormat("yyyy-MM-dd");
    btnDate_ = new QPushButton("Buscar por Rango (Búsqueda Binaria)");
    btnDate_->setObjectName("primaryButton");
    dateLayout->addWidget(new QLabel("Desde:"));
    dateLayout->addWidget(dateFrom_);
    dateLayout->addWidget(new QLabel("Hasta:"));
    dateLayout->addWidget(dateTo_);
    dateLayout->addWidget(btnDate_);
    searchTabs_->addTab(dateWidget, "Por Fecha");

    // --- Tab: Gravedad ---
    auto* gravWidget = new QWidget();
    auto* gravLayout = new QHBoxLayout(gravWidget);
    gravLayout->setContentsMargins(10, 10, 10, 10);
    cbGravedad_ = new QComboBox();
    cbGravedad_->addItems({"1 - Muy leve", "2 - Leve", "3 - Moderada",
                            "4 - Grave", "5 - Crítica"});
    btnGravedad_ = new QPushButton("Buscar por Gravedad");
    btnGravedad_->setObjectName("primaryButton");
    gravLayout->addWidget(new QLabel("Gravedad:"));
    gravLayout->addWidget(cbGravedad_);
    gravLayout->addWidget(btnGravedad_);
    gravLayout->addStretch();
    searchTabs_->addTab(gravWidget, "Por Gravedad");

    mainLayout->addWidget(searchTabs_);

    // Results info bar
    auto* infoLayout = new QHBoxLayout();
    lblTime_   = new QLabel("Tiempo: —");
    lblCount_  = new QLabel("Resultados: 0");
    lblLinear_ = new QLabel("Lineal: —");
    lblBinary_ = new QLabel("Binaria: —");
    infoLayout->addWidget(lblTime_);
    infoLayout->addWidget(new QLabel("|"));
    infoLayout->addWidget(lblCount_);
    infoLayout->addStretch();
    infoLayout->addWidget(new QLabel("Comparación:"));
    infoLayout->addWidget(lblLinear_);
    infoLayout->addWidget(lblBinary_);
    mainLayout->addLayout(infoLayout);

    // Results table
    resultsTable_ = new QTableWidget(0, 6, this);
    resultsTable_->setHorizontalHeaderLabels(
        {"Cédula / ID", "Nombre / Médico", "Fecha", "Diagnóstico", "Campo4", "Campo5"});
    resultsTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    resultsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    resultsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    resultsTable_->setAlternatingRowColors(true);
    resultsTable_->verticalHeader()->setVisible(false);
    mainLayout->addWidget(resultsTable_, 1);

    connect(btnCedula_,  &QPushButton::clicked, this, &RecordsSearchWidget::onSearchByCedula);
    connect(btnName_,    &QPushButton::clicked, this, &RecordsSearchWidget::onSearchByName);
    connect(btnDate_,    &QPushButton::clicked, this, &RecordsSearchWidget::onSearchByDate);
    connect(btnGravedad_,&QPushButton::clicked, this, &RecordsSearchWidget::onSearchByGravedad);
    connect(edCedula_,   &QLineEdit::returnPressed, this, &RecordsSearchWidget::onSearchByCedula);
    connect(edName_,     &QLineEdit::returnPressed, this, &RecordsSearchWidget::onSearchByName);
}

void RecordsSearchWidget::loadSampleData(int count) {
    auto pacientes = DataGenerator::generatePacientes(count);
    auto consultas = DataGenerator::generateConsultas(pacientes, count * 2);
    module_->loadData(pacientes, consultas);
}

int RecordsSearchWidget::pacienteCount() const {
    return module_->pacienteCount();
}

void RecordsSearchWidget::showPacienteResults(const QVector<Paciente>& results) {
    resultsTable_->setColumnCount(6);
    resultsTable_->setHorizontalHeaderLabels(
        {"Cédula", "Nombre", "Edad", "Cantón", "Diagnóstico", "Prioridad"});
    resultsTable_->setRowCount(results.size());
    for (int i = 0; i < results.size(); ++i) {
        const Paciente& p = results[i];
        resultsTable_->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(p.cedula)));
        resultsTable_->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(p.nombre)));
        resultsTable_->setItem(i, 2, new QTableWidgetItem(QString::number(p.edad)));
        resultsTable_->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(p.canton)));
        resultsTable_->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(p.diagnostico)));
        resultsTable_->setItem(i, 5, new QTableWidgetItem(QString::number(p.prioridad)));
    }
    lblCount_->setText(QString("Resultados: %1").arg(results.size()));
    lblTime_->setText(QString("Tiempo: %1 ms").arg(module_->getLastSearchTimeMs(), 0, 'f', 4));
}

void RecordsSearchWidget::showConsultaResults(const QVector<Consulta>& results) {
    resultsTable_->setColumnCount(6);
    resultsTable_->setHorizontalHeaderLabels(
        {"ID Consulta", "Cédula Paciente", "Fecha", "Médico", "Diagnóstico", "Gravedad"});
    resultsTable_->setRowCount(results.size());
    for (int i = 0; i < results.size(); ++i) {
        const Consulta& c = results[i];
        resultsTable_->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(c.idConsulta)));
        resultsTable_->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(c.cedulaPaciente)));
        resultsTable_->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(c.fecha)));
        resultsTable_->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(c.medicoTratante)));
        resultsTable_->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(c.diagnostico)));
        resultsTable_->setItem(i, 5, new QTableWidgetItem(QString::number(c.gravedad)));
    }
    lblCount_->setText(QString("Resultados: %1").arg(results.size()));
    lblTime_->setText(QString("Tiempo: %1 ms").arg(module_->getLastSearchTimeMs(), 0, 'f', 4));
}

void RecordsSearchWidget::onSearchByCedula() {
    QString cedula = edCedula_->text().trimmed();
    if (cedula.isEmpty()) return;
    Paciente p = module_->searchByCedula(cedula);
    QVector<Paciente> results;
    if (!p.cedula.empty()) results.push_back(p);
    showPacienteResults(results);
}

void RecordsSearchWidget::onSearchByName() {
    QString name = edName_->text().trimmed();
    if (name.isEmpty()) return;
    auto results = module_->searchByName(name);
    showPacienteResults(results);
}

void RecordsSearchWidget::onSearchByDate() {
    QString from = dateFrom_->date().toString("yyyy-MM-dd");
    QString to   = dateTo_->date().toString("yyyy-MM-dd");

    // Binary search
    auto results = module_->searchByDateRange(from, to);
    showConsultaResults(results);
    double binaryTime = module_->getLastSearchTimeMs();

    // Linear search for comparison (don't show - just time)
    auto t0 = std::chrono::high_resolution_clock::now();
    QString sFrom = from, sTo = to;
    QVector<Consulta> all = module_->getAllConsultas();
    std::string sf = sFrom.toStdString(), st = sTo.toStdString();
    QVector<Consulta> linear = linearSearchAll<Consulta>(all, [&](const Consulta& c){
        return c.fecha >= sf && c.fecha <= st;
    });
    double linearTime = std::chrono::duration<double, std::milli>(
        std::chrono::high_resolution_clock::now() - t0).count();

    lblLinear_->setText(QString("Lineal: %1 ms").arg(linearTime, 0, 'f', 3));
    lblBinary_->setText(QString("Binaria: %1 ms").arg(binaryTime, 0, 'f', 3));
}

void RecordsSearchWidget::onSearchByGravedad() {
    int gravedad = cbGravedad_->currentIndex() + 1;
    auto results = module_->searchByGravedad(gravedad);
    showConsultaResults(results);
}
