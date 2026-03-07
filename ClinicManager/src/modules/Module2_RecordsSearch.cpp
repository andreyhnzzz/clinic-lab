#include "Module2_RecordsSearch.h"
#include "../algorithms/searching/LinearSearch.h"
#include "../algorithms/searching/BinarySearch.h"
#include <chrono>
#include <algorithm>

Module2_RecordsSearch::Module2_RecordsSearch(QObject* parent)
    : QObject(parent) {}

void Module2_RecordsSearch::loadData(const QVector<Paciente>& pacientes,
                                      const QVector<Consulta>& consultas) {
    pacientes_ = pacientes;
    // Sort consultas by date for binary search
    consultas_ = consultas;
    std::sort(consultas_.begin(), consultas_.end(),
              [](const Consulta& a, const Consulta& b){ return a.fecha < b.fecha; });
    // Build hash index
    hashIndex_.clear();
    for (const auto& p : pacientes_)
        hashIndex_.insert(p.cedula, p);
}

void Module2_RecordsSearch::addPaciente(const Paciente& p) {
    pacientes_.push_back(p);
    hashIndex_.insert(p.cedula, p);
}

void Module2_RecordsSearch::addConsulta(const Consulta& c) {
    // Insert in sorted position
    auto it = std::lower_bound(consultas_.begin(), consultas_.end(), c,
        [](const Consulta& a, const Consulta& b){ return a.fecha < b.fecha; });
    consultas_.insert(it, c);
}

Paciente Module2_RecordsSearch::searchByCedula(const QString& cedula) {
    auto t0 = std::chrono::high_resolution_clock::now();
    auto result = hashIndex_.find(cedula.toStdString());
    auto t1 = std::chrono::high_resolution_clock::now();
    lastSearchTimeMs_ = std::chrono::duration<double, std::milli>(t1 - t0).count();
    if (result) return *result;
    return {};
}

QVector<Paciente> Module2_RecordsSearch::searchByName(const QString& name) {
    auto t0 = std::chrono::high_resolution_clock::now();
    QString lower = name.toLower();
    QVector<Paciente> results = linearSearchAll<Paciente>(pacientes_,
        [&](const Paciente& p){
            return QString::fromStdString(p.nombre).toLower().contains(lower);
        });
    auto t1 = std::chrono::high_resolution_clock::now();
    lastSearchTimeMs_ = std::chrono::duration<double, std::milli>(t1 - t0).count();
    return results;
}

QVector<Consulta> Module2_RecordsSearch::searchByDateRange(const QString& from, const QString& to) {
    auto t0 = std::chrono::high_resolution_clock::now();
    std::string sFrom = from.toStdString();
    std::string sTo = to.toStdString();
    QVector<Consulta> results = binarySearchRange<Consulta>(
        consultas_,
        [&](const Consulta& c){ return c.fecha >= sFrom; },
        [&](const Consulta& c){ return c.fecha <= sTo; }
    );
    auto t1 = std::chrono::high_resolution_clock::now();
    lastSearchTimeMs_ = std::chrono::duration<double, std::milli>(t1 - t0).count();
    return results;
}

QVector<Consulta> Module2_RecordsSearch::searchByGravedad(int gravedad) {
    auto t0 = std::chrono::high_resolution_clock::now();
    QVector<Consulta> results = linearSearchAll<Consulta>(consultas_,
        [&](const Consulta& c){ return c.gravedad == gravedad; });
    auto t1 = std::chrono::high_resolution_clock::now();
    lastSearchTimeMs_ = std::chrono::duration<double, std::milli>(t1 - t0).count();
    return results;
}

QVector<Paciente> Module2_RecordsSearch::getAllPacientes() const {
    return pacientes_;
}

QVector<Consulta> Module2_RecordsSearch::getAllConsultas() const {
    return consultas_;
}
