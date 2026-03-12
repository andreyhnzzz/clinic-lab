#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QGroupBox>
#include "../modules/Module1_AttentionQueue.h"

class AttentionQueueWidget : public QWidget {
    Q_OBJECT
public:
    explicit AttentionQueueWidget(QWidget* parent = nullptr);

    int queueSize() const;
    void addSamplePatients(int count = 10);
<<<<<<< HEAD
    Module1_AttentionQueue* module() { return module_; }
=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31

signals:
    void queueSizeChanged(int newSize);

private slots:
    void onRegisterPatient();
    void onAttendNext();
    void refreshTable();

private:
    void setupUI();
    void applyRowColor(int row, int priority);

    Module1_AttentionQueue* module_ = nullptr;

    QTableWidget* table_ = nullptr;
<<<<<<< HEAD
    QTableWidget* historyTable_ = nullptr;
=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    QPushButton* btnRegister_ = nullptr;
    QPushButton* btnAttend_ = nullptr;
    QLabel* lblAvgWait_ = nullptr;
    QProgressBar* progressWait_ = nullptr;
    QLabel* lblCritico_ = nullptr;
    QLabel* lblUrgente_ = nullptr;
    QLabel* lblNormal_ = nullptr;
    QLabel* lblTotal_ = nullptr;
<<<<<<< HEAD
    QLabel* lblNext_ = nullptr;
=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
};
