#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QMap>
#include <QTimer>
#include <QSharedPointer>
#include "file.h"

class FileManager : public QObject
{
    Q_OBJECT

public:
    static FileManager& getInstance();

    FileManager(const FileManager&) = delete;
    FileManager& operator=(const FileManager&) = delete;
    bool isMonitoring() const { return m_isMonitoring; }

    void addFiles(const QStringList& paths);
    void removeFile(const QString& path);
    void startMonitoring(int intervalMs = 100);
    void stopMonitoring();
    void listFiles() const;
    void checkFiles();

private slots:
    void onFileCreated(const QString& path, int size);
    void onFileDeleted(const QString& path);
    void onFileModified(const QString& path, int newSize, int oldSize);

private:
    FileManager(QObject* parent = nullptr);
    ~FileManager();

    QMap<QString, QSharedPointer<File>> m_files;
    QTimer* m_timer;
    bool m_isMonitoring;
};

#endif
