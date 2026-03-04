#pragma once

#include <QVector>
#include <QString>
#include <functional>
#include "Patient.h"

// Linear and binary search algorithms for Patient vectors.
// Binary search requires the vector to be sorted by the provided key.

struct SearchResult {
    int     index       = -1;   // -1 if not found
    qint64  elapsedUs   = 0;    // microseconds
    int     comparisons = 0;
};

class SearchAlgorithms
{
public:
    // ── Linear search ─────────────────────────────────────────────────────────
    // O(n) - works on any (unsorted) collection
    static SearchResult linearSearchByCedula(const QVector<Patient> &data,
                                              const QString &cedula);

    static SearchResult linearSearchByNombre(const QVector<Patient> &data,
                                              const QString &nombre);

    // ── Binary search ─────────────────────────────────────────────────────────
    // O(log n) - requires data sorted by cédula (ascending)
    static SearchResult binarySearchByCedula(const QVector<Patient> &sortedData,
                                              const QString &cedula);

    // ── Sort helpers ──────────────────────────────────────────────────────────
    // Sort a copy of the vector by cédula for binary-search use
    static QVector<Patient> sortByCedula(QVector<Patient> data);
};
