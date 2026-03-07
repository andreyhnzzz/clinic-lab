#pragma once
#include <QMainWindow>
#include <QTabWidget>
#include <QLabel>
#include <QStatusBar>
#include <QMenuBar>

class AttentionQueueWidget;
class RecordsSearchWidget;
class DiagnosisTreeWidget;
class PerformanceLabWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onAbout();
    void onLoadSampleData();
    void updateStatusBar();

private:
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
    void loadStyleSheet();

    QTabWidget* tabs_ = nullptr;
    AttentionQueueWidget* queueWidget_ = nullptr;
    RecordsSearchWidget* searchWidget_ = nullptr;
    DiagnosisTreeWidget* treeWidget_ = nullptr;
    PerformanceLabWidget* perfWidget_ = nullptr;

    QLabel* statusPatients_ = nullptr;
    QLabel* statusQueue_ = nullptr;
    QLabel* statusTime_ = nullptr;
};
