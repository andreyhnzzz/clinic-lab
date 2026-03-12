#pragma once
#include <QObject>
#include <QVector>
#include <QDateTime>
#include "../models/Paciente.h"
#include "../data_structures/PriorityQueue.h"

<<<<<<< HEAD
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

=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
class Module1_AttentionQueue : public QObject {
    Q_OBJECT
public:
    explicit Module1_AttentionQueue(QObject* parent = nullptr);

    void addPatient(const Paciente& p);
    Paciente attendNextPatient();
<<<<<<< HEAD
    QVector<QueuedPatient> getQueueInAttentionOrder() const;
=======
    QVector<Paciente> getPatientsInQueue() const;
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    double getAverageWaitTimeMinutes() const;
    int countByPriority(int priority) const;
    bool isEmpty() const;
    int size() const;
    void clear();
<<<<<<< HEAD
    bool containsCedula(const QString& cedula) const;

    const QVector<Paciente>& recentlyAttended() const { return attendedHistory_; }
=======
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31

signals:
    void queueChanged();
    void patientAttended(const Paciente& p);

private:
<<<<<<< HEAD
    PriorityQueue<QueuedPatient> queue_;
    int nextArrivalOrder_ = 0;
    QVector<Paciente> attendedHistory_;
=======
    PriorityQueue<Paciente> queue_;

    struct QueueEntry {
        Paciente paciente;
        QDateTime arrivalTime;
    };
    QVector<QueueEntry> arrivalLog_;
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
};
