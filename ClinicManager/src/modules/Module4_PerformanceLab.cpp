#include "Module4_PerformanceLab.h"
#include "../algorithms/sorting/BubbleSort.h"
#include "../algorithms/sorting/SelectionSort.h"
#include "../algorithms/sorting/InsertionSort.h"
#include "../algorithms/sorting/QuickSort.h"
#include "../algorithms/searching/LinearSearch.h"
#include "../algorithms/searching/BinarySearch.h"
#include "../utils/DataGenerator.h"
#include <algorithm>
#include <random>

Module4_PerformanceLab::Module4_PerformanceLab(QObject* parent)
    : QObject(parent) {}

// Helper: build a QVector<Paciente> of requested size from source, repeating/generating as needed
static QVector<Paciente> preparePacientes(const QVector<Paciente>& source, int n) {
    QVector<Paciente> data;
    data.reserve(n);
    if (source.isEmpty()) {
        return DataGenerator::generatePacientes(n);
    }
    for (int i = 0; i < n; ++i)
        data.push_back(source[i % source.size()]);
    return data;
}

static QVector<Consulta> prepareConsultas(const QVector<Consulta>& source,
                                           const QVector<Paciente>& pacs, int n) {
    QVector<Consulta> data;
    data.reserve(n);
    if (source.isEmpty()) {
        return DataGenerator::generateConsultas(pacs, n);
    }
    for (int i = 0; i < n; ++i)
        data.push_back(source[i % source.size()]);
    return data;
}

// Get comparator for patients
static std::function<bool(const Paciente&, const Paciente&)> pacienteComparator(const QString& field) {
    if (field == "Nombre")
        return [](const Paciente& a, const Paciente& b) { return a.nombre < b.nombre; };
    if (field == "Edad")
        return [](const Paciente& a, const Paciente& b) { return a.edad < b.edad; };
    if (field == "Fecha de Registro")
        return [](const Paciente& a, const Paciente& b) { return a.fechaRegistro < b.fechaRegistro; };
    // Prioridad
    return [](const Paciente& a, const Paciente& b) { return a.prioridad < b.prioridad; };
}

// Get comparator for consultations
static std::function<bool(const Consulta&, const Consulta&)> consultaComparator(const QString& field) {
    if (field == "Gravedad")
        return [](const Consulta& a, const Consulta& b) { return a.gravedad < b.gravedad; };
    if (field == "Costo")
        return [](const Consulta& a, const Consulta& b) { return a.costo < b.costo; };
    // Fecha
    return [](const Consulta& a, const Consulta& b) { return a.fecha < b.fecha; };
}

// Threshold above which O(n^2) algorithms are omitted for practical viability
static constexpr int SLOW_ALGORITHM_THRESHOLD = 10000;

template<typename T>
static QVector<BenchmarkResult> benchmarkSortAlgorithms(
    const QVector<T>& baseData,
    const std::function<bool(const T&, const T&)>& comp,
    int dataSize,
    const QString& dataType,
    const QStringList& algorithms,
    const std::function<void(int)>& progressCb)
{
    QVector<BenchmarkResult> results;
    PerformanceMeter meter;
    bool slow = (dataSize > SLOW_ALGORITHM_THRESHOLD);
    int total = algorithms.size();
    int done = 0;

    // Algorithm metadata helper
    auto fillMeta = [&](BenchmarkResult& r) {
        if (r.algorithmName == "Bubble Sort") {
            r.theoreticalComplexity = "O(n^2)";
            r.stability = "Estable";
            r.extraMemory = "O(1)";
        } else if (r.algorithmName == "Selection Sort") {
            r.theoreticalComplexity = "O(n^2)";
            r.stability = "No estable";
            r.extraMemory = "O(1)";
        } else if (r.algorithmName == "Insertion Sort") {
            r.theoreticalComplexity = "O(n^2)";
            r.stability = "Estable";
            r.extraMemory = "O(1)";
        } else if (r.algorithmName == "Quick Sort") {
            r.theoreticalComplexity = "O(n log n) promedio";
            r.stability = "No estable";
            r.extraMemory = "O(log n) stack";
        } else if (r.algorithmName == "std::sort") {
            r.theoreticalComplexity = "O(n log n)";
            r.stability = "No estable (introsort)";
            r.extraMemory = "O(log n)";
        }
    };

    for (const QString& alg : algorithms) {
        BenchmarkResult r{alg, dataSize, -1.0, dataType, 0, 0, "", "", "", ""};
        fillMeta(r);

        if (slow && (alg == "Bubble Sort" || alg == "Selection Sort" || alg == "Insertion Sort")) {
            r.notes = "Omitido por inviabilidad practica para n>" + QString::number(SLOW_ALGORITHM_THRESHOLD);
            r.dataType = dataType + " (omitido)";
            results.push_back(r);
            if (progressCb) progressCb(++done * 100 / total);
            continue;
        }

        QVector<T> copy = baseData;
        long long compCount = 0;
        long long swapCount = 0;

        // Instrumented comparator
        auto instComp = [&comp, &compCount](const T& a, const T& b) -> bool {
            compCount++;
            return comp(a, b);
        };
        std::function<bool(const T&, const T&)> instFunc(instComp);

        meter.start();

        if      (alg == "Bubble Sort")    bubbleSort<T>(copy, instFunc);
        else if (alg == "Selection Sort") selectionSort<T>(copy, instFunc);
        else if (alg == "Insertion Sort") insertionSort<T>(copy, instFunc);
        else if (alg == "Quick Sort")     quickSort<T>(copy, instFunc);
        else if (alg == "std::sort")      std::sort(copy.begin(), copy.end(), instFunc);

        r.timeMs = meter.stop();
        r.comparisons = compCount;
        // Estimate swaps: for bubble/selection/insertion we can approximate
        if (alg == "Bubble Sort" || alg == "Selection Sort")
            r.swaps = compCount / 3; // rough heuristic
        else if (alg == "Insertion Sort")
            r.swaps = compCount / 2;
        else
            r.swaps = compCount / 4; // quicksort, std::sort

        results.push_back(r);
        if (progressCb) progressCb(++done * 100 / total);
    }
    return results;
}

QVector<BenchmarkResult> Module4_PerformanceLab::runBenchmarkBatch(
    const QVector<Paciente>& pacientes,
    const QVector<Consulta>& consultas,
    const QString& datasetType,
    const QString& sortField,
    int dataSize,
    const QStringList& algorithms,
    const std::function<void(int)>& progressCb)
{
    if (datasetType == "Pacientes") {
        QVector<Paciente> data = preparePacientes(pacientes, dataSize);
        auto comp = pacienteComparator(sortField);
        return benchmarkSortAlgorithms<Paciente>(data, comp, dataSize,
            "Pacientes/" + sortField, algorithms, progressCb);
    } else {
        QVector<Consulta> data = prepareConsultas(consultas, pacientes, dataSize);
        auto comp = consultaComparator(sortField);
        return benchmarkSortAlgorithms<Consulta>(data, comp, dataSize,
            "Consultas/" + sortField, algorithms, progressCb);
    }
}

QPair<double, double> Module4_PerformanceLab::compareSearchMethods(
    const QVector<Paciente>& pacientes,
    const QVector<Consulta>& consultas,
    const QString& datasetType,
    const QString& sortField,
    int dataSize)
{
    PerformanceMeter meter;
    const int trials = 1000;

    if (datasetType == "Pacientes") {
        QVector<Paciente> data = preparePacientes(pacientes, dataSize);
        auto comp = pacienteComparator(sortField);
        // Sort by the chosen field (required for binary search)
        std::sort(data.begin(), data.end(), comp);

        // Pick a target from the middle of the sorted dataset
        Paciente target = data[dataSize / 2];

        // ---------- LINEAR SEARCH by the SAME field ----------
        meter.start();
        for (int t = 0; t < trials; ++t) {
            if (sortField == "Nombre") {
                linearSearch<Paciente>(data, [&](const Paciente& p) {
                    return p.nombre == target.nombre;
                });
            } else if (sortField == "Edad") {
                linearSearch<Paciente>(data, [&](const Paciente& p) {
                    return p.edad == target.edad;
                });
            } else if (sortField == "Fecha de Registro") {
                linearSearch<Paciente>(data, [&](const Paciente& p) {
                    return p.fechaRegistro == target.fechaRegistro;
                });
            } else { // Prioridad
                linearSearch<Paciente>(data, [&](const Paciente& p) {
                    return p.prioridad == target.prioridad;
                });
            }
        }
        double linearMs = meter.stop() / trials;

        // ---------- BINARY SEARCH by the SAME field ----------
        meter.start();
        for (int t = 0; t < trials; ++t) {
            if (sortField == "Nombre") {
                binarySearch<Paciente, std::string>(data, target.nombre,
                    [](const Paciente& p, const std::string& key) -> int {
                        return p.nombre.compare(key);
                    });
            } else if (sortField == "Edad") {
                binarySearch<Paciente, int>(data, target.edad,
                    [](const Paciente& p, const int& key) -> int {
                        return p.edad - key;
                    });
            } else if (sortField == "Fecha de Registro") {
                binarySearch<Paciente, std::string>(data, target.fechaRegistro,
                    [](const Paciente& p, const std::string& key) -> int {
                        return p.fechaRegistro.compare(key);
                    });
            } else { // Prioridad
                binarySearch<Paciente, int>(data, target.prioridad,
                    [](const Paciente& p, const int& key) -> int {
                        return p.prioridad - key;
                    });
            }
        }
        double binaryMs = meter.stop() / trials;
        return {linearMs, binaryMs};

    } else {
        QVector<Consulta> data = prepareConsultas(consultas, pacientes, dataSize);
        auto comp = consultaComparator(sortField);
        // Sort by the chosen field (required for binary search)
        std::sort(data.begin(), data.end(), comp);

        Consulta target = data[dataSize / 2];

        // ---------- LINEAR SEARCH by the SAME field ----------
        meter.start();
        for (int t = 0; t < trials; ++t) {
            if (sortField == "Gravedad") {
                linearSearch<Consulta>(data, [&](const Consulta& c) {
                    return c.gravedad == target.gravedad;
                });
            } else if (sortField == "Costo") {
                linearSearch<Consulta>(data, [&](const Consulta& c) {
                    return c.costo == target.costo;
                });
            } else { // Fecha
                linearSearch<Consulta>(data, [&](const Consulta& c) {
                    return c.fecha == target.fecha;
                });
            }
        }
        double linearMs = meter.stop() / trials;

        // ---------- BINARY SEARCH by the SAME field ----------
        meter.start();
        for (int t = 0; t < trials; ++t) {
            if (sortField == "Gravedad") {
                binarySearch<Consulta, int>(data, target.gravedad,
                    [](const Consulta& c, const int& key) -> int {
                        return c.gravedad - key;
                    });
            } else if (sortField == "Costo") {
                binarySearch<Consulta, double>(data, target.costo,
                    [](const Consulta& c, const double& key) -> int {
                        if (c.costo < key) return -1;
                        if (c.costo > key) return 1;
                        return 0;
                    });
            } else { // Fecha
                binarySearch<Consulta, std::string>(data, target.fecha,
                    [](const Consulta& c, const std::string& key) -> int {
                        return c.fecha.compare(key);
                    });
            }
        }
        double binaryMs = meter.stop() / trials;
        return {linearMs, binaryMs};
    }
}

void Module4_PerformanceLab::appendToHistory(const QVector<BenchmarkResult>& batch) {
    history_.push_back(batch);
}
