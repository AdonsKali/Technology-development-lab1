#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QDateTime>

class Logger
{
public:
    virtual ~Logger() = default;

    virtual void logFileAdded(const QString& path) = 0;
    virtual void logFileRemoved(const QString& path) = 0;
    virtual void logFileCreated(const QString& path, qint64 size) = 0;
    virtual void logFileDeleted(const QString& path) = 0;
    virtual void logFileModified(const QString& path, qint64 oldSize, qint64 newSize) = 0;
    virtual void logMonitoringStarted(int interval) = 0;
    virtual void logMonitoringStopped() = 0;
    virtual void logError(const QString& message) = 0;
    virtual void logInfo(const QString& message) = 0;
    virtual void logPrompt() = 0;

protected:
    QString currentTimestamp() const {
        return QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    }
};

#endif
