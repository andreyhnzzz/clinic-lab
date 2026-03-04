#include "DiagnosisTreeWidget.h"

#include <QApplication>
#include <QStyle>
#include <QHeaderView>

DiagnosisTreeWidget::DiagnosisTreeWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void DiagnosisTreeWidget::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(6);

    // ── Toolbar ───────────────────────────────────────────────────────────────
    auto *toolBar = new QHBoxLayout;

    toolBar->addWidget(new QLabel("Recorrido:"));
    m_traversalCombo = new QComboBox(this);
    m_traversalCombo->addItem("Pre-Order");
    m_traversalCombo->addItem("Level-Order (BFS)");
    toolBar->addWidget(m_traversalCombo);

    toolBar->addSpacing(20);
    toolBar->addWidget(new QLabel("Buscar:"));
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Código o nombre de diagnóstico...");
    toolBar->addWidget(m_searchEdit, 1);

    m_searchBtn = new QPushButton("Buscar", this);
    toolBar->addWidget(m_searchBtn);

    m_searchResult = new QLabel("", this);
    toolBar->addWidget(m_searchResult);

    mainLayout->addLayout(toolBar);

    // ── Splitter: tree + detail ───────────────────────────────────────────────
    auto *splitter = new QSplitter(Qt::Horizontal, this);

    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setHeaderLabel("Árbol Diagnóstico (Área → Especialidad → Diagnóstico)");
    m_treeWidget->setIconSize(QSize(20, 20));
    m_treeWidget->setAlternatingRowColors(true);
    splitter->addWidget(m_treeWidget);

    // Detail panel
    m_detailGroup = new QGroupBox("Detalle del Nodo", this);
    auto *detailLayout = new QVBoxLayout(m_detailGroup);
    m_detailLabel = new QLabel("Seleccione un nodo para ver detalles.", this);
    m_detailLabel->setWordWrap(true);
    m_detailLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    detailLayout->addWidget(m_detailLabel);
    detailLayout->addStretch();
    splitter->addWidget(m_detailGroup);

    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);

    mainLayout->addWidget(splitter, 1);

    // ── Traversal result label ────────────────────────────────────────────────
    m_traversalLabel = new QLabel("", this);
    m_traversalLabel->setWordWrap(true);
    mainLayout->addWidget(m_traversalLabel);

    // ── Connections ───────────────────────────────────────────────────────────
    connect(m_searchBtn, &QPushButton::clicked, this, &DiagnosisTreeWidget::onSearch);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &DiagnosisTreeWidget::onSearch);
    connect(m_traversalCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DiagnosisTreeWidget::onTraversalChanged);
    connect(m_treeWidget, &QTreeWidget::itemClicked,
            this, &DiagnosisTreeWidget::onItemSelected);
}

// ── Public API ────────────────────────────────────────────────────────────────

void DiagnosisTreeWidget::setTree(std::shared_ptr<DiagnosticoNode> root)
{
    m_tree.setRoot(root);
    populateTree(root);
    onTraversalChanged(m_traversalCombo->currentIndex());
}

// ── Tree population ───────────────────────────────────────────────────────────

void DiagnosisTreeWidget::populateTree(const std::shared_ptr<DiagnosticoNode> &root)
{
    m_treeWidget->clear();
    if (!root) return;

    // Skip the virtual ROOT node; iterate its children (areas)
    for (const auto &area : root->children()) {
        auto *areaItem = new QTreeWidgetItem(m_treeWidget);
        areaItem->setText(0, area->nombre());
        areaItem->setIcon(0, iconForLevel(area->level()));
        areaItem->setData(0, Qt::UserRole, QVariant::fromValue<void*>(area.get()));
        addTreeItem(areaItem, area);
    }

    m_treeWidget->expandToDepth(0);
}

void DiagnosisTreeWidget::addTreeItem(QTreeWidgetItem *parent,
                                       const std::shared_ptr<DiagnosticoNode> &node)
{
    for (const auto &child : node->children()) {
        auto *item = new QTreeWidgetItem(parent);
        item->setText(0, child->nombre());
        item->setIcon(0, iconForLevel(child->level()));
        item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(child.get()));
        if (child->hasChildren())
            addTreeItem(item, child);
    }
}

QIcon DiagnosisTreeWidget::iconForLevel(DiagnosticoNode::Level level)
{
    switch (level) {
    case DiagnosticoNode::Level::Area:
        return QApplication::style()->standardIcon(QStyle::SP_DirIcon);
    case DiagnosticoNode::Level::Specialty:
        return QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView);
    case DiagnosticoNode::Level::Diagnosis:
        return QApplication::style()->standardIcon(QStyle::SP_FileIcon);
    }
    return {};
}

// ── Slots ─────────────────────────────────────────────────────────────────────

void DiagnosisTreeWidget::onSearch()
{
    QString query = m_searchEdit->text().trimmed();
    if (query.isEmpty()) { m_searchResult->setText(""); return; }

    DiagnosticoNode *found = m_tree.findByName(query);
    if (found) {
        m_searchResult->setText(QString("✔ Encontrado: %1 (%2)")
                                .arg(found->nombre(), found->codigo()));
        showNodeDetails(found);
        // Try to select in tree widget
        QTreeWidgetItemIterator it(m_treeWidget);
        while (*it) {
            void *ptr = (*it)->data(0, Qt::UserRole).value<void*>();
            if (ptr == found) {
                m_treeWidget->setCurrentItem(*it);
                m_treeWidget->scrollToItem(*it);
                break;
            }
            ++it;
        }
    } else {
        m_searchResult->setText("✘ No encontrado: " + query);
    }
}

void DiagnosisTreeWidget::onTraversalChanged(int index)
{
    if (m_tree.isEmpty()) return;

    QVector<DiagnosticoNode*> nodes = (index == 0)
        ? m_tree.preOrder()
        : m_tree.levelOrder();

    // Show first 20 nodes as a preview
    QStringList preview;
    int shown = qMin(20, nodes.size());
    for (int i = 0; i < shown; ++i)
        preview << nodes[i]->nombre();

    m_traversalLabel->setText(
        QString("Recorrido %1 (%2 nodos): %3%4")
        .arg(index == 0 ? "Pre-Order" : "Level-Order")
        .arg(nodes.size())
        .arg(preview.join(" → "))
        .arg(nodes.size() > 20 ? " → ..." : "")
    );
}

void DiagnosisTreeWidget::onItemSelected(QTreeWidgetItem *item)
{
    if (!item) return;
    void *ptr = item->data(0, Qt::UserRole).value<void*>();
    if (ptr) showNodeDetails(static_cast<DiagnosticoNode*>(ptr));
}

void DiagnosisTreeWidget::showNodeDetails(DiagnosticoNode *node)
{
    QString levelStr;
    switch (node->level()) {
    case DiagnosticoNode::Level::Area:      levelStr = "Área"; break;
    case DiagnosticoNode::Level::Specialty: levelStr = "Especialidad"; break;
    case DiagnosticoNode::Level::Diagnosis: levelStr = "Diagnóstico"; break;
    }

    m_detailLabel->setText(
        QString("<b>Código:</b> %1<br>"
                "<b>Nombre:</b> %2<br>"
                "<b>Nivel:</b> %3<br>"
                "<b>Categoría:</b> %4<br>"
                "<b>Subcategoría:</b> %5<br>"
                "<b>Descripción:</b> %6<br>"
                "<b>Hijos:</b> %7")
        .arg(node->codigo(), node->nombre(), levelStr,
             node->categoria(), node->subcategoria(),
             node->descripcion())
        .arg(node->children().size())
    );
}
