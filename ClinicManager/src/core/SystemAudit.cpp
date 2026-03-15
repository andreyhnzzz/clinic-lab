#include "SystemAudit.h"
#include "ClinicDataStore.h"
#include <QSet>
#include <QRegularExpression>

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

    checkDuplicatePatients(report);
    checkOrphanConsultations(report);
    checkInvalidPriorities(report);
    checkInvalidDates(report);
    checkInvalidCosts(report);
    checkInvalidGravedad(report);
    checkIndexConsistency(report);

    if (report.totalIssues() == 0) {
        addEntry(report, AuditEntry::Info, "Sistema",
                 "Auditoria completada sin problemas detectados.");
    }

    lastReport_ = report;
    emit auditCompleted(report);
    return report;
}

void SystemAudit::checkDuplicatePatients(AuditReport& report) {
    const auto& pacs = ClinicDataStore::instance().pacientes();
    QSet<QString> seen;
    int dupes = 0;
    for (const auto& p : pacs) {
        QString ced = QString::fromStdString(p.cedula);
        if (seen.contains(ced)) {
            dupes++;
            if (dupes <= 5) {
                addEntry(report, AuditEntry::Critical, "Duplicados",
                         QString("Paciente duplicado: cedula %1").arg(ced));
            }
        }
        seen.insert(ced);
    }
    if (dupes > 5)
        addEntry(report, AuditEntry::Critical, "Duplicados",
                 QString("...y %1 duplicados mas").arg(dupes - 5));
    if (dupes == 0)
        addEntry(report, AuditEntry::Info, "Duplicados",
                 "No se encontraron pacientes duplicados.");
}

void SystemAudit::checkOrphanConsultations(AuditReport& report) {
    const auto& cons = ClinicDataStore::instance().consultas();
    int orphans = 0;
    for (const auto& c : cons) {
        if (!ClinicDataStore::instance().containsCedula(
                QString::fromStdString(c.cedulaPaciente))) {
            orphans++;
            if (orphans <= 3) {
                addEntry(report, AuditEntry::Warning, "Consultas huerfanas",
                         QString("Consulta %1 referencia paciente inexistente: %2")
                             .arg(QString::fromStdString(c.idConsulta))
                             .arg(QString::fromStdString(c.cedulaPaciente)));
            }
        }
    }
    if (orphans > 3)
        addEntry(report, AuditEntry::Warning, "Consultas huerfanas",
                 QString("...y %1 consultas huerfanas mas").arg(orphans - 3));
    if (orphans == 0)
        addEntry(report, AuditEntry::Info, "Consultas huerfanas",
                 "Todas las consultas tienen paciente valido.");
}

void SystemAudit::checkInvalidPriorities(AuditReport& report) {
    const auto& pacs = ClinicDataStore::instance().pacientes();
    int invalid = 0;
    for (const auto& p : pacs) {
        if (p.prioridad < 1 || p.prioridad > 3) {
            invalid++;
            if (invalid <= 3) {
                addEntry(report, AuditEntry::Warning, "Prioridades",
                         QString("Paciente %1 tiene prioridad fuera de rango: %2")
                             .arg(QString::fromStdString(p.cedula))
                             .arg(p.prioridad));
            }
        }
    }
    if (invalid == 0)
        addEntry(report, AuditEntry::Info, "Prioridades",
                 "Todas las prioridades estan en rango valido (1-3).");
}

void SystemAudit::checkInvalidDates(AuditReport& report) {
    static QRegularExpression dateRx("^\\d{4}-\\d{2}-\\d{2}$");
    const auto& pacs = ClinicDataStore::instance().pacientes();
    const auto& cons = ClinicDataStore::instance().consultas();
    int invalid = 0;
    for (const auto& p : pacs) {
        QString fecha = QString::fromStdString(p.fechaRegistro);
        if (!dateRx.match(fecha).hasMatch()) {
            invalid++;
            if (invalid <= 3)
                addEntry(report, AuditEntry::Warning, "Fechas",
                         QString("Paciente %1: fecha de registro invalida '%2'")
                             .arg(QString::fromStdString(p.cedula)).arg(fecha));
        }
    }
    for (const auto& c : cons) {
        QString fecha = QString::fromStdString(c.fecha);
        if (!dateRx.match(fecha).hasMatch()) {
            invalid++;
            if (invalid <= 3)
                addEntry(report, AuditEntry::Warning, "Fechas",
                         QString("Consulta %1: fecha invalida '%2'")
                             .arg(QString::fromStdString(c.idConsulta)).arg(fecha));
        }
    }
    if (invalid == 0)
        addEntry(report, AuditEntry::Info, "Fechas",
                 "Todas las fechas tienen formato valido.");
}

void SystemAudit::checkInvalidCosts(AuditReport& report) {
    const auto& cons = ClinicDataStore::instance().consultas();
    int invalid = 0;
    for (const auto& c : cons) {
        if (c.costo < 0.0) {
            invalid++;
            if (invalid <= 3)
                addEntry(report, AuditEntry::Warning, "Costos",
                         QString("Consulta %1: costo negativo %2")
                             .arg(QString::fromStdString(c.idConsulta))
                             .arg(c.costo));
        }
    }
    if (invalid == 0)
        addEntry(report, AuditEntry::Info, "Costos",
                 "Todos los costos de consultas son validos.");
}

void SystemAudit::checkInvalidGravedad(AuditReport& report) {
    const auto& cons = ClinicDataStore::instance().consultas();
    int invalid = 0;
    for (const auto& c : cons) {
        if (c.gravedad < 1 || c.gravedad > 5) {
            invalid++;
            if (invalid <= 3)
                addEntry(report, AuditEntry::Warning, "Gravedad",
                         QString("Consulta %1: gravedad fuera de rango: %2")
                             .arg(QString::fromStdString(c.idConsulta))
                             .arg(c.gravedad));
        }
    }
    if (invalid == 0)
        addEntry(report, AuditEntry::Info, "Gravedad",
                 "Todas las gravedades estan en rango valido (1-5).");
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
