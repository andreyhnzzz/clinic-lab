#pragma once
#include <QObject>
#include <QVector>
#include <QDateTime>
#include "../models/Paciente.h"
#include "../data_structures/PriorityQueue.h"

struct QueuedPatient {
    Paciente paciente;
    int arrivalOrder = 0;
    QDateTime arrivalTime;

    bool operator<(const QueuedPatient& other) const {
        if (paciente.prioridad != other.paciente.prioridad)
            return paciente.prioridad < other.paciente.prioridad;
        return arrivalOrder < other.arrivalOrder; // FIFO tiebreaker
    }
};

class Module1_AttentionQueue : public QObject {
    Q_OBJECT
public:
    explicit Module1_AttentionQueue(QObject* parent = nullptr);

    void addPatient(const Paciente& p);
    Paciente attendNextPatient();
    QVector<QueuedPatient> getQueueInAttentionOrder() const;
    double getAverageWaitTimeMinutes() const;
    int countByPriority(int priority) const;
    bool isEmpty() const;
    int size() const;
    void clear();
    bool containsCedula(const QString& cedula) const;

    const QVector<Paciente>& recentlyAttended() const { return attendedHistory_; }

signals:
    void queueChanged();
    void patientAttended(const Paciente& p);

private:
    PriorityQueue<QueuedPatient> queue_;
    int nextArrivalOrder_ = 0;
    QVector<Paciente> attendedHistory_;
};
