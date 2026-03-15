#pragma once
#include <QWidget>
#include <QTreeWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QComboBox>
#include "../modules/Module3_DiagnosisTree.h"

class DiagnosisTreeWidget : public QWidget {
    Q_OBJECT
public:
    explicit DiagnosisTreeWidget(QWidget* parent = nullptr);

private slots:
    void onSearchByName();
    void onSearchByCode();
    void onFilterBySpecialty();
    void onPreOrder();
    void onBFS();
    void onPostOrder();
    void onFullTraversal();
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
    QLineEdit* codeEdit_ = nullptr;
    QComboBox* cbSpecialty_ = nullptr;
    QPushButton* btnSearchName_ = nullptr;
    QPushButton* btnSearchCode_ = nullptr;
    QPushButton* btnFilterSpec_ = nullptr;
    QPushButton* btnPreOrder_ = nullptr;
    QPushButton* btnBFS_ = nullptr;
    QPushButton* btnPostOrder_ = nullptr;
    QPushButton* btnFullTraversal_ = nullptr;
    QPushButton* btnExpand_ = nullptr;
    QPushButton* btnCollapse_ = nullptr;
    QTextEdit* detailPanel_ = nullptr;
    QLabel* lblNodeCount_ = nullptr;
};
