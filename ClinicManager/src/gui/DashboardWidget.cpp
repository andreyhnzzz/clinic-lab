#include "DashboardWidget.h"
#include "../core/ClinicDataStore.h"
#include "../core/EventLog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QFont>
#include <QDateTime>

DashboardWidget::DashboardWidget(QWidget* parent)
    : QWidget(parent) {
    setupUI();

    connect(&ClinicDataStore::instance(), &ClinicDataStore::dataChanged,
            this, &DashboardWidget::refresh);
    refresh();
}

void DashboardWidget::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(24, 24, 24, 24);

    // Title
    auto* titleLabel = new QLabel("Dashboard Ejecutivo");
    QFont titleFont;
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #E6EDF3;");
    mainLayout->addWidget(titleLabel);

    auto* subtitleLabel = new QLabel("Resumen general del sistema de gestion clinica");
    subtitleLabel->setStyleSheet("color: #7D8894; font-size: 12px;");
    mainLayout->addWidget(subtitleLabel);
    mainLayout->addSpacing(8);

    // KPI Grid
    auto* kpiGrid = new QGridLayout();
    kpiGrid->setSpacing(12);

    auto makeKpiCard = [](const QString& title, const QString& value,
                          const QString& accent, QLabel*& valueLbl) -> QFrame* {
        auto* card = new QFrame();
        card->setObjectName("kpiCard");
        card->setMinimumHeight(100);
        card->setStyleSheet(QString(
            "QFrame#kpiCard { background: #1C2128; border: 1px solid #2E3742; "
            "border-radius: 8px; border-left: 3px solid %1; }").arg(accent));
        auto* lay = new QVBoxLayout(card);
        lay->setContentsMargins(16, 12, 16, 12);
        auto* titleLbl = new QLabel(title);
        titleLbl->setStyleSheet("color: #A7B1BC; font-size: 11px; font-weight: bold;");
        valueLbl = new QLabel(value);
        valueLbl->setStyleSheet(QString("color: %1; font-size: 24px; font-weight: bold;").arg(accent));
        lay->addWidget(titleLbl);
        lay->addWidget(valueLbl);
        lay->addStretch();
        return card;
    };

    kpiGrid->addWidget(makeKpiCard("TOTAL PACIENTES", "0", "#1DBF73", lblTotalPacientes_), 0, 0);
    kpiGrid->addWidget(makeKpiCard("TOTAL CONSULTAS", "0", "#3AA0FF", lblTotalConsultas_), 0, 1);
    kpiGrid->addWidget(makeKpiCard("PACIENTES EN COLA", "0", "#F59E0B", lblEnCola_), 0, 2);
    kpiGrid->addWidget(makeKpiCard("DIAGNOSTICOS CARGADOS", "0", "#4FD1C5", lblDiagnosticos_), 1, 0);
    kpiGrid->addWidget(makeKpiCard("ESTADO AUDITORIA", "--", "#38BDF8", lblAuditStatus_), 1, 1);
    kpiGrid->addWidget(makeKpiCard("ULTIMA ACTUALIZACION", "--", "#A7B1BC", lblLastUpdate_), 1, 2);

    mainLayout->addLayout(kpiGrid);
    mainLayout->addStretch();
}

void DashboardWidget::refresh() {
    auto& store = ClinicDataStore::instance();
    lblTotalPacientes_->setText(QString::number(store.pacienteCount()));
    lblTotalConsultas_->setText(QString::number(store.consultaCount()));

    int diagCount = store.diagnosisTree().totalNodes();
    lblDiagnosticos_->setText(QString::number(diagCount));
    lblLastUpdate_->setText(QDateTime::currentDateTime().toString("hh:mm:ss"));
}

void DashboardWidget::setQueueSize(int size) {
    lblEnCola_->setText(QString::number(size));
}

void DashboardWidget::setAuditStatus(const QString& status) {
    lblAuditStatus_->setText(status);
}
