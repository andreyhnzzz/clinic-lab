#pragma once
#include <QObject>
#include <QVector>
#include <QDateTime>
#include "../models/Paciente.h"
#include "../data_structures/PriorityQueue.h"

class Module1_AttentionQueue : public QObject {
    Q_OBJECT
public:
    explicit Module1_AttentionQueue(QObject* parent = nullptr);

    void addPatient(const Paciente& p);
    Paciente attendNextPatient();
    QVector<Paciente> getPatientsInQueue() const;
    double getAverageWaitTimeMinutes() const;
    int countByPriority(int priority) const;
    bool isEmpty() const;
    int size() const;
    void clear();

signals:
    void queueChanged();
    void patientAttended(const Paciente& p);

private:
    PriorityQueue<Paciente> queue_;

    struct QueueEntry {
        Paciente paciente;
        QDateTime arrivalTime;
    };
    QVector<QueueEntry> arrivalLog_;
};
