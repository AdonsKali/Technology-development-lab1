#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QMap>
#include <QFileInfo>
#include <QTimer>
#include <memory>
#include "logger.h"

struct FileState {
    bool exists;
    qint64 size;

    FileState() : exists(false), size(0) {}
    FileState(bool e, qint64 s) : exists(e), size(s) {}
};

class FileManager : public QObject
{
    Q_OBJECT

public:
    static FileManager& getInstance();

    void setLogger(std::shared_ptr<Logger> logger);

public slots:
    void addFiles(const QStringList& paths);
    void removeFile(const QString& path);
    void startMonitoring(int intervalMs = 100);
    void stopMonitoring();
    void listFiles();
    void checkFiles();

signals:
    void fileAdded(const QString& path);
    void fileRemoved(const QString& path);
    void fileCreated(const QString& path, qint64 size);
    void fileDeleted(const QString& path);
    void fileModified(const QString& path, qint64 oldSize, qint64 newSize);
    void monitoringStarted(int interval);
    void monitoringStopped();
    void errorOccurred(const QString& message);
    void infoMessage(const QString& message);

private:
    FileManager(const FileManager&) = delete;
    FileManager& operator=(const FileManager&) = delete;
    FileManager(QObject* parent = nullptr);
    ~FileManager() = default;

    void connectSignalsToLogger();

    QMap<QString, FileState> m_files;
    bool m_isMonitoring;
    std::shared_ptr<Logger> m_logger;
    QTimer* m_monitorTimer;
};

#endif
