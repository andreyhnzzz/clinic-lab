#pragma once

#include <QWidget>
#include <QVector>
#include <QTableView>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

#include "Patient.h"
#include "PriorityQueue.h"
#include "SearchAlgorithms.h"
#include "SearchBenchmark.h"

// Model for QTableView – uses QAbstractTableModel for efficiency with large datasets
class PatientTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column { ColCedula=0, ColNombre, ColEdad, ColFechaReg,
                  ColPrioridad, ColSangre, ColDiag, ColCount };

    explicit PatientTableModel(QObject *parent = nullptr);

    void setPatients(const QVector<Patient> &patients);
    const QVector<Patient> &patients() const { return m_data; }

    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QVector<Patient> m_data;
};

// ── Main widget ───────────────────────────────────────────────────────────────

class PatientTableWidget : public QWidget
{
    Q_OBJECT

public:
    enum class Mode { PriorityQueue, Search };

    explicit PatientTableWidget(QWidget *parent = nullptr);

    void setPatients(const QVector<Patient> &patients);
    void setMode(Mode mode);

private slots:
    void onSearch();
    void onEnqueue();
    void onDequeue();

private:
    void setupUi();
    void setupPriorityPanel();
    void setupSearchPanel();
    void showPriorityPanel(bool show);
    void showSearchPanel(bool show);

    // Table
    PatientTableModel *m_model    = nullptr;
    QTableView        *m_tableView = nullptr;

    // Search panel
    QGroupBox  *m_searchGroup     = nullptr;
    QLineEdit  *m_searchEdit      = nullptr;
    QComboBox  *m_searchTypeCombo = nullptr;
    QPushButton *m_searchBtn      = nullptr;
    QLabel     *m_searchResult    = nullptr;

    // Priority queue panel
    QGroupBox  *m_queueGroup      = nullptr;
    QPushButton *m_enqueueBtn     = nullptr;
    QPushButton *m_dequeueBtn     = nullptr;
    QLabel     *m_queueStatus     = nullptr;
    QTableView *m_queueView       = nullptr;
    PatientTableModel *m_queueModel = nullptr;

    PriorityQueue<Patient> m_priorityQueue;
    SearchBenchmark        m_searchBench;
    Mode                   m_mode = Mode::PriorityQueue;
};
