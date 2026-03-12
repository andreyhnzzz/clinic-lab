#pragma once
#include <QWidget>
#include <QTreeWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
<<<<<<< HEAD
#include <QComboBox>
=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
#include "../modules/Module3_DiagnosisTree.h"

class DiagnosisTreeWidget : public QWidget {
    Q_OBJECT
public:
    explicit DiagnosisTreeWidget(QWidget* parent = nullptr);

private slots:
<<<<<<< HEAD
    void onSearchByName();
    void onSearchByCode();
    void onFilterBySpecialty();
=======
    void onSearch();
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
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
<<<<<<< HEAD
    QLineEdit* codeEdit_ = nullptr;
    QComboBox* cbSpecialty_ = nullptr;
    QPushButton* btnSearchName_ = nullptr;
    QPushButton* btnSearchCode_ = nullptr;
    QPushButton* btnFilterSpec_ = nullptr;
=======
    QPushButton* btnSearch_ = nullptr;
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    QPushButton* btnPreOrder_ = nullptr;
    QPushButton* btnBFS_ = nullptr;
    QPushButton* btnExpand_ = nullptr;
    QPushButton* btnCollapse_ = nullptr;
    QTextEdit* detailPanel_ = nullptr;
    QLabel* lblNodeCount_ = nullptr;
};
