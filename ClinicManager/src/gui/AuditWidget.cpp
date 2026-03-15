#include "AuditWidget.h"
#include "../core/EventLog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFrame>
#include <QFont>

AuditWidget::AuditWidget(QWidget* parent)
    : QWidget(parent) {
    audit_ = new SystemAudit(this);
    setupUI();
    connect(audit_, &SystemAudit::auditCompleted, this, &AuditWidget::displayReport);
}

void AuditWidget::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(12, 12, 12, 12);

    // Header
    auto* headerLayout = new QHBoxLayout();
    auto* titleLabel = new QLabel("Auditoria de Integridad del Sistema");
    QFont titleFont;
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #E6EDF3;");
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    btnRun_ = new QPushButton("Ejecutar Auditoria");
    btnRun_->setObjectName("primaryButton");
    btnRun_->setMinimumHeight(36);
    headerLayout->addWidget(btnRun_);
    mainLayout->addLayout(headerLayout);

    // Summary
    auto* summaryFrame = new QFrame();
    summaryFrame->setStyleSheet("background: #1C2128; border: 1px solid #2E3742; border-radius: 6px; padding: 8px;");
    auto* summaryLayout = new QHBoxLayout(summaryFrame);
    lblSummary_ = new QLabel("Pendiente de ejecucion...");
    lblSummary_->setStyleSheet("color: #A7B1BC; font-size: 13px;");
    lblTimestamp_ = new QLabel("");
    lblTimestamp_->setStyleSheet("color: #7D8894; font-size: 11px;");
    summaryLayout->addWidget(lblSummary_);
    summaryLayout->addStretch();
    summaryLayout->addWidget(lblTimestamp_);
    mainLayout->addWidget(summaryFrame);

    // Results table
    table_ = new QTableWidget(0, 4);
    table_->setHorizontalHeaderLabels({"Severidad", "Categoria", "Mensaje", "Hora"});
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    table_->horizontalHeader()->resizeSection(0, 100);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->verticalHeader()->setVisible(false);
    mainLayout->addWidget(table_, 1);

    connect(btnRun_, &QPushButton::clicked, this, &AuditWidget::onRunAudit);
}

void AuditWidget::onRunAudit() {
    btnRun_->setEnabled(false);
    audit_->runFullAudit();
    btnRun_->setEnabled(true);
    EventLog::instance().info("Auditoria", "Auditoria de integridad ejecutada.");
}

void AuditWidget::displayReport(const AuditReport& report) {
    lblSummary_->setText(report.summary());
    lblTimestamp_->setText(report.timestamp.toString("yyyy-MM-dd hh:mm:ss"));

    if (report.criticalCount > 0)
        lblSummary_->setStyleSheet("color: #EF4444; font-size: 13px; font-weight: bold;");
    else if (report.warningCount > 0)
        lblSummary_->setStyleSheet("color: #F59E0B; font-size: 13px; font-weight: bold;");
    else
        lblSummary_->setStyleSheet("color: #1DBF73; font-size: 13px; font-weight: bold;");

    table_->setRowCount(0);
    for (const auto& entry : report.entries) {
        int row = table_->rowCount();
        table_->insertRow(row);

        auto* sevItem = new QTableWidgetItem(entry.severityString());
        QColor sevColor;
        switch (entry.severity) {
            case AuditEntry::Critical: sevColor = QColor("#EF4444"); break;
            case AuditEntry::Warning:  sevColor = QColor("#F59E0B"); break;
            default:                   sevColor = QColor("#1DBF73"); break;
        }
        sevItem->setForeground(sevColor);
        table_->setItem(row, 0, sevItem);
        table_->setItem(row, 1, new QTableWidgetItem(entry.category));
        table_->setItem(row, 2, new QTableWidgetItem(entry.message));
        table_->setItem(row, 3, new QTableWidgetItem(entry.timestamp.toString("hh:mm:ss")));
    }
}
