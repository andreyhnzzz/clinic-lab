#include "Module1_AttentionQueue.h"

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
    emit queueChanged();
}

Paciente Module1_AttentionQueue::attendNextPatient() {
    if (queue_.isEmpty()) return {};
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
}

int Module1_AttentionQueue::countByPriority(int priority) const {
    int count = 0;
    for (const auto& p : queue_.data())
        if (p.prioridad == priority) ++count;
    return count;
}

bool Module1_AttentionQueue::isEmpty() const { return queue_.isEmpty(); }
int Module1_AttentionQueue::size() const { return queue_.size(); }
void Module1_AttentionQueue::clear() { queue_.clear(); arrivalLog_.clear(); emit queueChanged(); }
