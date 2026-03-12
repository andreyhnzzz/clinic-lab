#pragma once
#include <QMainWindow>
<<<<<<< HEAD
#include <QStackedWidget>
#include <QLabel>
#include <QStatusBar>
#include <QMenuBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFrame>
=======
#include <QTabWidget>
#include <QLabel>
#include <QStatusBar>
#include <QMenuBar>
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31

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
<<<<<<< HEAD
    void switchModule(int index);
=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31

private:
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
<<<<<<< HEAD

    QStackedWidget* stack_ = nullptr;
=======
    void loadStyleSheet();

    QTabWidget* tabs_ = nullptr;
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    AttentionQueueWidget* queueWidget_ = nullptr;
    RecordsSearchWidget* searchWidget_ = nullptr;
    DiagnosisTreeWidget* treeWidget_ = nullptr;
    PerformanceLabWidget* perfWidget_ = nullptr;

<<<<<<< HEAD
    QVector<QPushButton*> navButtons_;
    QLabel* statusPatients_ = nullptr;
    QLabel* statusQueue_ = nullptr;
    QLabel* statusTime_ = nullptr;
    QLabel* headerTitle_ = nullptr;
=======
    QLabel* statusPatients_ = nullptr;
    QLabel* statusQueue_ = nullptr;
    QLabel* statusTime_ = nullptr;
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
};
