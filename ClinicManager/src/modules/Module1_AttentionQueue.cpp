#include "Module1_AttentionQueue.h"
<<<<<<< HEAD
#include <algorithm>

Module1_AttentionQueue::Module1_AttentionQueue(QObject* parent)
    : QObject(parent),
      queue_([](const QueuedPatient& a, const QueuedPatient& b){ return a < b; })
{}

void Module1_AttentionQueue::addPatient(const Paciente& p) {
    QueuedPatient qp;
    qp.paciente = p;
    qp.arrivalOrder = nextArrivalOrder_++;
    qp.arrivalTime = QDateTime::currentDateTime();
    queue_.push(qp);
=======

Module1_AttentionQueue::Module1_AttentionQueue(QObject* parent)
    : QObject(parent),
      queue_([](const Paciente& a, const Paciente& b){ return a.prioridad < b.prioridad; })
{}

void Module1_AttentionQueue::addPatient(const Paciente& p) {
    queue_.push(p);
    QueueEntry entry;
    entry.paciente = p;
    entry.arrivalTime = QDateTime::currentDateTime();
    arrivalLog_.push_back(entry);
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
    emit queueChanged();
}

Paciente Module1_AttentionQueue::attendNextPatient() {
    if (queue_.isEmpty()) return {};
<<<<<<< HEAD
    QueuedPatient qp = queue_.pop();
    attendedHistory_.prepend(qp.paciente);
    if (attendedHistory_.size() > 20)
        attendedHistory_.resize(20);
    emit patientAttended(qp.paciente);
    emit queueChanged();
    return qp.paciente;
}

QVector<QueuedPatient> Module1_AttentionQueue::getQueueInAttentionOrder() const {
    // Extract all items, sort by priority then arrival order -> real attention order
    QVector<QueuedPatient> sorted = queue_.data();
    std::sort(sorted.begin(), sorted.end());
    return sorted;
}

double Module1_AttentionQueue::getAverageWaitTimeMinutes() const {
    const auto& heap = queue_.data();
    if (heap.isEmpty()) return 0.0;
    QDateTime now = QDateTime::currentDateTime();
    double total = 0.0;
    for (const auto& qp : heap)
        total += qp.arrivalTime.secsTo(now) / 60.0;
    return total / heap.size();
=======
    Paciente p = queue_.pop();
    // Remove from arrival log
    for (int i = 0; i < arrivalLog_.size(); ++i) {
        if (arrivalLog_[i].paciente.cedula == p.cedula) {
            arrivalLog_.remove(i);
            break;
        }
    }
    emit patientAttended(p);
    emit queueChanged();
    return p;
}

QVector<Paciente> Module1_AttentionQueue::getPatientsInQueue() const {
    return queue_.data();
}

double Module1_AttentionQueue::getAverageWaitTimeMinutes() const {
    if (arrivalLog_.isEmpty()) return 0.0;
    QDateTime now = QDateTime::currentDateTime();
    double total = 0.0;
    for (const auto& entry : arrivalLog_)
        total += entry.arrivalTime.secsTo(now) / 60.0;
    return total / arrivalLog_.size();
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
}

int Module1_AttentionQueue::countByPriority(int priority) const {
    int count = 0;
<<<<<<< HEAD
    for (const auto& qp : queue_.data())
        if (qp.paciente.prioridad == priority) ++count;
    return count;
}

bool Module1_AttentionQueue::containsCedula(const QString& cedula) const {
    std::string ced = cedula.toStdString();
    for (const auto& qp : queue_.data())
        if (qp.paciente.cedula == ced) return true;
    return false;
}

bool Module1_AttentionQueue::isEmpty() const { return queue_.isEmpty(); }
int Module1_AttentionQueue::size() const { return queue_.size(); }
void Module1_AttentionQueue::clear() {
    queue_.clear();
    nextArrivalOrder_ = 0;
    emit queueChanged();
}
=======
    for (const auto& p : queue_.data())
        if (p.prioridad == priority) ++count;
    return count;
}

bool Module1_AttentionQueue::isEmpty() const { return queue_.isEmpty(); }
int Module1_AttentionQueue::size() const { return queue_.size(); }
void Module1_AttentionQueue::clear() { queue_.clear(); arrivalLog_.clear(); emit queueChanged(); }
>>>>>>> 0b6db00e07b4a0712a21602b3913477cc7392e31
