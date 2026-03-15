#pragma once
#include <QObject>
#include <QString>
#include <QVector>
#include <QDateTime>

struct AuditEntry {
    enum Severity { Info, Warning, Critical };
    Severity severity = Info;
    QString message;
    QString category;
    QDateTime timestamp;

    QString severityString() const {
        switch (severity) {
            case Critical: return "CRITICO";
            case Warning:  return "ADVERTENCIA";
            default:       return "INFO";
        }
    }
};

struct AuditReport {
    QVector<AuditEntry> entries;
    int criticalCount = 0;
    int warningCount = 0;
    int infoCount = 0;
    QDateTime timestamp;

    int totalIssues() const { return criticalCount + warningCount; }
    QString summary() const {
        return QString("Criticos: %1 | Advertencias: %2 | Info: %3")
            .arg(criticalCount).arg(warningCount).arg(infoCount);
    }
};

class SystemAudit : public QObject {
    Q_OBJECT
public:
    explicit SystemAudit(QObject* parent = nullptr);

    AuditReport runFullAudit();
    const AuditReport& lastReport() const { return lastReport_; }

signals:
    void auditCompleted(const AuditReport& report);

private:
    void checkIndexConsistency(AuditReport& report);

    void addEntry(AuditReport& report, AuditEntry::Severity sev,
                  const QString& category, const QString& message);

    AuditReport lastReport_;
};
