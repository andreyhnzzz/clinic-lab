#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>

class EventLogWidget : public QWidget {
    Q_OBJECT
public:
    explicit EventLogWidget(QWidget* parent = nullptr);

private slots:
    void onNewEntry();
    void onFilterChanged();
    void refreshTable();

private:
    void setupUI();

    QTableWidget* table_ = nullptr;
    QComboBox* cbFilter_ = nullptr;
    QPushButton* btnClear_ = nullptr;
    QLabel* lblCount_ = nullptr;
};
