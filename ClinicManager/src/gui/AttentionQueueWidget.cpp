#include "AttentionQueueWidget.h"
#include "../utils/DataGenerator.h"
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
    progressWait_ = new QProgressBar();
    progressWait_->setRange(0, 60);
    progressWait_->setValue(0);
    progressWait_->setFormat("%v min");
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
        case 1: bg = QColor("#7B1C1C"); badge = "🔴 Crítico";   break;
        case 2: bg = QColor("#7B4A00"); badge = "🟠 Urgente";  break;
        default: bg = QColor("#1B4D1B"); badge = "🟢 Normal";   break;
    }
    for (int col = 0; col < table_->columnCount(); ++col) {
        if (auto* item = table_->item(row, col))
            item->setBackground(bg);
    }
    // Update priority cell
    if (auto* item = table_->item(row, 2))
        item->setText(badge);
}

void AttentionQueueWidget::refreshTable() {
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

    int c1 = module_->countByPriority(1);
    int c2 = module_->countByPriority(2);
    int c3 = module_->countByPriority(3);
    int total = module_->size();

    lblCritico_->setText(QString("🔴 Crítico: %1").arg(c1));
    lblUrgente_->setText(QString("🟠 Urgente: %1").arg(c2));
    lblNormal_ ->setText(QString("🟢 Normal: %1").arg(c3));
    lblTotal_  ->setText(QString("Total: %1").arg(total));

    double avgWait = module_->getAverageWaitTimeMinutes();
    lblAvgWait_->setText(QString("%1 min").arg(avgWait, 0, 'f', 1));
    progressWait_->setValue(static_cast<int>(qMin(avgWait, 60.0)));

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
    cbPrioridad->addItems({"1 - Crítico", "2 - Urgente", "3 - Normal"});
    cbPrioridad->setCurrentIndex(2);
    auto* edCanton    = new QLineEdit(&dlg);
    auto* edDiag      = new QLineEdit(&dlg);

    // Suggest random data
    Paciente sample = DataGenerator::generatePacientes(1)[0];
    edCedula->setPlaceholderText(QString::fromStdString(sample.cedula));
    edNombre->setPlaceholderText(QString::fromStdString(sample.nombre));
    edCanton->setPlaceholderText(QString::fromStdString(sample.canton));
    edDiag->setPlaceholderText(QString::fromStdString(sample.diagnostico));

    form->addRow("Cédula:",    edCedula);
    form->addRow("Nombre:",    edNombre);
    form->addRow("Edad:",      spEdad);
    form->addRow("Prioridad:", cbPrioridad);
    form->addRow("Cantón:",    edCanton);
    form->addRow("Diagnóstico:", edDiag);

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

    module_->addPatient(p);
}

void AttentionQueueWidget::onAttendNext() {
    if (module_->isEmpty()) {
        QMessageBox::information(this, "Cola vacía", "No hay pacientes en la cola.");
        return;
    }
    Paciente p = module_->attendNextPatient();
    QMessageBox::information(this, "Paciente Atendido",
        QString("Paciente atendido:\n\nCédula: %1\nNombre: %2\nPrioridad: %3")
            .arg(QString::fromStdString(p.cedula))
            .arg(QString::fromStdString(p.nombre))
            .arg(p.prioridad));
}

int AttentionQueueWidget::queueSize() const {
    return module_->size();
}

void AttentionQueueWidget::addSamplePatients(int count) {
    auto patients = DataGenerator::generatePacientes(count);
    for (const auto& p : patients)
        module_->addPatient(p);
}
