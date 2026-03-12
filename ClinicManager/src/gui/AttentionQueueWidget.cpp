#include "AttentionQueueWidget.h"
#include "../utils/DataGenerator.h"
<<<<<<< HEAD
#include "../core/ClinicDataStore.h"
=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QFrame>
#include <QFont>
<<<<<<< HEAD
#include <QSplitter>
=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31

AttentionQueueWidget::AttentionQueueWidget(QWidget* parent)
    : QWidget(parent) {
    module_ = new Module1_AttentionQueue(this);
    setupUI();
    connect(module_, &Module1_AttentionQueue::queueChanged,
            this, &AttentionQueueWidget::refreshTable);
    addSamplePatients(8);
}

void AttentionQueueWidget::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
<<<<<<< HEAD
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(12, 12, 12, 12);

    // KPI cards row
    auto* kpiLayout = new QHBoxLayout();
    kpiLayout->setSpacing(8);
    auto makeKpiCard = [](const QString& text, QLabel*& lbl) -> QFrame* {
        auto* card = new QFrame();
        card->setObjectName("kpiCard");
        auto* lay = new QVBoxLayout(card);
        lay->setContentsMargins(12, 8, 12, 8);
        lbl = new QLabel(text);
        lbl->setAlignment(Qt::AlignCenter);
        lay->addWidget(lbl);
        return card;
    };
    kpiLayout->addWidget(makeKpiCard("Critico: 0", lblCritico_));
    kpiLayout->addWidget(makeKpiCard("Urgente: 0", lblUrgente_));
    kpiLayout->addWidget(makeKpiCard("Normal: 0", lblNormal_));
    kpiLayout->addWidget(makeKpiCard("Total: 0", lblTotal_));
    mainLayout->addLayout(kpiLayout);

    // Next patient label
    lblNext_ = new QLabel("Siguiente: --");
    lblNext_->setObjectName("nextPatientLabel");
    mainLayout->addWidget(lblNext_);

    // Splitter: queue table | recently attended
    auto* splitter = new QSplitter(Qt::Horizontal, this);

    // Queue table (shows REAL attention order)
    auto* queueFrame = new QFrame();
    auto* queueLayout = new QVBoxLayout(queueFrame);
    queueLayout->setContentsMargins(0, 0, 0, 0);
    auto* queueTitle = new QLabel("Pacientes en Espera (Orden Real de Atencion)");
    queueTitle->setObjectName("tableTitle");
    queueLayout->addWidget(queueTitle);
    table_ = new QTableWidget(0, 6, this);
    table_->setHorizontalHeaderLabels(
        {"#", "Cedula", "Nombre", "Prioridad", "Edad", "Canton"});
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    table_->horizontalHeader()->resizeSection(0, 40);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->verticalHeader()->setVisible(false);
    queueLayout->addWidget(table_, 1);
    splitter->addWidget(queueFrame);

    // Recently attended history
    auto* histFrame = new QFrame();
    auto* histLayout = new QVBoxLayout(histFrame);
    histLayout->setContentsMargins(0, 0, 0, 0);
    auto* histTitle = new QLabel("Historial de Atencion Reciente");
    histTitle->setObjectName("tableTitle");
    histLayout->addWidget(histTitle);
    historyTable_ = new QTableWidget(0, 3, this);
    historyTable_->setHorizontalHeaderLabels({"Cedula", "Nombre", "Prioridad"});
    historyTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    historyTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    historyTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    historyTable_->verticalHeader()->setVisible(false);
    histLayout->addWidget(historyTable_, 1);
    splitter->addWidget(histFrame);

    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    mainLayout->addWidget(splitter, 1);

    // Wait time section
    auto* waitLayout = new QHBoxLayout();
    waitLayout->addWidget(new QLabel("Espera promedio:"));
=======
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // Title
    auto* title = new QLabel("🏥 Cola de Atención por Prioridad");
    QFont f = title->font();
    f.setPointSize(14);
    f.setBold(true);
    title->setFont(f);
    title->setObjectName("sectionTitle");
    mainLayout->addWidget(title);

    // Stats row
    auto* statsBox = new QGroupBox("Estadísticas de la Cola");
    auto* statsLayout = new QHBoxLayout(statsBox);

    lblCritico_ = new QLabel("🔴 Crítico: 0");
    lblUrgente_ = new QLabel("🟠 Urgente: 0");
    lblNormal_  = new QLabel("🟢 Normal: 0");
    lblTotal_   = new QLabel("Total: 0");

    for (auto* lbl : {lblCritico_, lblUrgente_, lblNormal_, lblTotal_}) {
        lbl->setObjectName("statLabel");
        statsLayout->addWidget(lbl);
        statsLayout->addStretch();
    }
    mainLayout->addWidget(statsBox);

    // Table
    table_ = new QTableWidget(0, 6, this);
    table_->setHorizontalHeaderLabels(
        {"Cédula", "Nombre", "Prioridad", "Edad", "Cantón", "Diagnóstico"});
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setAlternatingRowColors(false);
    table_->verticalHeader()->setVisible(false);
    mainLayout->addWidget(table_, 1);

    // Wait time section
    auto* waitBox = new QGroupBox("Tiempo Promedio de Espera");
    auto* waitLayout = new QHBoxLayout(waitBox);
    lblAvgWait_ = new QLabel("0.0 min");
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    progressWait_ = new QProgressBar();
    progressWait_->setRange(0, 60);
    progressWait_->setValue(0);
    progressWait_->setFormat("%v min");
<<<<<<< HEAD
    waitLayout->addWidget(progressWait_, 1);
    lblAvgWait_ = new QLabel("0.0 min");
    waitLayout->addWidget(lblAvgWait_);
    mainLayout->addLayout(waitLayout);

    // Buttons
    auto* btnLayout = new QHBoxLayout();
    btnRegister_ = new QPushButton("Registrar Paciente");
    btnAttend_   = new QPushButton("Atender Siguiente");
    btnRegister_->setObjectName("primaryButton");
    btnAttend_->setObjectName("successButton");
    btnRegister_->setMinimumHeight(38);
    btnAttend_->setMinimumHeight(38);
=======
    waitLayout->addWidget(new QLabel("Espera promedio:"));
    waitLayout->addWidget(progressWait_, 1);
    waitLayout->addWidget(lblAvgWait_);
    mainLayout->addWidget(waitBox);

    // Buttons
    auto* btnLayout = new QHBoxLayout();
    btnRegister_ = new QPushButton("➕  Registrar Paciente");
    btnAttend_   = new QPushButton("✅  Atender Siguiente");
    btnRegister_->setObjectName("primaryButton");
    btnAttend_->setObjectName("successButton");
    btnRegister_->setMinimumHeight(40);
    btnAttend_->setMinimumHeight(40);
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    btnLayout->addWidget(btnRegister_);
    btnLayout->addWidget(btnAttend_);
    mainLayout->addLayout(btnLayout);

    connect(btnRegister_, &QPushButton::clicked, this, &AttentionQueueWidget::onRegisterPatient);
    connect(btnAttend_,   &QPushButton::clicked, this, &AttentionQueueWidget::onAttendNext);
}

void AttentionQueueWidget::applyRowColor(int row, int priority) {
    QColor bg;
    QString badge;
    switch (priority) {
<<<<<<< HEAD
        case 1: bg = QColor("#3D1A1A"); badge = "Critico";   break;
        case 2: bg = QColor("#3D2E0A"); badge = "Urgente";  break;
        default: bg = QColor("#0D3D1A"); badge = "Normal";   break;
=======
        case 1: bg = QColor("#7B1C1C"); badge = "🔴 Crítico";   break;
        case 2: bg = QColor("#7B4A00"); badge = "🟠 Urgente";  break;
        default: bg = QColor("#1B4D1B"); badge = "🟢 Normal";   break;
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    }
    for (int col = 0; col < table_->columnCount(); ++col) {
        if (auto* item = table_->item(row, col))
            item->setBackground(bg);
    }
<<<<<<< HEAD
    if (auto* item = table_->item(row, 3))
=======
    // Update priority cell
    if (auto* item = table_->item(row, 2))
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
        item->setText(badge);
}

void AttentionQueueWidget::refreshTable() {
<<<<<<< HEAD
    // Get patients in REAL attention order (sorted by priority + arrival)
    QVector<QueuedPatient> ordered = module_->getQueueInAttentionOrder();
    table_->setRowCount(0);
    table_->setRowCount(ordered.size());

    for (int i = 0; i < ordered.size(); ++i) {
        const Paciente& p = ordered[i].paciente;
        table_->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        table_->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(p.cedula)));
        table_->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(p.nombre)));
        table_->setItem(i, 3, new QTableWidgetItem(QString::number(p.prioridad)));
        table_->setItem(i, 4, new QTableWidgetItem(QString::number(p.edad)));
        table_->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(p.canton)));
        applyRowColor(i, p.prioridad);
    }

    // Update next patient label
    if (!ordered.isEmpty()) {
        const Paciente& next = ordered[0].paciente;
        QString prioStr;
        switch (next.prioridad) {
            case 1: prioStr = "CRITICO"; break;
            case 2: prioStr = "URGENTE"; break;
            default: prioStr = "NORMAL"; break;
        }
        lblNext_->setText(QString("Siguiente: %1 | %2 | %3")
            .arg(QString::fromStdString(next.nombre))
            .arg(QString::fromStdString(next.cedula))
            .arg(prioStr));
    } else {
        lblNext_->setText("Siguiente: -- (cola vacia)");
    }

    // Update KPI counts
=======
    const QVector<Paciente>& patients = module_->getPatientsInQueue();
    table_->setRowCount(0);
    table_->setRowCount(patients.size());

    for (int i = 0; i < patients.size(); ++i) {
        const Paciente& p = patients[i];
        table_->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(p.cedula)));
        table_->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(p.nombre)));
        table_->setItem(i, 2, new QTableWidgetItem(QString::number(p.prioridad)));
        table_->setItem(i, 3, new QTableWidgetItem(QString::number(p.edad)));
        table_->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(p.canton)));
        table_->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(p.diagnostico)));
        applyRowColor(i, p.prioridad);
    }

>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    int c1 = module_->countByPriority(1);
    int c2 = module_->countByPriority(2);
    int c3 = module_->countByPriority(3);
    int total = module_->size();

<<<<<<< HEAD
    lblCritico_->setText(QString("Critico: %1").arg(c1));
    lblUrgente_->setText(QString("Urgente: %1").arg(c2));
    lblNormal_ ->setText(QString("Normal: %1").arg(c3));
=======
    lblCritico_->setText(QString("🔴 Crítico: %1").arg(c1));
    lblUrgente_->setText(QString("🟠 Urgente: %1").arg(c2));
    lblNormal_ ->setText(QString("🟢 Normal: %1").arg(c3));
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    lblTotal_  ->setText(QString("Total: %1").arg(total));

    double avgWait = module_->getAverageWaitTimeMinutes();
    lblAvgWait_->setText(QString("%1 min").arg(avgWait, 0, 'f', 1));
    progressWait_->setValue(static_cast<int>(qMin(avgWait, 60.0)));

<<<<<<< HEAD
    // Update recently attended history
    const auto& attended = module_->recentlyAttended();
    historyTable_->setRowCount(attended.size());
    for (int i = 0; i < attended.size(); ++i) {
        const Paciente& p = attended[i];
        historyTable_->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(p.cedula)));
        historyTable_->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(p.nombre)));
        historyTable_->setItem(i, 2, new QTableWidgetItem(QString::number(p.prioridad)));
    }

=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    emit queueSizeChanged(total);
}

void AttentionQueueWidget::onRegisterPatient() {
    QDialog dlg(this);
    dlg.setWindowTitle("Registrar Paciente");
    dlg.setMinimumWidth(400);
    auto* form = new QFormLayout(&dlg);

    auto* edCedula    = new QLineEdit(&dlg);
    auto* edNombre    = new QLineEdit(&dlg);
    auto* spEdad      = new QSpinBox(&dlg);
    spEdad->setRange(1, 120);
    spEdad->setValue(30);
    auto* cbPrioridad = new QComboBox(&dlg);
<<<<<<< HEAD
    cbPrioridad->addItems({"1 - Critico", "2 - Urgente", "3 - Normal"});
=======
    cbPrioridad->addItems({"1 - Crítico", "2 - Urgente", "3 - Normal"});
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    cbPrioridad->setCurrentIndex(2);
    auto* edCanton    = new QLineEdit(&dlg);
    auto* edDiag      = new QLineEdit(&dlg);

    // Suggest random data
    Paciente sample = DataGenerator::generatePacientes(1)[0];
    edCedula->setPlaceholderText(QString::fromStdString(sample.cedula));
    edNombre->setPlaceholderText(QString::fromStdString(sample.nombre));
    edCanton->setPlaceholderText(QString::fromStdString(sample.canton));
    edDiag->setPlaceholderText(QString::fromStdString(sample.diagnostico));

<<<<<<< HEAD
    form->addRow("Cedula:",    edCedula);
    form->addRow("Nombre:",    edNombre);
    form->addRow("Edad:",      spEdad);
    form->addRow("Prioridad:", cbPrioridad);
    form->addRow("Canton:",    edCanton);
    form->addRow("Diagnostico:", edDiag);
=======
    form->addRow("Cédula:",    edCedula);
    form->addRow("Nombre:",    edNombre);
    form->addRow("Edad:",      spEdad);
    form->addRow("Prioridad:", cbPrioridad);
    form->addRow("Cantón:",    edCanton);
    form->addRow("Diagnóstico:", edDiag);
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    Paciente p;
    p.cedula    = edCedula->text().isEmpty() ? sample.cedula : edCedula->text().toStdString();
    p.nombre    = edNombre->text().isEmpty() ? sample.nombre : edNombre->text().toStdString();
    p.edad      = spEdad->value();
    p.prioridad = cbPrioridad->currentIndex() + 1;
    p.canton    = edCanton->text().isEmpty() ? sample.canton : edCanton->text().toStdString();
    p.diagnostico = edDiag->text().isEmpty() ? sample.diagnostico : edDiag->text().toStdString();
    p.fechaRegistro = QDate::currentDate().toString("yyyy-MM-dd").toStdString();
    p.tipoSangre = sample.tipoSangre;
    p.telefono   = sample.telefono;

<<<<<<< HEAD
    // Check for duplicate cedula
    if (module_->containsCedula(QString::fromStdString(p.cedula))) {
        QMessageBox::warning(this, "Duplicado",
            "Ya existe un paciente con esa cedula en la cola.");
        return;
    }

    module_->addPatient(p);
    // Also add to central store
    ClinicDataStore::instance().addPaciente(p);
=======
    module_->addPatient(p);
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
}

void AttentionQueueWidget::onAttendNext() {
    if (module_->isEmpty()) {
<<<<<<< HEAD
        QMessageBox::information(this, "Cola vacia", "No hay pacientes en la cola.");
=======
        QMessageBox::information(this, "Cola vacía", "No hay pacientes en la cola.");
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
        return;
    }
    Paciente p = module_->attendNextPatient();
    QMessageBox::information(this, "Paciente Atendido",
<<<<<<< HEAD
        QString("Paciente atendido:\n\nCedula: %1\nNombre: %2\nPrioridad: %3")
=======
        QString("Paciente atendido:\n\nCédula: %1\nNombre: %2\nPrioridad: %3")
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
            .arg(QString::fromStdString(p.cedula))
            .arg(QString::fromStdString(p.nombre))
            .arg(p.prioridad));
}

int AttentionQueueWidget::queueSize() const {
    return module_->size();
}

void AttentionQueueWidget::addSamplePatients(int count) {
    auto patients = DataGenerator::generatePacientes(count);
<<<<<<< HEAD
    for (const auto& p : patients) {
        module_->addPatient(p);
        ClinicDataStore::instance().addPaciente(p);
    }
=======
    for (const auto& p : patients)
        module_->addPatient(p);
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
}
