#pragma once

#include <QObject>
#include <QVector>
#include <memory>

#include "Patient.h"
#include "Consulta.h"
#include "Diagnostico.h"

// Generates reproducible synthetic data for all entities.
// Uses a seeded std::mt19937 so results are deterministic per seed.
class DataGenerator : public QObject
{
    Q_OBJECT

public:
    // Supported dataset sizes
    enum class DatasetSize {
        Small  =    500,
        Medium =   5000,
        Large  =  50000,
        XLarge = 200000
    };

    explicit DataGenerator(QObject *parent = nullptr);

    // Seed the random engine (default 42)
    void setSeed(unsigned int seed);
    unsigned int seed() const { return m_seed; }

    // Generate patients
    QVector<Patient> generatePatients(int count);
    QVector<Patient> generatePatients(DatasetSize size);

    // Generate consultations (linked to existing patient cédulas)
    QVector<Consulta> generateConsultas(const QVector<Patient> &patients, int count);
    QVector<Consulta> generateConsultas(const QVector<Patient> &patients, DatasetSize size);

    // Build the N-ary diagnosis tree from the static data pools
    std::shared_ptr<DiagnosticoNode> buildDiagnosisTree();

    // Convenience: generate everything at once
    struct FullDataset {
        QVector<Patient>  patients;
        QVector<Consulta> consultas;
        std::shared_ptr<DiagnosticoNode> diagnosisTree;
    };
    FullDataset generateAll(DatasetSize size);

signals:
    void progressChanged(int percent);   // 0-100

private:
    unsigned int m_seed = 42;
    // Internal RNG state (re-seeded each generation call)
    unsigned int m_rng = 0;

    int    nextInt(int min, int max);
    double nextDouble(double min, double max);
    QString pickRandom(const QStringList &list);
    QDate  randomDate(const QDate &from, const QDate &to);
};
