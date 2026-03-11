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
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(12, 12, 12, 12);

    // Search by name row
    auto* searchLayout = new QHBoxLayout();
    searchEdit_ = new QLineEdit();
    searchEdit_->setPlaceholderText("Buscar diagnostico por nombre...");
    btnSearchName_ = new QPushButton("Buscar Nombre");
    btnSearchName_->setObjectName("primaryButton");
    searchLayout->addWidget(new QLabel("Nombre:"));
    searchLayout->addWidget(searchEdit_, 1);
    searchLayout->addWidget(btnSearchName_);
    mainLayout->addLayout(searchLayout);

    // Search by code row
    auto* codeLayout = new QHBoxLayout();
    codeEdit_ = new QLineEdit();
    codeEdit_->setPlaceholderText("Buscar por codigo ICD-10 (ej: I21, K35)...");
    btnSearchCode_ = new QPushButton("Buscar Codigo");
    btnSearchCode_->setObjectName("primaryButton");
    codeLayout->addWidget(new QLabel("Codigo:"));
    codeLayout->addWidget(codeEdit_, 1);
    codeLayout->addWidget(btnSearchCode_);
    mainLayout->addLayout(codeLayout);

    // Filter by specialty row
    auto* specLayout = new QHBoxLayout();
    cbSpecialty_ = new QComboBox();
    cbSpecialty_->addItem("-- Todas las especialidades --");
    QStringList specs = module_->allSpecialties();
    cbSpecialty_->addItems(specs);
    btnFilterSpec_ = new QPushButton("Listar por Especialidad");
    btnFilterSpec_->setObjectName("primaryButton");
    specLayout->addWidget(new QLabel("Especialidad:"));
    specLayout->addWidget(cbSpecialty_, 1);
    specLayout->addWidget(btnFilterSpec_);
    mainLayout->addLayout(specLayout);

    // Buttons
    auto* btnLayout = new QHBoxLayout();
    btnPreOrder_ = new QPushButton("Pre-Order");
    btnBFS_      = new QPushButton("BFS (Nivel)");
    btnExpand_   = new QPushButton("Expandir Todo");
    btnCollapse_ = new QPushButton("Colapsar Todo");
    for (auto* b : {btnPreOrder_, btnBFS_, btnExpand_, btnCollapse_})
        btnLayout->addWidget(b);
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    // Splitter: tree | detail
    auto* splitter = new QSplitter(Qt::Horizontal, this);

    treeWidget_ = new QTreeWidget();
    treeWidget_->setHeaderLabels({"Nombre", "Codigo"});
    treeWidget_->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    treeWidget_->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    treeWidget_->setAnimated(true);

    detailPanel_ = new QTextEdit();
    detailPanel_->setReadOnly(true);
    detailPanel_->setPlaceholderText("Seleccione un elemento para ver detalles...");

    splitter->addWidget(treeWidget_);
    splitter->addWidget(detailPanel_);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);
    mainLayout->addWidget(splitter, 1);

    // Node count
    lblNodeCount_ = new QLabel();
    mainLayout->addWidget(lblNodeCount_);

    // Connections
    connect(btnSearchName_,  &QPushButton::clicked, this, &DiagnosisTreeWidget::onSearchByName);
    connect(searchEdit_,     &QLineEdit::returnPressed, this, &DiagnosisTreeWidget::onSearchByName);
    connect(btnSearchCode_,  &QPushButton::clicked, this, &DiagnosisTreeWidget::onSearchByCode);
    connect(codeEdit_,       &QLineEdit::returnPressed, this, &DiagnosisTreeWidget::onSearchByCode);
    connect(btnFilterSpec_,  &QPushButton::clicked, this, &DiagnosisTreeWidget::onFilterBySpecialty);
    connect(btnPreOrder_,    &QPushButton::clicked, this, &DiagnosisTreeWidget::onPreOrder);
    connect(btnBFS_,         &QPushButton::clicked, this, &DiagnosisTreeWidget::onBFS);
    connect(btnExpand_,      &QPushButton::clicked, this, &DiagnosisTreeWidget::onExpandAll);
    connect(btnCollapse_,    &QPushButton::clicked, this, &DiagnosisTreeWidget::onCollapseAll);
    connect(treeWidget_,     &QTreeWidget::itemClicked,
            this, &DiagnosisTreeWidget::onItemSelected);
}

void DiagnosisTreeWidget::populateTree() {
    treeWidget_->clear();
    DiagnosisNode* root = module_->getTree().root();
    if (!root) return;

    for (DiagnosisNode* area : root->children) {
        auto* areaItem = new QTreeWidgetItem(treeWidget_);
        areaItem->setText(0, area->name);
        QFont bold = areaItem->font(0);
        bold.setBold(true);
        areaItem->setFont(0, bold);
        areaItem->setForeground(0, QColor("#4FD1C5"));

        for (DiagnosisNode* spec : area->children) {
            auto* specItem = new QTreeWidgetItem(areaItem);
            specItem->setText(0, spec->name);
            QFont italic = specItem->font(0);
            italic.setItalic(true);
            specItem->setFont(0, italic);
            specItem->setForeground(0, QColor("#A7B1BC"));

            for (DiagnosisNode* diag : spec->children) {
                auto* diagItem = new QTreeWidgetItem(specItem);
                diagItem->setText(0, diag->name);
                diagItem->setText(1, diag->code);
                diagItem->setData(0, Qt::UserRole, diag->description);
                diagItem->setData(1, Qt::UserRole, area->name);
                diagItem->setData(2, Qt::UserRole, spec->name);
            }
        }
    }

    treeWidget_->expandToDepth(0);
    lblNodeCount_->setText(QString("Nodos totales: %1 | Diagnosticos: %2")
        .arg(module_->totalNodes())
        .arg(module_->totalDiagnoses()));
}

void DiagnosisTreeWidget::onSearchByName() {
    QString query = searchEdit_->text().trimmed();
    if (query.isEmpty()) return;
    auto results = module_->searchByName(query);
    showTraversalResults(results, QString("Busqueda por nombre: \"%1\"").arg(query));

    // Highlight matching items in tree
    QString lower = query.toLower();
    for (int i = 0; i < treeWidget_->topLevelItemCount(); ++i) {
        auto* area = treeWidget_->topLevelItem(i);
        for (int j = 0; j < area->childCount(); ++j) {
            auto* spec = area->child(j);
            for (int k = 0; k < spec->childCount(); ++k) {
                auto* diag = spec->child(k);
                bool match = diag->text(0).toLower().contains(lower);
                diag->setBackground(0, match ? QColor("#1E3A5F") : QBrush());
                if (match) {
                    treeWidget_->scrollToItem(diag);
                    area->setExpanded(true);
                    spec->setExpanded(true);
                }
            }
        }
    }
}

void DiagnosisTreeWidget::onSearchByCode() {
    QString code = codeEdit_->text().trimmed();
    if (code.isEmpty()) return;
    auto results = module_->searchByCode(code);
    showTraversalResults(results, QString("Busqueda por codigo: \"%1\"").arg(code));

    // Highlight matching items
    QString lower = code.toLower();
    for (int i = 0; i < treeWidget_->topLevelItemCount(); ++i) {
        auto* area = treeWidget_->topLevelItem(i);
        for (int j = 0; j < area->childCount(); ++j) {
            auto* spec = area->child(j);
            for (int k = 0; k < spec->childCount(); ++k) {
                auto* diag = spec->child(k);
                bool match = diag->text(1).toLower().contains(lower);
                diag->setBackground(0, match ? QColor("#1E3A5F") : QBrush());
                diag->setBackground(1, match ? QColor("#1E3A5F") : QBrush());
                if (match) {
                    treeWidget_->scrollToItem(diag);
                    area->setExpanded(true);
                    spec->setExpanded(true);
                }
            }
        }
    }
}

void DiagnosisTreeWidget::onFilterBySpecialty() {
    int idx = cbSpecialty_->currentIndex();
    if (idx <= 0) {
        // Show all
        auto results = module_->preOrderTraversal();
        showTraversalResults(results, "Todos los diagnosticos");
        return;
    }
    QString specialty = cbSpecialty_->currentText();
    auto results = module_->listBySpecialty(specialty);
    showTraversalResults(results, QString("Especialidad: %1").arg(specialty));
}

void DiagnosisTreeWidget::showTraversalResults(const QVector<Diagnostico>& results, const QString& title) {
    QString text;
    text += QString("<h3>%1</h3>").arg(title);
    text += QString("<p>Total: <b>%1</b> diagnosticos</p><hr/>").arg(results.size());
    int n = qMin(results.size(), 50);
    for (int i = 0; i < n; ++i) {
        const auto& d = results[i];
        text += QString("<p><b>[%1]</b> %2<br/><small>%3 &rarr; %4</small></p>")
            .arg(QString::fromStdString(d.codigo))
            .arg(QString::fromStdString(d.nombre))
            .arg(QString::fromStdString(d.categoria))
            .arg(QString::fromStdString(d.subcategoria));
    }
    if (results.size() > 50)
        text += QString("<p><i>... y %1 mas</i></p>").arg(results.size() - 50);
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
    if (!item) return;

    // Determine the level based on parent hierarchy
    int depth = 0;
    QTreeWidgetItem* p = item->parent();
    while (p) { ++depth; p = p->parent(); }

    QString html;
    if (depth == 0) {
        // Area level
        html = QString("<h3>Area Medica: %1</h3>"
                       "<p>Especialidades: <b>%2</b></p>"
                       "<p>Seleccione una especialidad o diagnostico para mas detalles.</p>")
            .arg(item->text(0))
            .arg(item->childCount());
    } else if (depth == 1) {
        // Specialty level
        int diagCount = item->childCount();
        html = QString("<h3>Especialidad: %1</h3>"
                       "<p>Area: %2</p>"
                       "<p>Diagnosticos en esta especialidad: <b>%3</b></p>")
            .arg(item->text(0))
            .arg(item->parent()->text(0))
            .arg(diagCount);
        if (diagCount > 0) {
            html += "<ul>";
            for (int i = 0; i < diagCount; ++i)
                html += QString("<li>[%1] %2</li>")
                    .arg(item->child(i)->text(1))
                    .arg(item->child(i)->text(0));
            html += "</ul>";
        }
    } else {
        // Diagnosis level
        QString desc = item->data(0, Qt::UserRole).toString();
        QString area = item->data(1, Qt::UserRole).toString();
        QString spec = item->data(2, Qt::UserRole).toString();

        html = QString(
            "<h3>%1</h3>"
            "<table cellpadding='3'>"
            "<tr><td><b>Codigo:</b></td><td>%2</td></tr>"
            "<tr><td><b>Area:</b></td><td>%3</td></tr>"
            "<tr><td><b>Especialidad:</b></td><td>%4</td></tr>"
            "<tr><td><b>Descripcion:</b></td><td>%5</td></tr>"
            "</table>")
            .arg(item->text(0))
            .arg(item->text(1))
            .arg(area)
            .arg(spec)
            .arg(desc.isEmpty() ? "<i>Sin descripcion disponible</i>" : desc);
    }

    detailPanel_->setHtml(html);
}
