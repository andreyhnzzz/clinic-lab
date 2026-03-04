#include "DataGenerator.h"
#include "CostaRicanData.h"

#include <QDate>
#include <random>

// ── Constructor ──────────────────────────────────────────────────────────────

DataGenerator::DataGenerator(QObject *parent)
    : QObject(parent), m_seed(42), m_rng(42) {}

// ── Seed ─────────────────────────────────────────────────────────────────────

void DataGenerator::setSeed(unsigned int seed)
{
    m_seed = seed;
    m_rng  = seed;
}

// ── RNG helpers (Linear Congruential Generator) ───────────────────────────────
// Fast, deterministic, and dependency-free.

int DataGenerator::nextInt(int min, int max)
{
    // LCG: Numerical Recipes constants
    m_rng = m_rng * 1664525u + 1013904223u;
    return min + static_cast<int>(m_rng % static_cast<unsigned int>(max - min + 1));
}

double DataGenerator::nextDouble(double min, double max)
{
    m_rng = m_rng * 1664525u + 1013904223u;
    double norm = static_cast<double>(m_rng) / static_cast<double>(UINT_MAX);
    return min + norm * (max - min);
}

QString DataGenerator::pickRandom(const QStringList &list)
{
    if (list.isEmpty()) return {};
    return list.at(nextInt(0, list.size() - 1));
}

QDate DataGenerator::randomDate(const QDate &from, const QDate &to)
{
    int daysDiff = from.daysTo(to);
    if (daysDiff <= 0) return from;
    return from.addDays(nextInt(0, daysDiff));
}

// ── Patient generation ────────────────────────────────────────────────────────

QVector<Patient> DataGenerator::generatePatients(DatasetSize size)
{
    return generatePatients(static_cast<int>(size));
}

QVector<Patient> DataGenerator::generatePatients(int count)
{
    m_rng = m_seed;   // Reset RNG for reproducibility
    QVector<Patient> result;
    result.reserve(count);

    const QDate minDate(1990, 1, 1);
    const QDate maxDate = QDate::currentDate();
    const QDate minBirth(1930, 1, 1);
    const QDate maxBirth(2010, 12, 31);

    const QStringList &maleNames   = CostaRicanData::firstNamesMale();
    const QStringList &femaleNames = CostaRicanData::firstNamesFemale();
    const QStringList &lastNames   = CostaRicanData::lastNames();
    const QStringList &bloods      = CostaRicanData::bloodTypes();
    const QStringList &diags       = CostaRicanData::commonDiagnoses();

    for (int i = 0; i < count; ++i) {
        // Gender-aware name selection
        bool isMale = (nextInt(0, 1) == 0);
        QString firstName = isMale ? pickRandom(maleNames) : pickRandom(femaleNames);
        QString lastName1 = pickRandom(lastNames);
        QString lastName2 = pickRandom(lastNames);
        QString nombre = firstName + " " + lastName1 + " " + lastName2;

        // Cédula: province(1) + block(3) + individual(5) = 9 digits
        int province   = nextInt(1, 9);
        int block      = nextInt(1, 999);
        int individual = nextInt(1, 99999);
        QString cedula = QString("%1%2%3")
                         .arg(province)
                         .arg(block,      3, 10, QLatin1Char('0'))
                         .arg(individual, 5, 10, QLatin1Char('0'));

        // Age derived from birthdate
        QDate birthDate = randomDate(minBirth, maxBirth);
        int edad = birthDate.daysTo(QDate::currentDate()) / 365;

        QDate fechaReg = randomDate(minDate, maxDate);
        Priority priority = static_cast<Priority>(nextInt(1, 3));
        BloodType bt = Patient::bloodTypeFromString(pickRandom(bloods));
        QString diagnostico = pickRandom(diags);

        result.append(Patient(cedula, nombre, edad, fechaReg, priority, bt, diagnostico));

        if (count > 1000 && i % (count / 20) == 0)
            emit progressChanged(static_cast<int>((double)i / count * 100));
    }

    emit progressChanged(100);
    return result;
}

// ── Consulta generation ───────────────────────────────────────────────────────

QVector<Consulta> DataGenerator::generateConsultas(const QVector<Patient> &patients, DatasetSize size)
{
    return generateConsultas(patients, static_cast<int>(size));
}

QVector<Consulta> DataGenerator::generateConsultas(const QVector<Patient> &patients, int count)
{
    if (patients.isEmpty()) return {};

    m_rng = m_seed + 1;  // Offset seed so patients/consultas don't share RNG sequence
    QVector<Consulta> result;
    result.reserve(count);

    const QDate minDate(2015, 1, 1);
    const QDate maxDate = QDate::currentDate();
    const QStringList &doctors = CostaRicanData::doctorNames();
    const QStringList &diags   = CostaRicanData::commonDiagnoses();

    for (int i = 0; i < count; ++i) {
        int patientIndex = nextInt(0, patients.size() - 1);
        const Patient &p = patients.at(patientIndex);

        int     id         = i + 1;
        QString cedula     = p.cedula();
        QDate   fecha      = randomDate(minDate, maxDate);
        QString medico     = pickRandom(doctors);
        QString diagnostico = pickRandom(diags);
        int     gravedad   = nextInt(1, 5);
        double  costo      = nextDouble(5000.0, 250000.0); // Costa Rican colones

        result.append(Consulta(id, cedula, fecha, medico, diagnostico, gravedad, costo));

        if (count > 1000 && i % (count / 20) == 0)
            emit progressChanged(static_cast<int>((double)i / count * 100));
    }

    emit progressChanged(100);
    return result;
}

// ── Diagnosis tree ────────────────────────────────────────────────────────────

std::shared_ptr<DiagnosticoNode> DataGenerator::buildDiagnosisTree()
{
    using Level = DiagnosticoNode::Level;

    // Root node (virtual, not displayed)
    auto root = std::make_shared<DiagnosticoNode>(
        "ROOT", "Árbol de Diagnósticos", "", "", "", Level::Area);

    const QStringList &areas     = CostaRicanData::medicalAreas();
    const QList<QStringList> &specs  = CostaRicanData::specialtiesByArea();
    const QList<QStringList> &ddiags = CostaRicanData::diagnosesBySpecialty();

    int specialtyGlobalIndex = 0;

    for (int aIdx = 0; aIdx < areas.size(); ++aIdx) {
        auto areaNode = std::make_shared<DiagnosticoNode>(
            QString("A%1").arg(aIdx + 1, 2, 10, QLatin1Char('0')),
            areas.at(aIdx),
            areas.at(aIdx),
            "",
            "Área de " + areas.at(aIdx),
            Level::Area);

        const QStringList &areaSpecs = (aIdx < specs.size()) ? specs.at(aIdx) : QStringList{};

        for (int sIdx = 0; sIdx < areaSpecs.size(); ++sIdx) {
            QString specCode = QString("S%1").arg(specialtyGlobalIndex + 1, 3, 10, QLatin1Char('0'));
            auto specNode = std::make_shared<DiagnosticoNode>(
                specCode,
                areaSpecs.at(sIdx),
                areas.at(aIdx),
                areaSpecs.at(sIdx),
                "Especialidad: " + areaSpecs.at(sIdx),
                Level::Specialty);

            // Attach diagnoses
            if (specialtyGlobalIndex < ddiags.size()) {
                const QStringList &specDiags = ddiags.at(specialtyGlobalIndex);
                for (int dIdx = 0; dIdx < specDiags.size(); ++dIdx) {
                    QString diagCode = QString("D%1%2")
                                       .arg(specialtyGlobalIndex + 1, 2, 10, QLatin1Char('0'))
                                       .arg(dIdx + 1, 2, 10, QLatin1Char('0'));
                    auto diagNode = std::make_shared<DiagnosticoNode>(
                        diagCode,
                        specDiags.at(dIdx),
                        areas.at(aIdx),
                        areaSpecs.at(sIdx),
                        specDiags.at(dIdx),
                        Level::Diagnosis);
                    specNode->addChild(std::move(diagNode));
                }
            }

            areaNode->addChild(std::move(specNode));
            ++specialtyGlobalIndex;
        }

        root->addChild(std::move(areaNode));
    }

    return root;
}

// ── Full dataset ──────────────────────────────────────────────────────────────

DataGenerator::FullDataset DataGenerator::generateAll(DatasetSize size)
{
    FullDataset ds;
    ds.patients     = generatePatients(size);
    ds.consultas    = generateConsultas(ds.patients, size);
    ds.diagnosisTree = buildDiagnosisTree();
    return ds;
}
