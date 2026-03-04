#include "CSVExporter.h"

#include <QFile>
#include <QTextStream>
#include <QAbstractTableModel>

// ── Public API ───────────────────────────────────────────────────────────────

bool CSVExporter::exportModel(const QAbstractTableModel *model,
                              const QString &filePath,
                              QString &errorMessage)
{
    if (!model) {
        errorMessage = "Modelo nulo";
        return false;
    }

    // Build header row
    QStringList headers;
    for (int col = 0; col < model->columnCount(); ++col)
        headers << model->headerData(col, Qt::Horizontal).toString();

    // Build data rows
    QList<QStringList> rows;
    for (int row = 0; row < model->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 0; col < model->columnCount(); ++col)
            rowData << model->data(model->index(row, col)).toString();
        rows << rowData;
    }

    return exportData(headers, rows, filePath, errorMessage);
}

bool CSVExporter::exportData(const QStringList &headers,
                             const QList<QStringList> &rows,
                             const QString &filePath,
                             QString &errorMessage)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        errorMessage = QString("No se pudo abrir el archivo: %1").arg(file.errorString());
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    // Header
    QStringList escapedHeaders;
    for (const QString &h : headers)
        escapedHeaders << escapeCsv(h);
    out << escapedHeaders.join(',') << "\n";

    // Rows
    for (const QStringList &row : rows) {
        QStringList escapedRow;
        for (const QString &cell : row)
            escapedRow << escapeCsv(cell);
        out << escapedRow.join(',') << "\n";
    }

    file.close();
    return true;
}

// ── Private helpers ──────────────────────────────────────────────────────────

QString CSVExporter::escapeCsv(const QString &value)
{
    // RFC 4180: wrap in quotes if value contains comma, quote or newline
    if (value.contains(',') || value.contains('"') || value.contains('\n')) {
        QString escaped = value;
        escaped.replace('"', "\"\"");
        return '"' + escaped + '"';
    }
    return value;
}
