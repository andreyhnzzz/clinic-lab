#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QLabel>
#include <QTabWidget>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QFrame>
#include "../modules/Module2_RecordsSearch.h"

class RecordsSearchWidget : public QWidget {
    Q_OBJECT
public:
    explicit RecordsSearchWidget(QWidget* parent = nullptr);

    void loadSampleData(int count);
    int pacienteCount() const;

private slots:
    void onSearchByCedula();
    void onSearchByName();
    void onSearchByDate();
    void onSearchByGravedad();
    void showPatientRecord(const QString& cedula);

private:
    void setupUI();
    void showPacienteResults(const QVector<Paciente>& results);
    void showConsultaResults(const QVector<Consulta>& results);

    Module2_RecordsSearch* module_ = nullptr;

    QTabWidget* searchTabs_ = nullptr;

    // Cedula tab
    QLineEdit* edCedula_ = nullptr;
    QPushButton* btnCedula_ = nullptr;

    // Name tab
    QLineEdit* edName_ = nullptr;
    QPushButton* btnName_ = nullptr;

    // Date tab
    QDateEdit* dateFrom_ = nullptr;
    QDateEdit* dateTo_ = nullptr;
    QPushButton* btnDate_ = nullptr;

    // Gravedad tab
    QComboBox* cbGravedad_ = nullptr;
    QPushButton* btnGravedad_ = nullptr;

    QTableWidget* resultsTable_ = nullptr;
    QLabel* lblTime_ = nullptr;
    QLabel* lblCount_ = nullptr;
    QLabel* lblLinear_ = nullptr;
    QLabel* lblBinary_ = nullptr;

    // Patient record detail panel
    QTextEdit* patientDetail_ = nullptr;
};
