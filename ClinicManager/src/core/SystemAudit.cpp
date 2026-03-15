#include "SystemAudit.h"
#include "ClinicDataStore.h"
#include "DataConsistencyValidator.h"
#include <QMap>

SystemAudit::SystemAudit(QObject* parent) : QObject(parent) {}

void SystemAudit::addEntry(AuditReport& report, AuditEntry::Severity sev,
                           const QString& category, const QString& message) {
    AuditEntry e;
    e.severity = sev;
    e.category = category;
    e.message = message;
    e.timestamp = QDateTime::currentDateTime();
    report.entries.push_back(e);

    switch (sev) {
        case AuditEntry::Critical: report.criticalCount++; break;
        case AuditEntry::Warning:  report.warningCount++;  break;
        case AuditEntry::Info:     report.infoCount++;      break;
    }
}

AuditReport SystemAudit::runFullAudit() {
    AuditReport report;
    report.timestamp = QDateTime::currentDateTime();

    const auto& store = ClinicDataStore::instance();
    QVector<DataValidationIssue> issues =
        DataConsistencyValidator::validateDataset(store.pacientes(), store.consultas());

    const QMap<QString, int> categoryLimits = {
        {"Duplicados", 5},
        {"Consultas huerfanas", 3},
        {"Prioridades", 3},
        {"Fechas", 3},
        {"Costos", 3},
        {"Gravedad", 3}
    };
    QMap<QString, int> printedCount;
    QMap<QString, int> totalCount;

    for (const auto& issue : issues) {
        totalCount[issue.category]++;
        int limit = categoryLimits.value(issue.category, 3);
        int shown = printedCount.value(issue.category, 0);
        if (shown >= limit) continue;

        AuditEntry::Severity sev = (issue.severity == ValidationSeverity::Critical)
            ? AuditEntry::Critical
            : AuditEntry::Warning;
        addEntry(report, sev, issue.category, issue.message);
        printedCount[issue.category] = shown + 1;
    }

    for (auto it = totalCount.begin(); it != totalCount.end(); ++it) {
        int limit = categoryLimits.value(it.key(), 3);
        if (it.value() > limit) {
            addEntry(report, AuditEntry::Warning, it.key(),
                     QString("...y %1 registros mas").arg(it.value() - limit));
        }
    }

    if (!totalCount.contains("Duplicados")) {
        addEntry(report, AuditEntry::Info, "Duplicados",
                 "No se encontraron pacientes duplicados.");
    }
    if (!totalCount.contains("Consultas huerfanas")) {
        addEntry(report, AuditEntry::Info, "Consultas huerfanas",
                 "Todas las consultas tienen paciente valido.");
    }
    if (!totalCount.contains("Prioridades")) {
        addEntry(report, AuditEntry::Info, "Prioridades",
                 "Todas las prioridades estan en rango valido (1-3).");
    }
    if (!totalCount.contains("Fechas")) {
        addEntry(report, AuditEntry::Info, "Fechas",
                 "Todas las fechas tienen formato valido.");
    }
    if (!totalCount.contains("Costos")) {
        addEntry(report, AuditEntry::Info, "Costos",
                 "Todos los costos de consultas son validos.");
    }
    if (!totalCount.contains("Gravedad")) {
        addEntry(report, AuditEntry::Info, "Gravedad",
                 "Todas las gravedades estan en rango valido (1-5).");
    }

    checkIndexConsistency(report);

    if (report.totalIssues() == 0) {
        addEntry(report, AuditEntry::Info, "Sistema",
                 "Auditoria completada sin problemas detectados.");
    }

    lastReport_ = report;
    emit auditCompleted(report);
    return report;
}

void SystemAudit::checkIndexConsistency(AuditReport& report) {
    auto& store = ClinicDataStore::instance();
    const auto& pacs = store.pacientes();
    int mismatches = 0;
    for (const auto& p : pacs) {
        Paciente found = store.findByCedula(QString::fromStdString(p.cedula));
        if (found.cedula != p.cedula) {
            mismatches++;
            if (mismatches <= 3)
                addEntry(report, AuditEntry::Critical, "Indices",
                         QString("Indice inconsistente para cedula %1")
                             .arg(QString::fromStdString(p.cedula)));
        }
    }
    if (mismatches == 0)
        addEntry(report, AuditEntry::Info, "Indices",
                 "Indices de cedula consistentes con datos base.");
}
