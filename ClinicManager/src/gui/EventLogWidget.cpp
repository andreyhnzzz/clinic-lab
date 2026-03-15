#include "EventLogWidget.h"
#include "../core/EventLog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFont>

EventLogWidget::EventLogWidget(QWidget* parent)
    : QWidget(parent) {
    setupUI();
    connect(&EventLog::instance(), &EventLog::entryAdded,
            this, &EventLogWidget::onNewEntry);
    connect(&EventLog::instance(), &EventLog::logCleared,
            this, &EventLogWidget::refreshTable);
}

void EventLogWidget::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(12, 12, 12, 12);

    // Header
    auto* headerLayout = new QHBoxLayout();
    auto* titleLabel = new QLabel("Registro de Eventos del Sistema");
    QFont titleFont;
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #E6EDF3;");
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    headerLayout->addWidget(new QLabel("Filtrar:"));
    cbFilter_ = new QComboBox();
    cbFilter_->addItems({"Todos", "INFO", "OK", "WARN", "ERROR"});
    cbFilter_->setMinimumWidth(100);
    headerLayout->addWidget(cbFilter_);

    lblCount_ = new QLabel("0 eventos");
    lblCount_->setStyleSheet("color: #7D8894;");
    headerLayout->addWidget(lblCount_);

    btnClear_ = new QPushButton("Limpiar");
    btnClear_->setMinimumHeight(30);
    headerLayout->addWidget(btnClear_);
    mainLayout->addLayout(headerLayout);

    // Table
    table_ = new QTableWidget(0, 4);
    table_->setHorizontalHeaderLabels({"Nivel", "Categoria", "Mensaje", "Hora"});
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    table_->horizontalHeader()->resizeSection(0, 70);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->verticalHeader()->setVisible(false);
    mainLayout->addWidget(table_, 1);

    connect(cbFilter_, &QComboBox::currentTextChanged,
            this, &EventLogWidget::onFilterChanged);
    connect(btnClear_, &QPushButton::clicked, this, [this](){
        EventLog::instance().clear();
    });
}

void EventLogWidget::onNewEntry() {
    refreshTable();
}

void EventLogWidget::onFilterChanged() {
    refreshTable();
}

void EventLogWidget::refreshTable() {
    table_->setRowCount(0);
    QString filter = cbFilter_->currentText();

    const auto& entries = EventLog::instance().entries();
    int count = 0;

    for (int i = entries.size() - 1; i >= 0; --i) {
        const auto& e = entries[i];
        if (filter != "Todos" && e.levelString() != filter) continue;

        int row = table_->rowCount();
        table_->insertRow(row);
        count++;

        auto* lvlItem = new QTableWidgetItem(e.levelString());
        QColor color;
        switch (e.level) {
            case LogEntry::Success: color = QColor("#1DBF73"); break;
            case LogEntry::Warning: color = QColor("#F59E0B"); break;
            case LogEntry::Error:   color = QColor("#EF4444"); break;
            default:                color = QColor("#38BDF8"); break;
        }
        lvlItem->setForeground(color);
        table_->setItem(row, 0, lvlItem);
        table_->setItem(row, 1, new QTableWidgetItem(e.category));
        table_->setItem(row, 2, new QTableWidgetItem(e.message));
        table_->setItem(row, 3, new QTableWidgetItem(e.timestamp.toString("hh:mm:ss.zzz")));
    }

    lblCount_->setText(QString("%1 eventos").arg(count));
}
