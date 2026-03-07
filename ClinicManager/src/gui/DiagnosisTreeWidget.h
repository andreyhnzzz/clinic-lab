#pragma once
#include <QWidget>
#include <QTreeWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include "../modules/Module3_DiagnosisTree.h"

class DiagnosisTreeWidget : public QWidget {
    Q_OBJECT
public:
    explicit DiagnosisTreeWidget(QWidget* parent = nullptr);

private slots:
    void onSearch();
    void onPreOrder();
    void onBFS();
    void onExpandAll();
    void onCollapseAll();
    void onItemSelected(QTreeWidgetItem* item, int column);

private:
    void setupUI();
    void populateTree();
    void showTraversalResults(const QVector<Diagnostico>& results, const QString& title);

    Module3_DiagnosisTree* module_ = nullptr;

    QTreeWidget* treeWidget_ = nullptr;
    QLineEdit* searchEdit_ = nullptr;
    QPushButton* btnSearch_ = nullptr;
    QPushButton* btnPreOrder_ = nullptr;
    QPushButton* btnBFS_ = nullptr;
    QPushButton* btnExpand_ = nullptr;
    QPushButton* btnCollapse_ = nullptr;
    QTextEdit* detailPanel_ = nullptr;
    QLabel* lblNodeCount_ = nullptr;
};
