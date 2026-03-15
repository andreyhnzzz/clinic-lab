#include "AttentionQueueWidget.h"
#include "../utils/DataGenerator.h"
#include "../core/ClinicDataStore.h"
#include "../data_structures/DiagnosisTree.h"
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
#include <QDateTime>
#include <QFrame>
#include <QFont>
#include <QSplitter>
#include <QTreeWidget>
#include <QGroupBox>

AttentionQueueWidget::AttentionQueueWidget(QWidget* parent)
    : QWidget(parent) {
    module_ = new Module1_AttentionQueue(this);
    setupUI();
    connect(module_, &Module1_AttentionQueue::queueChanged,
            this, &AttentionQueueWidget::refreshTable);

    // Phase 3E: QTimer to refresh wait time metrics every 15 seconds
    waitTimer_ = new QTimer(this);
    connect(waitTimer_, &QTimer::timeout, this, &AttentionQueueWidget::refreshWaitMetrics);
    waitTimer_->start(15000);

    addSamplePatients(8);
}

void AttentionQueueWidget::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
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
    progressWait_ = new QProgressBar();
    progressWait_->setRange(0, 60);
    progressWait_->setValue(0);
    progressWait_->setFormat("%v min");
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
        case 1: bg = QColor("#3D1A1A"); badge = "Critico";   break;
        case 2: bg = QColor("#3D2E0A"); badge = "Urgente";  break;
        default: bg = QColor("#0D3D1A"); badge = "Normal";   break;
    }
    for (int col = 0; col < table_->columnCount(); ++col) {
        if (auto* item = table_->item(row, col))
            item->setBackground(bg);
    }
    if (auto* item = table_->item(row, 3))
        item->setText(badge);
}

void AttentionQueueWidget::refreshTable() {
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
    int c1 = module_->countByPriority(1);
    int c2 = module_->countByPriority(2);
    int c3 = module_->countByPriority(3);
    int total = module_->size();

    lblCritico_->setText(QString("Critico: %1").arg(c1));
    lblUrgente_->setText(QString("Urgente: %1").arg(c2));
    lblNormal_ ->setText(QString("Normal: %1").arg(c3));
    lblTotal_  ->setText(QString("Total: %1").arg(total));

    double avgWait = module_->getAverageWaitTimeMinutes();
    lblAvgWait_->setText(QString("%1 min").arg(avgWait, 0, 'f', 1));
    progressWait_->setValue(static_cast<int>(qMin(avgWait, 60.0)));

    // Update recently attended history
    const auto& attended = module_->recentlyAttended();
    historyTable_->setRowCount(attended.size());
    for (int i = 0; i < attended.size(); ++i) {
        const Paciente& p = attended[i];
        historyTable_->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(p.cedula)));
        historyTable_->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(p.nombre)));
        historyTable_->setItem(i, 2, new QTableWidgetItem(QString::number(p.prioridad)));
    }

    emit queueSizeChanged(total);

    // Emit next patient info for dashboard
    QString nextInfo;
    if (!ordered.isEmpty()) {
        const Paciente& np = ordered[0].paciente;
        QString ps;
        switch (np.prioridad) {
            case 1: ps = "CRITICO"; break;
            case 2: ps = "URGENTE"; break;
            default: ps = "NORMAL"; break;
        }
        nextInfo = QString("%1 | %2").arg(QString::fromStdString(np.nombre), ps);
    }
    emit nextPatientChanged(nextInfo);
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
    cbPrioridad->addItems({"1 - Critico", "2 - Urgente", "3 - Normal"});
    cbPrioridad->setCurrentIndex(2);
    auto* edCanton    = new QLineEdit(&dlg);
    auto* edDiag      = new QLineEdit(&dlg);

    // Suggest random data
    Paciente sample = DataGenerator::generatePacientes(1)[0];
    edCedula->setPlaceholderText(QString::fromStdString(sample.cedula));
    edNombre->setPlaceholderText(QString::fromStdString(sample.nombre));
    edCanton->setPlaceholderText(QString::fromStdString(sample.canton));
    edDiag->setPlaceholderText(QString::fromStdString(sample.diagnostico));

    form->addRow("Cedula:",    edCedula);
    form->addRow("Nombre:",    edNombre);
    form->addRow("Edad:",      spEdad);
    form->addRow("Prioridad:", cbPrioridad);
    form->addRow("Canton:",    edCanton);
    form->addRow("Diagnostico:", edDiag);

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

    // Check for duplicate cedula
    if (module_->containsCedula(QString::fromStdString(p.cedula))) {
        QMessageBox::warning(this, "Duplicado",
            "Ya existe un paciente con esa cedula en la cola.");
        return;
    }

    module_->addPatient(p);
    // Also add to central store
    ClinicDataStore::instance().addPaciente(p);
}

void AttentionQueueWidget::refreshWaitMetrics() {
    if (module_->isEmpty()) return;
    double avgWait = module_->getAverageWaitTimeMinutes();
    lblAvgWait_->setText(QString("%1 min").arg(avgWait, 0, 'f', 1));
    progressWait_->setValue(static_cast<int>(qMin(avgWait, 60.0)));
}

void AttentionQueueWidget::onAttendNext() {
    if (module_->isEmpty()) {
        QMessageBox::information(this, "Cola vacia", "No hay pacientes en la cola.");
        return;
    }
    Paciente p = module_->attendNextPatient();

    // Clinical flow: open a consultation dialog with structured diagnosis selector
    QDialog dlg(this);
    dlg.setWindowTitle("Registrar Consulta - Flujo Clinico");
    dlg.setMinimumWidth(540);
    dlg.setMinimumHeight(480);
    auto* form = new QFormLayout(&dlg);

    auto* lblInfo = new QLabel(QString("Paciente: <b>%1</b> | Cedula: %2")
        .arg(QString::fromStdString(p.nombre))
        .arg(QString::fromStdString(p.cedula)));
    form->addRow(lblInfo);

    auto* edMotivo = new QLineEdit(&dlg);
    edMotivo->setPlaceholderText("Motivo de consulta...");
    auto* cbGravedad = new QComboBox(&dlg);
    cbGravedad->addItems({"1 - Leve", "2 - Moderado", "3 - Medio", "4 - Grave", "5 - Critico"});
    cbGravedad->setCurrentIndex(1);
    auto* spCosto = new QSpinBox(&dlg);
    spCosto->setRange(5000, 5000000);
    spCosto->setSingleStep(5000);
    spCosto->setValue(25000);
    spCosto->setSuffix(" CRC");

    // --- Structured diagnosis selector from the tree ---
    auto* diagGroup = new QGroupBox("Seleccion de Diagnostico (desde arbol ICD-10)", &dlg);
    auto* diagLayout = new QVBoxLayout(diagGroup);

    auto* diagSearch = new QLineEdit(&dlg);
    diagSearch->setPlaceholderText("Buscar diagnostico por nombre o codigo...");
    diagLayout->addWidget(diagSearch);

    auto* diagTree = new QTreeWidget(&dlg);
    diagTree->setHeaderLabels({"Diagnostico", "Codigo"});
    diagTree->setMinimumHeight(150);
    diagTree->setMaximumHeight(200);

    // Populate tree from ClinicDataStore diagnosis tree
    auto& tree = ClinicDataStore::instance().diagnosisTree();
    DiagnosisNode* treeRoot = tree.root();
    if (treeRoot) {
        for (auto* areaNode : treeRoot->children) {
            auto* areaItem = new QTreeWidgetItem(diagTree, {areaNode->name, ""});
            areaItem->setFlags(areaItem->flags() & ~Qt::ItemIsSelectable);
            for (auto* specNode : areaNode->children) {
                auto* specItem = new QTreeWidgetItem(areaItem, {specNode->name, ""});
                specItem->setFlags(specItem->flags() & ~Qt::ItemIsSelectable);
                for (auto* diagNode : specNode->children) {
                    new QTreeWidgetItem(specItem,
                        {diagNode->name, diagNode->code});
                }
            }
        }
    }
    diagLayout->addWidget(diagTree);

    auto* lblSelected = new QLabel("Seleccionado: (ninguno)");
    lblSelected->setStyleSheet("color: #A7B1BC; font-size: 11px;");
    diagLayout->addWidget(lblSelected);

    // Track selected diagnosis info
    QString selectedDiagName, selectedDiagCode, selectedArea, selectedSpecialty;

    QObject::connect(diagTree, &QTreeWidget::itemClicked, &dlg,
        [&](QTreeWidgetItem* item, int) {
            if (!item || item->text(1).isEmpty()) return; // Not a leaf diagnosis
            selectedDiagName = item->text(0);
            selectedDiagCode = item->text(1);
            // Get area and specialty from parents
            if (item->parent()) {
                selectedSpecialty = item->parent()->text(0);
                if (item->parent()->parent())
                    selectedArea = item->parent()->parent()->text(0);
            }
            lblSelected->setText(QString("Seleccionado: [%1] %2 (%3 > %4)")
                .arg(selectedDiagCode).arg(selectedDiagName)
                .arg(selectedArea).arg(selectedSpecialty));
        });

    // Search/filter functionality
    QObject::connect(diagSearch, &QLineEdit::textChanged, &dlg,
        [diagTree](const QString& text) {
            QString lower = text.toLower();
            for (int i = 0; i < diagTree->topLevelItemCount(); ++i) {
                auto* area = diagTree->topLevelItem(i);
                bool areaVisible = false;
                for (int j = 0; j < area->childCount(); ++j) {
                    auto* spec = area->child(j);
                    bool specVisible = false;
                    for (int k = 0; k < spec->childCount(); ++k) {
                        auto* diag = spec->child(k);
                        bool match = text.isEmpty()
                            || diag->text(0).toLower().contains(lower)
                            || diag->text(1).toLower().contains(lower);
                        diag->setHidden(!match);
                        if (match) {
                            specVisible = true;
                            areaVisible = true;
                        }
                    }
                    spec->setHidden(!specVisible);
                    if (specVisible) spec->setExpanded(true);
                }
                area->setHidden(!areaVisible);
                if (areaVisible) area->setExpanded(true);
            }
        });

    auto* edNotas = new QLineEdit(&dlg);
    edNotas->setPlaceholderText("Notas adicionales...");

    form->addRow("Motivo:", edMotivo);
    form->addRow("Gravedad:", cbGravedad);
    form->addRow("Costo:", spCosto);
    form->addRow(diagGroup);
    form->addRow("Notas:", edNotas);

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        // Create consultation and persist it
        Consulta c;
        c.idConsulta = ("CONS-" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz")).toStdString();
        c.cedulaPaciente = p.cedula;
        c.fecha = QDate::currentDate().toString("yyyy-MM-dd").toStdString();
        static const char* DEFAULT_MEDICO = "Medico de turno";
        c.medicoTratante = DEFAULT_MEDICO;
        // Use structured diagnosis if selected, otherwise fallback to patient's existing
        if (!selectedDiagName.isEmpty()) {
            c.diagnostico = selectedDiagName.toStdString();
            c.codigoDiagnostico = selectedDiagCode.toStdString();
            c.areaDiagnostico = selectedArea.toStdString();
            c.especialidadDiagnostico = selectedSpecialty.toStdString();
        } else {
            c.diagnostico = p.diagnostico;
        }
        c.gravedad = cbGravedad->currentIndex() + 1;
        c.costo = spCosto->value();
        c.notas = edNotas->text().toStdString();

        ClinicDataStore::instance().addConsulta(c);

        QString diagInfo = selectedDiagCode.isEmpty()
            ? QString::fromStdString(c.diagnostico)
            : QString("[%1] %2").arg(selectedDiagCode).arg(selectedDiagName);

        QMessageBox::information(this, "Consulta Registrada",
            QString("Paciente atendido y consulta registrada:\n\n"
                    "Cedula: %1\nNombre: %2\nDiagnostico: %3\nGravedad: %4\nCosto: %5 CRC")
                .arg(QString::fromStdString(p.cedula))
                .arg(QString::fromStdString(p.nombre))
                .arg(diagInfo)
                .arg(c.gravedad)
                .arg(c.costo, 0, 'f', 0));
    } else {
        QMessageBox::information(this, "Paciente Atendido",
            QString("Paciente atendido (sin consulta registrada):\n\n"
                    "Cedula: %1\nNombre: %2\nPrioridad: %3")
                .arg(QString::fromStdString(p.cedula))
                .arg(QString::fromStdString(p.nombre))
                .arg(p.prioridad));
    }
}

int AttentionQueueWidget::queueSize() const {
    return module_->size();
}

void AttentionQueueWidget::addSamplePatients(int count) {
    auto patients = DataGenerator::generatePacientes(count);
    for (const auto& p : patients) {
        module_->addPatient(p);
        ClinicDataStore::instance().addPaciente(p);
    }
}
