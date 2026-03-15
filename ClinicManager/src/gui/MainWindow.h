#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QLabel>
#include <QStatusBar>
#include <QMenuBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFrame>

class DashboardWidget;
class AttentionQueueWidget;
class RecordsSearchWidget;
class DiagnosisTreeWidget;
class PerformanceLabWidget;
class AuditWidget;
class EventLogWidget;
class JsonPersistence;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onAbout();
    void onLoadSampleData();
    void onSaveData();
    void onLoadData();
    void updateStatusBar();
    void switchModule(int index);

private:
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();

    QStackedWidget* stack_ = nullptr;
    DashboardWidget* dashWidget_ = nullptr;
    AttentionQueueWidget* queueWidget_ = nullptr;
    RecordsSearchWidget* searchWidget_ = nullptr;
    DiagnosisTreeWidget* treeWidget_ = nullptr;
    PerformanceLabWidget* perfWidget_ = nullptr;
    AuditWidget* auditWidget_ = nullptr;
    EventLogWidget* logWidget_ = nullptr;
    JsonPersistence* persistence_ = nullptr;

    QVector<QPushButton*> navButtons_;
    QLabel* statusPatients_ = nullptr;
    QLabel* statusQueue_ = nullptr;
    QLabel* statusTime_ = nullptr;
    QLabel* headerTitle_ = nullptr;
};
