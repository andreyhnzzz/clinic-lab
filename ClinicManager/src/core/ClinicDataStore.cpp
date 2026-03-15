#include "ClinicDataStore.h"
#include "../utils/DataGenerator.h"
#include <algorithm>

ClinicDataStore& ClinicDataStore::instance() {
    static ClinicDataStore store;
    return store;
}

ClinicDataStore::ClinicDataStore(QObject* parent)
    : QObject(parent) {}

void ClinicDataStore::addPaciente(const Paciente& p) {
    if (containsCedula(QString::fromStdString(p.cedula))) return;
    int idx = pacientes_.size();
    pacientes_.push_back(p);
    cedulaIndex_.insert(p.cedula, idx);
    emit dataChanged();
}

void ClinicDataStore::loadPacientes(const QVector<Paciente>& list) {
    pacientes_.clear();
    cedulaIndex_.clear();
    for (const auto& p : list) {
        if (cedulaIndex_.contains(p.cedula)) continue; // skip duplicates
        int idx = pacientes_.size();
        pacientes_.push_back(p);
        cedulaIndex_.insert(p.cedula, idx);
    }
    emit dataChanged();
}

Paciente ClinicDataStore::findByCedula(const QString& cedula) const {
    auto opt = cedulaIndex_.find(cedula.toStdString());
    if (opt && *opt >= 0 && *opt < pacientes_.size())
        return pacientes_[*opt];
    return {};
}

bool ClinicDataStore::containsCedula(const QString& cedula) const {
    return cedulaIndex_.contains(cedula.toStdString());
}

void ClinicDataStore::addConsulta(const Consulta& c) {
    int idx = consultas_.size();
    consultas_.push_back(c);
    consultasByPatient_[c.cedulaPaciente].push_back(idx);
    emit dataChanged();
}

void ClinicDataStore::loadConsultas(const QVector<Consulta>& list) {
    consultas_ = list;
    std::sort(consultas_.begin(), consultas_.end(),
              [](const Consulta& a, const Consulta& b){ return a.fecha < b.fecha; });
    consultasByPatient_.clear();
    for (int i = 0; i < consultas_.size(); ++i)
        consultasByPatient_[consultas_[i].cedulaPaciente].push_back(i);
    emit dataChanged();
}

QVector<Consulta> ClinicDataStore::consultasPorPaciente(const QString& cedula) const {
    QVector<Consulta> result;
    auto it = consultasByPatient_.find(cedula.toStdString());
    if (it != consultasByPatient_.end()) {
        for (int idx : *it) {
            if (idx >= 0 && idx < consultas_.size())
                result.push_back(consultas_[idx]);
        }
    }
    return result;
}

void ClinicDataStore::generateSampleData(int numPacientes) {
    auto pacs = DataGenerator::generatePacientes(numPacientes);
    auto cons = DataGenerator::generateConsultas(pacs, numPacientes * 2);
    loadPacientes(pacs);
    loadConsultas(cons);
}

void ClinicDataStore::clear() {
    pacientes_.clear();
    consultas_.clear();
    cedulaIndex_.clear();
    consultasByPatient_.clear();
    emit dataChanged();
}

void ClinicDataStore::rebuildIndices() {
    cedulaIndex_.clear();
    for (int i = 0; i < pacientes_.size(); ++i)
        cedulaIndex_.insert(pacientes_[i].cedula, i);
    consultasByPatient_.clear();
    for (int i = 0; i < consultas_.size(); ++i)
        consultasByPatient_[consultas_[i].cedulaPaciente].push_back(i);
}
