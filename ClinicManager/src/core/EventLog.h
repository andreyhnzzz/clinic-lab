#pragma once
#include <QObject>
#include <QVector>
#include <QString>
#include <QDateTime>

struct LogEntry {
    enum Level { Info, Success, Warning, Error };
    Level level = Info;
    QString category;
    QString message;
    QDateTime timestamp;

    QString levelString() const {
        switch (level) {
            case Success: return "OK";
            case Warning: return "WARN";
            case Error:   return "ERROR";
            default:      return "INFO";
        }
    }
};

class EventLog : public QObject {
    Q_OBJECT
public:
    static EventLog& instance();

    void log(LogEntry::Level level, const QString& category, const QString& message);
    void info(const QString& category, const QString& message);
    void success(const QString& category, const QString& message);
    void warning(const QString& category, const QString& message);
    void error(const QString& category, const QString& message);

    const QVector<LogEntry>& entries() const { return entries_; }
    void clear() { entries_.clear(); emit logCleared(); }

    int maxEntries() const { return maxEntries_; }
    void setMaxEntries(int max) { maxEntries_ = max; }

signals:
    void entryAdded(const LogEntry& entry);
    void logCleared();

private:
    explicit EventLog(QObject* parent = nullptr);
    QVector<LogEntry> entries_;
    int maxEntries_ = 500;
};
