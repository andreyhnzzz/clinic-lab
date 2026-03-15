#include "DataConsistencyValidator.h"
#include <string>

#include <regex>

namespace {
DataValidationIssue makeIssue(ValidationSeverity severity,
                              const QString& category,
                              const QString& message) {
    DataValidationIssue issue;
    issue.severity = severity;
    issue.category = category;
    issue.message = message;
    return issue;
}
}  // namespace

namespace DataConsistencyValidator {

bool isValidIsoDate(const QString& dateStr) {
    static const std::regex rx("^\\d{4}-\\d{2}-\\d{2}$");
    const std::string s = dateStr.toStdString();
    if (!std::regex_match(s, rx)) return false;

    int year = std::stoi(s.substr(0, 4));
    int month = std::stoi(s.substr(5, 2));
    int day = std::stoi(s.substr(8, 2));

    if (month < 1 || month > 12) return false;

    int maxDay = 31;
    if (month == 4 || month == 6 || month == 9 || month == 11) {
        maxDay = 30;
    } else if (month == 2) {
        bool leap = ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
        maxDay = leap ? 29 : 28;
    }

    return day >= 1 && day <= maxDay;
}

std::set<QString> collectCedulas(const QVector<Paciente>& pacientes) {
    std::set<QString> cedulas;
    for (const auto& p : pacientes) {
        cedulas.insert(QString::fromStdString(p.cedula).trimmed());
    }
    return cedulas;
}

QVector<DataValidationIssue> validatePacientes(const QVector<Paciente>& pacientes) {
    QVector<DataValidationIssue> issues;
    std::set<QString> seenCedulas;

    for (int i = 0; i < pacientes.size(); ++i) {
        const auto& p = pacientes[i];
        QString cedula = QString::fromStdString(p.cedula).trimmed();
        QString fecha = QString::fromStdString(p.fechaRegistro).trimmed();

        if (cedula.isEmpty()) {
            issues.push_back(makeIssue(ValidationSeverity::Critical, "Duplicados",
                QString("Paciente[%1]: cedula vacia.").arg(i)));
        } else if (seenCedulas.find(cedula) != seenCedulas.end()) {
            issues.push_back(makeIssue(ValidationSeverity::Critical, "Duplicados",
                QString("Paciente duplicado: cedula %1").arg(cedula)));
        }
        seenCedulas.insert(cedula);

        if (p.prioridad < 1 || p.prioridad > 3) {
            issues.push_back(makeIssue(ValidationSeverity::Warning, "Prioridades",
                QString("Paciente %1 tiene prioridad fuera de rango: %2")
                    .arg(cedula.isEmpty() ? QString("(sin cedula)") : cedula)
                    .arg(p.prioridad)));
        }

        if (!isValidIsoDate(fecha)) {
            issues.push_back(makeIssue(ValidationSeverity::Warning, "Fechas",
                QString("Paciente %1: fecha de registro invalida '%2'")
                    .arg(cedula.isEmpty() ? QString("(sin cedula)") : cedula)
                    .arg(fecha)));
        }
    }

    return issues;
}

QVector<DataValidationIssue> validateConsultas(const QVector<Consulta>& consultas,
                                               const std::set<QString>& validCedulas) {
    QVector<DataValidationIssue> issues;

    for (int i = 0; i < consultas.size(); ++i) {
        const auto& c = consultas[i];
        QString cedulaPaciente = QString::fromStdString(c.cedulaPaciente).trimmed();
        QString fecha = QString::fromStdString(c.fecha).trimmed();

        if (cedulaPaciente.isEmpty() || validCedulas.find(cedulaPaciente) == validCedulas.end()) {
            issues.push_back(makeIssue(ValidationSeverity::Warning, "Consultas huerfanas",
                QString("Consulta %1 referencia paciente inexistente: %2")
                    .arg(QString::fromStdString(c.idConsulta))
                    .arg(cedulaPaciente.isEmpty() ? QString("(vacia)") : cedulaPaciente)));
        }

        if (!isValidIsoDate(fecha)) {
            issues.push_back(makeIssue(ValidationSeverity::Warning, "Fechas",
                QString("Consulta %1: fecha invalida '%2'")
                    .arg(QString::fromStdString(c.idConsulta))
                    .arg(fecha)));
        }

        if (c.gravedad < 1 || c.gravedad > 5) {
            issues.push_back(makeIssue(ValidationSeverity::Warning, "Gravedad",
                QString("Consulta %1: gravedad fuera de rango: %2")
                    .arg(QString::fromStdString(c.idConsulta))
                    .arg(c.gravedad)));
        }

        if (c.costo < 0.0) {
            issues.push_back(makeIssue(ValidationSeverity::Warning, "Costos",
                QString("Consulta %1: costo negativo %2")
                    .arg(QString::fromStdString(c.idConsulta))
                    .arg(c.costo)));
        }
    }

    return issues;
}

QVector<DataValidationIssue> validateDataset(const QVector<Paciente>& pacientes,
                                             const QVector<Consulta>& consultas) {
    QVector<DataValidationIssue> issues = validatePacientes(pacientes);
    std::set<QString> cedulas = collectCedulas(pacientes);
    issues += validateConsultas(consultas, cedulas);
    return issues;
}

}  // namespace DataConsistencyValidator
