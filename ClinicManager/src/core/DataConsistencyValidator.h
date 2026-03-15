#pragma once

#include <QString>
#include <QVector>
#include <set>
#include "../models/Paciente.h"
#include "../models/Consulta.h"

enum class ValidationSeverity {
    Warning,
    Critical
};

struct DataValidationIssue {
    ValidationSeverity severity = ValidationSeverity::Warning;
    QString category;
    QString message;
};

namespace DataConsistencyValidator {

bool isValidIsoDate(const QString& dateStr);
std::set<QString> collectCedulas(const QVector<Paciente>& pacientes);
QVector<DataValidationIssue> validatePacientes(const QVector<Paciente>& pacientes);
QVector<DataValidationIssue> validateConsultas(const QVector<Consulta>& consultas,
                                               const std::set<QString>& validCedulas);
QVector<DataValidationIssue> validateDataset(const QVector<Paciente>& pacientes,
                                             const QVector<Consulta>& consultas);

}  // namespace DataConsistencyValidator
