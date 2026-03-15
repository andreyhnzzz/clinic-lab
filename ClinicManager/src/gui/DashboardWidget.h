#pragma once
#include <QWidget>
#include <QLabel>

class DashboardWidget : public QWidget {
    Q_OBJECT
public:
    explicit DashboardWidget(QWidget* parent = nullptr);

public slots:
    void refresh();
    void setQueueSize(int size);
    void setAuditStatus(const QString& status);
    void setNextPatient(const QString& info);
    void setLastBenchmark(const QString& info);

private:
    void setupUI();

    QLabel* lblTotalPacientes_ = nullptr;
    QLabel* lblTotalConsultas_ = nullptr;
    QLabel* lblEnCola_ = nullptr;
    QLabel* lblProximaAtencion_ = nullptr;
    QLabel* lblDiagnosticos_ = nullptr;
    QLabel* lblLastBenchmark_ = nullptr;
    QLabel* lblAuditStatus_ = nullptr;
    QLabel* lblLastUpdate_ = nullptr;
};
