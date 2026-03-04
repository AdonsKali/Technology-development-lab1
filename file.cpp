#include "file.h"

File::File(const QString& filePath, QObject* parent)
    : QObject(parent)
    , m_filePath(filePath)
    , m_exists(false)
    , m_size(0)
    , m_fileInfo(filePath)
    , m_lastCheckTime(0)
{
    updateState();
}

void File::checkState()
{
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    m_fileInfo.refresh();

    bool currentExists = m_fileInfo.exists();
    qint64 currentSize = currentExists ? m_fileInfo.size() : 0;

    if (currentExists != m_exists) {
        if (currentExists) {
            emit fileCreated(m_filePath, currentSize);
        } else {
            emit fileDeleted(m_filePath);
        }
    }
    else if (currentExists && m_exists && currentSize != m_size) {
        emit fileModified(m_filePath, currentSize, m_size);
    }

    // Обновляем сохраненное состояние
    m_exists = currentExists;
    m_size = currentSize;
    m_lastCheckTime = currentTime;
}

void File::updateState()
{
    m_fileInfo.refresh();
    m_exists = m_fileInfo.exists();
    m_size = m_exists ? m_fileInfo.size() : 0;
}
