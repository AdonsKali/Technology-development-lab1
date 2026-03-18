#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <QObject>
#include <QMap>
#include <QFileInfo>
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

    FileManager(const FileManager&) = delete;
    FileManager& operator=(const FileManager&) = delete;

    void setLogger(std::shared_ptr<Logger> logger);

    void addFiles(const QStringList& paths);
    void removeFile(const QString& path);
    void startMonitoring(int intervalMs = 100);
    void stopMonitoring();
    void listFiles() const;
    bool isMonitoring() const { return m_isMonitoring; }

    void checkFiles();
private:
    FileManager(QObject* parent = nullptr);
    ~FileManager() = default;

    QMap<QString, FileState> m_files;
    bool m_isMonitoring;
    std::shared_ptr<Logger> m_logger;
};

#endif
