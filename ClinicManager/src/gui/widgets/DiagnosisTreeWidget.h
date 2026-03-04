#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSplitter>
#include <memory>

#include "Diagnostico.h"
#include "DiagnosisTree.h"

// Displays the N-ary diagnosis tree with icons per level.
// Supports pre-order and level-order traversal visualization.
class DiagnosisTreeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DiagnosisTreeWidget(QWidget *parent = nullptr);

    void setTree(std::shared_ptr<DiagnosticoNode> root);

private slots:
    void onSearch();
    void onTraversalChanged(int index);
    void onItemSelected(QTreeWidgetItem *item);

private:
    void setupUi();
    void populateTree(const std::shared_ptr<DiagnosticoNode> &root);
    void addTreeItem(QTreeWidgetItem *parent,
                     const std::shared_ptr<DiagnosticoNode> &node);
    QIcon iconForLevel(DiagnosticoNode::Level level);
    void showNodeDetails(DiagnosticoNode *node);

    DiagnosisTree  m_tree;

    QTreeWidget   *m_treeWidget   = nullptr;
    QLineEdit     *m_searchEdit   = nullptr;
    QPushButton   *m_searchBtn    = nullptr;
    QLabel        *m_searchResult = nullptr;
    QComboBox     *m_traversalCombo = nullptr;
    QLabel        *m_traversalLabel = nullptr;
    QLabel        *m_detailLabel    = nullptr;
    QGroupBox     *m_detailGroup    = nullptr;
};
