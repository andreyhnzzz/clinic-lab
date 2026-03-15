#include "EventLog.h"

EventLog& EventLog::instance() {
    static EventLog log;
    return log;
}

EventLog::EventLog(QObject* parent) : QObject(parent) {}

void EventLog::log(LogEntry::Level level, const QString& category, const QString& message) {
    LogEntry entry;
    entry.level = level;
    entry.category = category;
    entry.message = message;
    entry.timestamp = QDateTime::currentDateTime();

    entries_.push_back(entry);
    if (entries_.size() > maxEntries_)
        entries_.removeFirst();

    emit entryAdded(entry);
}

void EventLog::info(const QString& category, const QString& message) {
    log(LogEntry::Info, category, message);
}

void EventLog::success(const QString& category, const QString& message) {
    log(LogEntry::Success, category, message);
}

void EventLog::warning(const QString& category, const QString& message) {
    log(LogEntry::Warning, category, message);
}

void EventLog::error(const QString& category, const QString& message) {
    log(LogEntry::Error, category, message);
}
