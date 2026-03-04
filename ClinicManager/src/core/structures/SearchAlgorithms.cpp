#include "SearchAlgorithms.h"
#include "TimerUtils.h"

#include <algorithm>

// ── Linear search ─────────────────────────────────────────────────────────────

SearchResult SearchAlgorithms::linearSearchByCedula(const QVector<Patient> &data,
                                                     const QString &cedula)
{
    SearchResult res;
    TimerUtils::ScopedTimer t;

    for (int i = 0; i < data.size(); ++i) {
        ++res.comparisons;
        if (data[i].cedula() == cedula) {
            res.index = i;
            break;
        }
    }

    res.elapsedUs = t.elapsedMicroseconds();
    return res;
}

SearchResult SearchAlgorithms::linearSearchByNombre(const QVector<Patient> &data,
                                                     const QString &nombre)
{
    SearchResult res;
    TimerUtils::ScopedTimer t;

    QString query = nombre.toLower();
    for (int i = 0; i < data.size(); ++i) {
        ++res.comparisons;
        if (data[i].nombre().toLower().contains(query)) {
            res.index = i;
            break;
        }
    }

    res.elapsedUs = t.elapsedMicroseconds();
    return res;
}

// ── Binary search ─────────────────────────────────────────────────────────────

SearchResult SearchAlgorithms::binarySearchByCedula(const QVector<Patient> &sortedData,
                                                      const QString &cedula)
{
    SearchResult res;
    TimerUtils::ScopedTimer t;

    int low = 0, high = sortedData.size() - 1;
    while (low <= high) {
        ++res.comparisons;
        int mid = low + (high - low) / 2;
        const QString &midCedula = sortedData[mid].cedula();
        if (midCedula == cedula) {
            res.index = mid;
            break;
        } else if (midCedula < cedula) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    res.elapsedUs = t.elapsedMicroseconds();
    return res;
}

// ── Sort helper ───────────────────────────────────────────────────────────────

QVector<Patient> SearchAlgorithms::sortByCedula(QVector<Patient> data)
{
    std::sort(data.begin(), data.end(), [](const Patient &a, const Patient &b) {
        return a.cedula() < b.cedula();
    });
    return data;
}
