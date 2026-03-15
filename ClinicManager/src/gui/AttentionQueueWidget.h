#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QGroupBox>
#include <QTimer>
#include "../modules/Module1_AttentionQueue.h"

class AttentionQueueWidget : public QWidget {
    Q_OBJECT
public:
    explicit AttentionQueueWidget(QWidget* parent = nullptr);

    int queueSize() const;
    void addSamplePatients(int count = 10);
    Module1_AttentionQueue* module() { return module_; }

signals:
    void queueSizeChanged(int newSize);
    void nextPatientChanged(const QString& info);

private slots:
    void onRegisterPatient();
    void onAttendNext();
    void refreshTable();
    void refreshWaitMetrics();

private:
    void setupUI();
    void applyRowColor(int row, int priority);

    Module1_AttentionQueue* module_ = nullptr;
    QTimer* waitTimer_ = nullptr;

    QTableWidget* table_ = nullptr;
    QTableWidget* historyTable_ = nullptr;
    QPushButton* btnRegister_ = nullptr;
    QPushButton* btnAttend_ = nullptr;
    QLabel* lblAvgWait_ = nullptr;
    QProgressBar* progressWait_ = nullptr;
    QLabel* lblCritico_ = nullptr;
    QLabel* lblUrgente_ = nullptr;
    QLabel* lblNormal_ = nullptr;
    QLabel* lblTotal_ = nullptr;
    QLabel* lblNext_ = nullptr;
};
