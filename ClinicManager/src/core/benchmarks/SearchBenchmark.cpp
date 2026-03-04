#include "SearchBenchmark.h"

SearchBenchmark::SearchBenchmark(QObject *parent)
    : QObject(parent) {}

QVector<SearchBenchmarkResult> SearchBenchmark::runAll(const QVector<Patient> &data,
                                                        const QString &cedula)
{
    QVector<SearchBenchmarkResult> results;

    // Linear search on the original (unsorted) data
    results.append(runLinear(data, cedula));

    // Binary search on a sorted copy (sorting not timed)
    QVector<Patient> sorted = SearchAlgorithms::sortByCedula(data);
    results.append(runBinary(sorted, cedula));

    emit allFinished(results);
    return results;
}

SearchBenchmarkResult SearchBenchmark::runLinear(const QVector<Patient> &data,
                                                  const QString &cedula)
{
    SearchResult sr = SearchAlgorithms::linearSearchByCedula(data, cedula);
    SearchBenchmarkResult res;
    res.algorithm   = "Búsqueda Lineal";
    res.dataSize    = data.size();
    res.elapsedUs   = sr.elapsedUs;
    res.comparisons = sr.comparisons;
    res.found       = (sr.index != -1);
    res.searchTerm  = cedula;
    return res;
}

SearchBenchmarkResult SearchBenchmark::runBinary(const QVector<Patient> &sortedData,
                                                  const QString &cedula)
{
    SearchResult sr = SearchAlgorithms::binarySearchByCedula(sortedData, cedula);
    SearchBenchmarkResult res;
    res.algorithm   = "Búsqueda Binaria";
    res.dataSize    = sortedData.size();
    res.elapsedUs   = sr.elapsedUs;
    res.comparisons = sr.comparisons;
    res.found       = (sr.index != -1);
    res.searchTerm  = cedula;
    return res;
}
