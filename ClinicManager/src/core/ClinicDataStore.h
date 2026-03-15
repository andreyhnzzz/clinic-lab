#pragma once
#include <QObject>
#include <QVector>
#include <QString>
#include <QMap>
#include "../models/Paciente.h"
#include "../models/Consulta.h"
#include "../data_structures/DiagnosisTree.h"
#include "../data_structures/HashTable.h"

class ClinicDataStore : public QObject {
    Q_OBJECT
public:
    static ClinicDataStore& instance();

    // Threading model:
    // - All mutations are confined to the object's owner thread (UI thread).
    // - If a mutation is requested from another thread, it is marshalled
    //   synchronously to the owner thread.
    // - Read APIs are expected to be called from the owner thread.

    // --- Patients ---
    void addPaciente(const Paciente& p);
    void loadPacientes(const QVector<Paciente>& list);
    const QVector<Paciente>& pacientes() const { return pacientes_; }
    int pacienteCount() const { return pacientes_.size(); }
    Paciente findByCedula(const QString& cedula) const;
    bool containsCedula(const QString& cedula) const;

    // --- Consultations ---
    void addConsulta(const Consulta& c);
    void loadConsultas(const QVector<Consulta>& list);
    const QVector<Consulta>& consultas() const { return consultas_; }
    int consultaCount() const { return consultas_.size(); }
    QVector<Consulta> consultasPorPaciente(const QString& cedula) const;

    // --- Diagnosis Tree ---
    DiagnosisTree& diagnosisTree() { return tree_; }
    const DiagnosisTree& diagnosisTree() const { return tree_; }

    // --- Bulk generation ---
    void generateSampleData(int numPacientes);
    void clear();

signals:
    void dataChanged();

private:
    explicit ClinicDataStore(QObject* parent = nullptr);
    void rebuildIndices();
    bool isOwnerThread() const;
    void assertOwnerThread(const char* funcName) const;

    QVector<Paciente> pacientes_;
    QVector<Consulta> consultas_;
    HashTable<std::string, int> cedulaIndex_;  // cedula -> index in pacientes_
    QMap<std::string, QVector<int>> consultasByPatient_; // cedula -> indices in consultas_
    DiagnosisTree tree_;
};
