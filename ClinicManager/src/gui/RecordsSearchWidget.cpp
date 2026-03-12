#include "RecordsSearchWidget.h"
#include "../utils/DataGenerator.h"
<<<<<<< HEAD
#include "../core/ClinicDataStore.h"
=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
#include "../algorithms/searching/LinearSearch.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
<<<<<<< HEAD
#include <QSplitter>
#include <QHeaderView>
#include <QFont>
#include <QLabel>
#include <QTextEdit>
=======
#include <QHeaderView>
#include <QFont>
#include <QLabel>
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
#include <chrono>

RecordsSearchWidget::RecordsSearchWidget(QWidget* parent)
    : QWidget(parent) {
    module_ = new Module2_RecordsSearch(this);
    setupUI();
<<<<<<< HEAD

    // Load data from central store
    auto& store = ClinicDataStore::instance();
    if (store.pacienteCount() == 0)
        store.generateSampleData(500);
    module_->loadData(store.pacientes(), store.consultas());

    connect(&store, &ClinicDataStore::dataChanged, this, [this]() {
        module_->loadData(ClinicDataStore::instance().pacientes(),
                          ClinicDataStore::instance().consultas());
    });
=======
    loadSampleData(500);
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
}

void RecordsSearchWidget::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
<<<<<<< HEAD
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(12, 12, 12, 12);
=======
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // Title
    auto* title = new QLabel("🔍 Búsqueda de Expedientes Médicos");
    QFont f = title->font(); f.setPointSize(14); f.setBold(true);
    title->setFont(f);
    title->setObjectName("sectionTitle");
    mainLayout->addWidget(title);
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31

    // Search tabs
    searchTabs_ = new QTabWidget(this);

<<<<<<< HEAD
    // --- Tab: Cedula ---
    auto* cedulaWidget = new QWidget();
    auto* cedulaLayout = new QHBoxLayout(cedulaWidget);
    cedulaLayout->setContentsMargins(8, 8, 8, 8);
    edCedula_ = new QLineEdit();
    edCedula_->setPlaceholderText("Ej: 1-1234-5678");
    btnCedula_ = new QPushButton("Buscar por Cedula (HashTable O(1))");
    btnCedula_->setObjectName("primaryButton");
    cedulaLayout->addWidget(new QLabel("Cedula:"));
    cedulaLayout->addWidget(edCedula_, 1);
    cedulaLayout->addWidget(btnCedula_);
    searchTabs_->addTab(cedulaWidget, "Por Cedula");
=======
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
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31

    // --- Tab: Nombre ---
    auto* nameWidget = new QWidget();
    auto* nameLayout = new QHBoxLayout(nameWidget);
<<<<<<< HEAD
    nameLayout->setContentsMargins(8, 8, 8, 8);
    edName_ = new QLineEdit();
    edName_->setPlaceholderText("Ej: Maria Gonzalez");
    btnName_ = new QPushButton("Buscar por Nombre (Busqueda Lineal)");
=======
    nameLayout->setContentsMargins(10, 10, 10, 10);
    edName_ = new QLineEdit();
    edName_->setPlaceholderText("Ej: María González");
    btnName_ = new QPushButton("Buscar por Nombre (Búsqueda Lineal)");
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    btnName_->setObjectName("primaryButton");
    nameLayout->addWidget(new QLabel("Nombre:"));
    nameLayout->addWidget(edName_, 1);
    nameLayout->addWidget(btnName_);
    searchTabs_->addTab(nameWidget, "Por Nombre");

    // --- Tab: Fecha ---
    auto* dateWidget = new QWidget();
    auto* dateLayout = new QHBoxLayout(dateWidget);
<<<<<<< HEAD
    dateLayout->setContentsMargins(8, 8, 8, 8);
    dateFrom_ = new QDateEdit(QDate(2020, 1, 1));
    dateFrom_->setCalendarPopup(true);
    dateFrom_->setDisplayFormat("yyyy-MM-dd");
    dateTo_ = new QDateEdit(QDate::currentDate());
    dateTo_->setCalendarPopup(true);
    dateTo_->setDisplayFormat("yyyy-MM-dd");
    btnDate_ = new QPushButton("Buscar por Rango (Busqueda Binaria)");
=======
    dateLayout->setContentsMargins(10, 10, 10, 10);
    dateFrom_ = new QDateEdit(QDate(2022, 1, 1));
    dateFrom_->setCalendarPopup(true);
    dateFrom_->setDisplayFormat("yyyy-MM-dd");
    dateTo_   = new QDateEdit(QDate::currentDate());
    dateTo_->setCalendarPopup(true);
    dateTo_->setDisplayFormat("yyyy-MM-dd");
    btnDate_ = new QPushButton("Buscar por Rango (Búsqueda Binaria)");
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
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
<<<<<<< HEAD
    gravLayout->setContentsMargins(8, 8, 8, 8);
    cbGravedad_ = new QComboBox();
    cbGravedad_->addItems({"1 - Muy leve", "2 - Leve", "3 - Moderada",
                            "4 - Grave", "5 - Critica"});
=======
    gravLayout->setContentsMargins(10, 10, 10, 10);
    cbGravedad_ = new QComboBox();
    cbGravedad_->addItems({"1 - Muy leve", "2 - Leve", "3 - Moderada",
                            "4 - Grave", "5 - Crítica"});
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
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
<<<<<<< HEAD
    lblTime_   = new QLabel("Tiempo: --");
    lblCount_  = new QLabel("Resultados: 0");
    lblLinear_ = new QLabel("Lineal: --");
    lblBinary_ = new QLabel("Binaria: --");
=======
    lblTime_   = new QLabel("Tiempo: —");
    lblCount_  = new QLabel("Resultados: 0");
    lblLinear_ = new QLabel("Lineal: —");
    lblBinary_ = new QLabel("Binaria: —");
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    infoLayout->addWidget(lblTime_);
    infoLayout->addWidget(new QLabel("|"));
    infoLayout->addWidget(lblCount_);
    infoLayout->addStretch();
<<<<<<< HEAD
    infoLayout->addWidget(new QLabel("Comparacion:"));
=======
    infoLayout->addWidget(new QLabel("Comparación:"));
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    infoLayout->addWidget(lblLinear_);
    infoLayout->addWidget(lblBinary_);
    mainLayout->addLayout(infoLayout);

<<<<<<< HEAD
    // Splitter: results table | patient detail
    auto* splitter = new QSplitter(Qt::Horizontal, this);

    // Results table
    resultsTable_ = new QTableWidget(0, 6, this);
    resultsTable_->setHorizontalHeaderLabels(
        {"Cedula / ID", "Nombre / Medico", "Fecha", "Diagnostico", "Campo4", "Campo5"});
    resultsTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    resultsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    resultsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    resultsTable_->verticalHeader()->setVisible(false);
    splitter->addWidget(resultsTable_);

    // Patient detail panel (expedient)
    patientDetail_ = new QTextEdit();
    patientDetail_->setReadOnly(true);
    patientDetail_->setPlaceholderText("Seleccione un paciente para ver su expediente clinico...");
    splitter->addWidget(patientDetail_);

    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);
    mainLayout->addWidget(splitter, 1);

    // Connect table row selection to show patient record
    connect(resultsTable_, &QTableWidget::itemClicked, this, [this](QTableWidgetItem* item) {
        if (!item) return;
        int row = item->row();
        auto* firstCol = resultsTable_->item(row, 0);
        if (firstCol) showPatientRecord(firstCol->text());
    });
=======
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
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31

    connect(btnCedula_,  &QPushButton::clicked, this, &RecordsSearchWidget::onSearchByCedula);
    connect(btnName_,    &QPushButton::clicked, this, &RecordsSearchWidget::onSearchByName);
    connect(btnDate_,    &QPushButton::clicked, this, &RecordsSearchWidget::onSearchByDate);
    connect(btnGravedad_,&QPushButton::clicked, this, &RecordsSearchWidget::onSearchByGravedad);
    connect(edCedula_,   &QLineEdit::returnPressed, this, &RecordsSearchWidget::onSearchByCedula);
    connect(edName_,     &QLineEdit::returnPressed, this, &RecordsSearchWidget::onSearchByName);
}

void RecordsSearchWidget::loadSampleData(int count) {
<<<<<<< HEAD
    auto& store = ClinicDataStore::instance();
    store.generateSampleData(count);
    module_->loadData(store.pacientes(), store.consultas());
=======
    auto pacientes = DataGenerator::generatePacientes(count);
    auto consultas = DataGenerator::generateConsultas(pacientes, count * 2);
    module_->loadData(pacientes, consultas);
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
}

int RecordsSearchWidget::pacienteCount() const {
    return module_->pacienteCount();
}

void RecordsSearchWidget::showPacienteResults(const QVector<Paciente>& results) {
    resultsTable_->setColumnCount(6);
    resultsTable_->setHorizontalHeaderLabels(
<<<<<<< HEAD
        {"Cedula", "Nombre", "Edad", "Canton", "Diagnostico", "Prioridad"});
=======
        {"Cédula", "Nombre", "Edad", "Cantón", "Diagnóstico", "Prioridad"});
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
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
<<<<<<< HEAD
        {"Cedula Paciente", "Medico", "Fecha", "Diagnostico", "Gravedad", "Costo"});
    resultsTable_->setRowCount(results.size());
    for (int i = 0; i < results.size(); ++i) {
        const Consulta& c = results[i];
        resultsTable_->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(c.cedulaPaciente)));
        resultsTable_->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(c.medicoTratante)));
        resultsTable_->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(c.fecha)));
        resultsTable_->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(c.diagnostico)));
        resultsTable_->setItem(i, 4, new QTableWidgetItem(QString::number(c.gravedad)));
        resultsTable_->setItem(i, 5, new QTableWidgetItem(QString::number(c.costo, 'f', 0)));
=======
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
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    }
    lblCount_->setText(QString("Resultados: %1").arg(results.size()));
    lblTime_->setText(QString("Tiempo: %1 ms").arg(module_->getLastSearchTimeMs(), 0, 'f', 4));
}

<<<<<<< HEAD
void RecordsSearchWidget::showPatientRecord(const QString& cedula) {
    PatientRecord rec = module_->getPatientRecord(cedula);
    if (rec.paciente.cedula.empty()) {
        patientDetail_->setHtml("<p><i>Paciente no encontrado.</i></p>");
        return;
    }
    const Paciente& p = rec.paciente;
    QString html;
    html += "<h3>Expediente Clinico</h3>";
    html += "<table cellpadding='3'>";
    html += QString("<tr><td><b>Cedula:</b></td><td>%1</td></tr>").arg(QString::fromStdString(p.cedula));
    html += QString("<tr><td><b>Nombre:</b></td><td>%1</td></tr>").arg(QString::fromStdString(p.nombre));
    html += QString("<tr><td><b>Edad:</b></td><td>%1</td></tr>").arg(p.edad);
    html += QString("<tr><td><b>Canton:</b></td><td>%1</td></tr>").arg(QString::fromStdString(p.canton));
    html += QString("<tr><td><b>Tipo Sangre:</b></td><td>%1</td></tr>").arg(QString::fromStdString(p.tipoSangre));
    html += QString("<tr><td><b>Telefono:</b></td><td>%1</td></tr>").arg(QString::fromStdString(p.telefono));
    html += QString("<tr><td><b>Prioridad:</b></td><td>%1</td></tr>").arg(p.prioridad);
    html += "</table>";

    html += "<hr/>";
    html += QString("<p><b>Total de Consultas:</b> %1</p>").arg(rec.totalConsultas);
    html += QString("<p><b>Gravedad Maxima:</b> %1/5</p>").arg(rec.maxGravedad);
    if (!rec.ultimaConsultaFecha.isEmpty())
        html += QString("<p><b>Ultima Consulta:</b> %1</p>").arg(rec.ultimaConsultaFecha);

    if (!rec.consultas.isEmpty()) {
        html += "<h4>Historial de Consultas</h4>";
        html += "<table border='1' cellpadding='3' cellspacing='0' width='100%'>";
        html += "<tr><th>Fecha</th><th>Medico</th><th>Diagnostico</th><th>Gravedad</th></tr>";
        for (const auto& c : rec.consultas) {
            html += QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td></tr>")
                .arg(QString::fromStdString(c.fecha))
                .arg(QString::fromStdString(c.medicoTratante))
                .arg(QString::fromStdString(c.diagnostico))
                .arg(c.gravedad);
        }
        html += "</table>";
    }

    patientDetail_->setHtml(html);
}

=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
void RecordsSearchWidget::onSearchByCedula() {
    QString cedula = edCedula_->text().trimmed();
    if (cedula.isEmpty()) return;
    Paciente p = module_->searchByCedula(cedula);
    QVector<Paciente> results;
    if (!p.cedula.empty()) results.push_back(p);
    showPacienteResults(results);
<<<<<<< HEAD
    if (!p.cedula.empty()) showPatientRecord(cedula);
=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
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

<<<<<<< HEAD
    if (from > to) {
        lblTime_->setText("Error: fecha inicial > fecha final");
        return;
    }

=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    // Binary search
    auto results = module_->searchByDateRange(from, to);
    showConsultaResults(results);
    double binaryTime = module_->getLastSearchTimeMs();

    // Linear search for comparison (don't show - just time)
    auto t0 = std::chrono::high_resolution_clock::now();
<<<<<<< HEAD
    std::string sf = from.toStdString(), st = to.toStdString();
    QVector<Consulta> all = module_->getAllConsultas();
    linearSearchAll<Consulta>(all, [&](const Consulta& c){
=======
    QString sFrom = from, sTo = to;
    QVector<Consulta> all = module_->getAllConsultas();
    std::string sf = sFrom.toStdString(), st = sTo.toStdString();
    QVector<Consulta> linear = linearSearchAll<Consulta>(all, [&](const Consulta& c){
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
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
