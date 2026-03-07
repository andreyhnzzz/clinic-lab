#include "DiagnosisTreeWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QFont>
#include <QHeaderView>

DiagnosisTreeWidget::DiagnosisTreeWidget(QWidget* parent)
    : QWidget(parent) {
    module_ = new Module3_DiagnosisTree(this);
    setupUI();
    populateTree();
}

void DiagnosisTreeWidget::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // Title
    auto* title = new QLabel("🌳 Árbol de Diagnósticos Médicos (ICD-10)");
    QFont f = title->font(); f.setPointSize(14); f.setBold(true);
    title->setFont(f);
    title->setObjectName("sectionTitle");
    mainLayout->addWidget(title);

    // Search bar
    auto* searchLayout = new QHBoxLayout();
    searchEdit_ = new QLineEdit();
    searchEdit_->setPlaceholderText("Buscar diagnóstico...");
    btnSearch_ = new QPushButton("🔍 Buscar");
    btnSearch_->setObjectName("primaryButton");
    searchLayout->addWidget(searchEdit_, 1);
    searchLayout->addWidget(btnSearch_);
    mainLayout->addLayout(searchLayout);

    // Buttons
    auto* btnLayout = new QHBoxLayout();
    btnPreOrder_ = new QPushButton("Pre-Order");
    btnBFS_      = new QPushButton("BFS (Nivel)");
    btnExpand_   = new QPushButton("Expandir Todo");
    btnCollapse_ = new QPushButton("Colapsar Todo");
    for (auto* b : {btnPreOrder_, btnBFS_, btnExpand_, btnCollapse_}) {
        b->setObjectName("primaryButton");
        btnLayout->addWidget(b);
    }
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    // Splitter: tree | detail
    auto* splitter = new QSplitter(Qt::Horizontal, this);

    treeWidget_ = new QTreeWidget();
    treeWidget_->setHeaderLabels({"Nombre", "Código"});
    treeWidget_->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    treeWidget_->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    treeWidget_->setAnimated(true);
    treeWidget_->setAlternatingRowColors(true);

    auto* detailBox = new QGroupBox("Detalle del Diagnóstico");
    auto* detailLayout = new QVBoxLayout(detailBox);
    detailPanel_ = new QTextEdit();
    detailPanel_->setReadOnly(true);
    detailPanel_->setPlaceholderText("Seleccione un diagnóstico para ver sus detalles...");
    detailLayout->addWidget(detailPanel_);

    splitter->addWidget(treeWidget_);
    splitter->addWidget(detailBox);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);
    mainLayout->addWidget(splitter, 1);

    // Node count
    lblNodeCount_ = new QLabel();
    lblNodeCount_->setObjectName("statLabel");
    mainLayout->addWidget(lblNodeCount_);

    // Connections
    connect(btnSearch_,   &QPushButton::clicked, this, &DiagnosisTreeWidget::onSearch);
    connect(searchEdit_,  &QLineEdit::returnPressed, this, &DiagnosisTreeWidget::onSearch);
    connect(btnPreOrder_, &QPushButton::clicked, this, &DiagnosisTreeWidget::onPreOrder);
    connect(btnBFS_,      &QPushButton::clicked, this, &DiagnosisTreeWidget::onBFS);
    connect(btnExpand_,   &QPushButton::clicked, this, &DiagnosisTreeWidget::onExpandAll);
    connect(btnCollapse_, &QPushButton::clicked, this, &DiagnosisTreeWidget::onCollapseAll);
    connect(treeWidget_,  &QTreeWidget::itemClicked,
            this, &DiagnosisTreeWidget::onItemSelected);
}

void DiagnosisTreeWidget::populateTree() {
    treeWidget_->clear();
    DiagnosisNode* root = module_->getTree().root();
    if (!root) return;

    for (DiagnosisNode* area : root->children) {
        auto* areaItem = new QTreeWidgetItem(treeWidget_);
        areaItem->setText(0, area->name);
        areaItem->setIcon(0, QIcon());
        QFont bold = areaItem->font(0);
        bold.setBold(true);
        bold.setPointSize(bold.pointSize() + 1);
        areaItem->setFont(0, bold);
        areaItem->setForeground(0, QColor("#00BCD4"));

        for (DiagnosisNode* spec : area->children) {
            auto* specItem = new QTreeWidgetItem(areaItem);
            specItem->setText(0, spec->name);
            QFont italic = specItem->font(0);
            italic.setItalic(true);
            specItem->setFont(0, italic);
            specItem->setForeground(0, QColor("#B0BEC5"));

            for (DiagnosisNode* diag : spec->children) {
                auto* diagItem = new QTreeWidgetItem(specItem);
                diagItem->setText(0, diag->name);
                diagItem->setText(1, diag->code);
                // Store data for detail panel
                diagItem->setData(0, Qt::UserRole, diag->description);
                diagItem->setData(1, Qt::UserRole, area->name);
                diagItem->setData(2, Qt::UserRole, spec->name);
            }
        }
    }

    treeWidget_->expandToDepth(0); // Expand area level only
    lblNodeCount_->setText(QString("Nodos totales: %1 | Diagnósticos: %2")
        .arg(module_->totalNodes())
        .arg(module_->totalDiagnoses()));
}

void DiagnosisTreeWidget::onSearch() {
    QString query = searchEdit_->text().trimmed();
    if (query.isEmpty()) {
        treeWidget_->clearSelection();
        // Restore original highlighting
        for (int i = 0; i < treeWidget_->topLevelItemCount(); ++i)
            treeWidget_->topLevelItem(i)->setHidden(false);
        return;
    }

    auto results = module_->searchByName(query);
    showTraversalResults(results, QString("Búsqueda: \"%1\"").arg(query));

    // Highlight matching items in tree
    QString lower = query.toLower();
    for (int i = 0; i < treeWidget_->topLevelItemCount(); ++i) {
        auto* area = treeWidget_->topLevelItem(i);
        for (int j = 0; j < area->childCount(); ++j) {
            auto* spec = area->child(j);
            for (int k = 0; k < spec->childCount(); ++k) {
                auto* diag = spec->child(k);
                bool match = diag->text(0).toLower().contains(lower);
                diag->setBackground(0, match ? QColor("#1B3A4B") : QBrush());
                if (match) {
                    treeWidget_->scrollToItem(diag);
                    area->setExpanded(true);
                    spec->setExpanded(true);
                }
            }
        }
    }
}

void DiagnosisTreeWidget::showTraversalResults(const QVector<Diagnostico>& results, const QString& title) {
    QString text;
    text += QString("<h3>%1</h3>").arg(title);
    text += QString("<p>Total: <b>%1</b> diagnósticos</p><hr/>").arg(results.size());
    int n = qMin(results.size(), 50);
    for (int i = 0; i < n; ++i) {
        const auto& d = results[i];
        text += QString("<p><b>[%1]</b> %2<br/><small>%3 → %4</small></p>")
            .arg(QString::fromStdString(d.codigo))
            .arg(QString::fromStdString(d.nombre))
            .arg(QString::fromStdString(d.categoria))
            .arg(QString::fromStdString(d.subcategoria));
    }
    if (results.size() > 50)
        text += QString("<p><i>... y %1 más</i></p>").arg(results.size() - 50);
    detailPanel_->setHtml(text);
}

void DiagnosisTreeWidget::onPreOrder() {
    auto results = module_->preOrderTraversal();
    showTraversalResults(results, "Recorrido Pre-Order");
}

void DiagnosisTreeWidget::onBFS() {
    auto results = module_->bfsTraversal();
    showTraversalResults(results, "Recorrido BFS (Por Niveles)");
}

void DiagnosisTreeWidget::onExpandAll() {
    treeWidget_->expandAll();
}

void DiagnosisTreeWidget::onCollapseAll() {
    treeWidget_->collapseAll();
}

void DiagnosisTreeWidget::onItemSelected(QTreeWidgetItem* item, int /*column*/) {
    if (!item || item->childCount() > 0) return; // Only leaf (diagnosis) nodes
    QString desc = item->data(0, Qt::UserRole).toString();
    QString area = item->data(1, Qt::UserRole).toString();
    QString spec = item->data(2, Qt::UserRole).toString();

    QString html = QString(
        "<h3>%1</h3>"
        "<table>"
        "<tr><td><b>Código:</b></td><td>%2</td></tr>"
        "<tr><td><b>Área:</b></td><td>%3</td></tr>"
        "<tr><td><b>Especialidad:</b></td><td>%4</td></tr>"
        "<tr><td><b>Descripción:</b></td><td>%5</td></tr>"
        "</table>")
        .arg(item->text(0))
        .arg(item->text(1))
        .arg(area)
        .arg(spec)
        .arg(desc.isEmpty() ? "<i>Sin descripción disponible</i>" : desc);

    detailPanel_->setHtml(html);
}
