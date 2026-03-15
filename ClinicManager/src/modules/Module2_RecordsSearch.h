#pragma once
#include <QObject>
#include <QVector>
#include <QString>
#include "../models/Paciente.h"
#include "../models/Consulta.h"
#include "../data_structures/HashTable.h"

struct PatientRecord {
    Paciente paciente;
    QVector<Consulta> consultas;
    int totalConsultas = 0;
    int maxGravedad = 0;
    QString ultimaConsultaFecha;
};

class Module2_RecordsSearch : public QObject {
    Q_OBJECT
public:
    explicit Module2_RecordsSearch(QObject* parent = nullptr);

    void loadData(const QVector<Paciente>& pacientes, const QVector<Consulta>& consultas);
    void addPaciente(const Paciente& p);
    void addConsulta(const Consulta& c);

    Paciente searchByCedula(const QString& cedula);          // O(1) via HashTable
    QVector<Paciente> searchByName(const QString& name);     // Linear search
    QVector<Consulta> searchByDateRange(const QString& from, const QString& to); // Binary search
    QVector<Consulta> searchByGravedad(int gravedad);        // Linear search
    PatientRecord getPatientRecord(const QString& cedula);
    QVector<Paciente> getAllPacientes() const;
    QVector<Consulta> getAllConsultas() const;
    const QVector<Consulta>& getAllConsultasRef() const;

    double getLastSearchTimeMs() const { return lastSearchTimeMs_; }
    int pacienteCount() const { return static_cast<int>(pacientes_.size()); }
    int consultaCount() const { return static_cast<int>(consultas_.size()); }

private:
    QVector<Paciente> pacientes_;
    QVector<Consulta> consultas_;   // Kept sorted by fecha
    HashTable<std::string, Paciente> hashIndex_;

    double lastSearchTimeMs_ = 0.0;
};
