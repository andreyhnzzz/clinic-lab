#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include "../core/SystemAudit.h"

class AuditWidget : public QWidget {
    Q_OBJECT
public:
    explicit AuditWidget(QWidget* parent = nullptr);

signals:
    void auditStatusChanged(const QString& status);

private slots:
    void onRunAudit();
    void displayReport(const AuditReport& report);

private:
    void setupUI();

    SystemAudit* audit_ = nullptr;
    QTableWidget* table_ = nullptr;
    QLabel* lblSummary_ = nullptr;
    QLabel* lblTimestamp_ = nullptr;
    QPushButton* btnRun_ = nullptr;
};
