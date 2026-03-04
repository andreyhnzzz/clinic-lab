#pragma once

#include <QString>
#include <QStringList>
#include <QAbstractTableModel>

// Exports any QAbstractTableModel to a CSV file.
// Respects all visible columns and header labels of the model.
class CSVExporter
{
public:
    // Export the full model to filePath.
    // Returns true on success; populates errorMessage on failure.
    static bool exportModel(const QAbstractTableModel *model,
                            const QString &filePath,
                            QString &errorMessage);

    // Export a list of row data (each inner QStringList = one row).
    static bool exportData(const QStringList &headers,
                           const QList<QStringList> &rows,
                           const QString &filePath,
                           QString &errorMessage);

private:
    static QString escapeCsv(const QString &value);
};
