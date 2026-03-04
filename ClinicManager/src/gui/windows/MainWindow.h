#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
#include <QLabel>
#include <QSplitter>
#include <QStatusBar>
#include <memory>

#include "DataGenerator.h"

class PatientTableWidget;
class DiagnosisTreeWidget;
class PerformanceLabWindow;

// Main application window.
// Left panel: navigation menu (module list).
// Right panel: QStackedWidget that swaps between module views.
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onModuleSelected(int row);
    void onGenerateData();
    void onGenerateDataSize(DataGenerator::DatasetSize size);
    void onDataProgress(int percent);

private:
    void setupUi();
    void setupMenu();
    void applyTheme();
    void updateStatusBar(const QString &msg);

    // Central widgets
    QSplitter       *m_splitter   = nullptr;
    QListWidget     *m_navList    = nullptr;
    QStackedWidget  *m_stack      = nullptr;

    // Module pages (indices in m_stack)
    PatientTableWidget    *m_patientPage   = nullptr;  // Module 1 & 2
    DiagnosisTreeWidget   *m_diagPage      = nullptr;  // Module 3
    PerformanceLabWindow  *m_perfPage      = nullptr;  // Module 4

    // Data
    std::unique_ptr<DataGenerator> m_generator;
    QVector<Patient>  m_patients;
    QVector<Consulta> m_consultas;
    std::shared_ptr<DiagnosticoNode> m_diagTree;

    QLabel *m_statusLabel = nullptr;
};
