#pragma once
#include <QObject>
#include <QString>

class JsonPersistence : public QObject {
    Q_OBJECT
public:
    explicit JsonPersistence(QObject* parent = nullptr);

    bool saveToFile(const QString& filePath);
    bool loadFromFile(const QString& filePath);

    static QString defaultFilePath();

signals:
    void saved(const QString& path);
    void loaded(const QString& path);
    void errorOccurred(const QString& message);
};
