#include "PatientTableWidget.h"
#include "HighlightDelegate.h"

#include <QHeaderView>
#include <QSplitter>
#include <QMessageBox>
#include <QScrollBar>

// ── PatientTableModel ─────────────────────────────────────────────────────────

PatientTableModel::PatientTableModel(QObject *parent)
    : QAbstractTableModel(parent) {}

void PatientTableModel::setPatients(const QVector<Patient> &patients)
{
    beginResetModel();
    m_data = patients;
    endResetModel();
}

int PatientTableModel::rowCount(const QModelIndex &) const { return m_data.size(); }
int PatientTableModel::columnCount(const QModelIndex &) const { return ColCount; }

QVariant PatientTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_data.size()) return {};
    const Patient &p = m_data.at(index.row());

    if (role == Qt::DisplayRole || role == Qt::ToolTipRole) {
        switch (index.column()) {
        case ColCedula:   return p.cedula();
        case ColNombre:   return p.nombre();
        case ColEdad:     return p.edad();
        case ColFechaReg: return p.fechaReg().toString("dd/MM/yyyy");
        case ColPrioridad:return Patient::priorityLabel(p.priorityEnum());
        case ColSangre:   return p.tipoSangre();
        case ColDiag:     return p.diagnostico();
        default:          return {};
        }
    }

    if (role == Qt::BackgroundRole) {
        switch (p.priorityEnum()) {
        case Priority::Critical: return QColor(255, 200, 200);  // light red
        case Priority::Urgent:   return QColor(255, 235, 160);  // light amber
        default:                 return QColor(220, 245, 220);  // light green
        }
    }

    return {};
}

QVariant PatientTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return {};
    switch (section) {
    case ColCedula:   return "Cédula";
    case ColNombre:   return "Nombre";
    case ColEdad:     return "Edad";
    case ColFechaReg: return "F. Registro";
    case ColPrioridad:return "Prioridad";
    case ColSangre:   return "Sangre";
    case ColDiag:     return "Diagnóstico";
    default:          return {};
    }
}

// ── PatientTableWidget ────────────────────────────────────────────────────────

PatientTableWidget::PatientTableWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void PatientTableWidget::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(8);

    // ── Main table ────────────────────────────────────────────────────────────
    m_model = new PatientTableModel(this);
    m_tableView = new QTableView(this);
    m_tableView->setModel(m_model);
    m_tableView->setItemDelegate(new HighlightDelegate(this));
    m_tableView->horizontalHeader()->setSectionResizeMode(
        PatientTableModel::ColNombre, QHeaderView::Stretch);
    m_tableView->horizontalHeader()->setSectionResizeMode(
        PatientTableModel::ColDiag, QHeaderView::Stretch);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setSortingEnabled(true);
    m_tableView->verticalHeader()->setDefaultSectionSize(24);
    m_tableView->verticalHeader()->hide();

    mainLayout->addWidget(m_tableView, 1);

    setupSearchPanel();
    setupPriorityPanel();

    mainLayout->addWidget(m_searchGroup);
    mainLayout->addWidget(m_queueGroup);

    showSearchPanel(false);
    showPriorityPanel(true);
}

void PatientTableWidget::setupSearchPanel()
{
    m_searchGroup = new QGroupBox("Búsqueda", this);
    auto *layout = new QHBoxLayout(m_searchGroup);

    m_searchTypeCombo = new QComboBox(this);
    m_searchTypeCombo->addItem("Por Cédula");
    m_searchTypeCombo->addItem("Por Nombre");
    layout->addWidget(new QLabel("Tipo:"));
    layout->addWidget(m_searchTypeCombo);

    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Ingrese término de búsqueda...");
    layout->addWidget(m_searchEdit, 1);

    m_searchBtn = new QPushButton("Buscar", this);
    layout->addWidget(m_searchBtn);

    m_searchResult = new QLabel("", this);
    m_searchResult->setWordWrap(true);
    layout->addWidget(m_searchResult, 1);

    connect(m_searchBtn, &QPushButton::clicked, this, &PatientTableWidget::onSearch);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &PatientTableWidget::onSearch);
}

void PatientTableWidget::setupPriorityPanel()
{
    m_queueGroup = new QGroupBox("Cola de Prioridad", this);
    auto *layout = new QHBoxLayout(m_queueGroup);

    m_enqueueBtn = new QPushButton("Encolar pacientes", this);
    m_dequeueBtn = new QPushButton("Atender siguiente", this);
    m_queueStatus = new QLabel("Cola vacía", this);

    layout->addWidget(m_enqueueBtn);
    layout->addWidget(m_dequeueBtn);
    layout->addWidget(m_queueStatus, 1);

    connect(m_enqueueBtn, &QPushButton::clicked, this, &PatientTableWidget::onEnqueue);
    connect(m_dequeueBtn, &QPushButton::clicked, this, &PatientTableWidget::onDequeue);
}

void PatientTableWidget::showPriorityPanel(bool show) { m_queueGroup->setVisible(show); }
void PatientTableWidget::showSearchPanel(bool show)   { m_searchGroup->setVisible(show); }

void PatientTableWidget::setPatients(const QVector<Patient> &patients)
{
    m_model->setPatients(patients);
    m_priorityQueue.clear();
}

void PatientTableWidget::setMode(Mode mode)
{
    m_mode = mode;
    showPriorityPanel(mode == Mode::PriorityQueue);
    showSearchPanel(mode == Mode::Search);
}

// ── Slots ─────────────────────────────────────────────────────────────────────

void PatientTableWidget::onSearch()
{
    QString term = m_searchEdit->text().trimmed();
    if (term.isEmpty()) {
        m_searchResult->setText("Ingrese un término de búsqueda.");
        return;
    }

    const QVector<Patient> &data = m_model->patients();
    bool byName = (m_searchTypeCombo->currentIndex() == 1);

    QVector<SearchBenchmarkResult> results = byName
        ? QVector<SearchBenchmarkResult>{ m_searchBench.runLinear(data, term) }
        : m_searchBench.runAll(data, term);

    QString msg;
    for (const auto &r : results) {
        msg += QString("[%1]  Encontrado: %2 | Comparaciones: %3 | Tiempo: %4 µs\n")
               .arg(r.algorithm)
               .arg(r.found ? "Sí" : "No")
               .arg(r.comparisons)
               .arg(r.elapsedUs);
    }
    m_searchResult->setText(msg.trimmed());
}

void PatientTableWidget::onEnqueue()
{
    const QVector<Patient> &patients = m_model->patients();
    if (patients.isEmpty()) {
        m_queueStatus->setText("No hay pacientes cargados.");
        return;
    }
    m_priorityQueue.buildFromVector(patients);
    m_queueStatus->setText(
        QString("Cola construida: %1 pacientes. Siguiente: %2 (%3)")
        .arg(m_priorityQueue.size())
        .arg(m_priorityQueue.peek().nombre())
        .arg(Patient::priorityLabel(m_priorityQueue.peek().priorityEnum()))
    );
}

void PatientTableWidget::onDequeue()
{
    if (m_priorityQueue.isEmpty()) {
        m_queueStatus->setText("Cola vacía.");
        return;
    }
    Patient p = m_priorityQueue.dequeue();
    QString next = m_priorityQueue.isEmpty()
                   ? "Cola vacía"
                   : QString("Siguiente: %1 (%2)")
                     .arg(m_priorityQueue.peek().nombre())
                     .arg(Patient::priorityLabel(m_priorityQueue.peek().priorityEnum()));

    m_queueStatus->setText(
        QString("Atendido: %1 | %2 | Restantes: %3 | %4")
        .arg(p.nombre())
        .arg(Patient::priorityLabel(p.priorityEnum()))
        .arg(m_priorityQueue.size())
        .arg(next)
    );
}
