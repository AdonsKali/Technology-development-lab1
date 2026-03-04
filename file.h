#ifndef FILE_H
#define FILE_H

#include <QObject>
#include <QFileInfo>
#include <QDateTime>

class File : public QObject
{
    Q_OBJECT

public:
    explicit File(const QString& filePath, QObject* parent = nullptr);

    void checkState();
    bool exists() const { return m_exists; }
    qint64 getSize() const { return m_size; }
    QString getPath() const { return m_filePath; }

signals:
    void fileCreated(const QString& path, qint64 size);
    void fileDeleted(const QString& path);
    void fileModified(const QString& path, qint64 newSize, qint64 oldSize);
    void fileRenamed(const QString& oldPath, const QString& newPath);

private:
    void updateState();

    QString m_filePath;
    bool m_exists;
    qint64 m_size;
    QFileInfo m_fileInfo;
    qint64 m_lastCheckTime;
    QByteArray m_lastFileId;
};

#endif
