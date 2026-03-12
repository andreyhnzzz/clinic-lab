#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QLabel>
#include <QTabWidget>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
<<<<<<< HEAD
#include <QTextEdit>
#include <QFrame>
=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
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
<<<<<<< HEAD
    void showPatientRecord(const QString& cedula);
=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31

private:
    void setupUI();
    void showPacienteResults(const QVector<Paciente>& results);
    void showConsultaResults(const QVector<Consulta>& results);

    Module2_RecordsSearch* module_ = nullptr;

    QTabWidget* searchTabs_ = nullptr;

<<<<<<< HEAD
    // Cedula tab
=======
    // Cédula tab
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
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
<<<<<<< HEAD

    // Patient record detail panel
    QTextEdit* patientDetail_ = nullptr;
=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
};
