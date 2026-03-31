#include "filemanager.h"
#include <QDir>

FileManager::FileManager(QObject* parent)
    : QObject(parent)
    , m_isMonitoring(false)
{
}

FileManager& FileManager::getInstance()
{
    static FileManager instance;
    return instance;
}

void FileManager::setLogger(std::shared_ptr<Logger> logger)
{
    m_logger = logger;
}

void FileManager::addFiles(const QStringList& paths)
{
    if (!m_logger) return;

    for (const QString& path : paths) {
        QString trimmedPath = path.trimmed();
        if (trimmedPath.isEmpty()) continue;

        QString normalizedPath = QDir::fromNativeSeparators(trimmedPath);

        if (!m_files.contains(normalizedPath)) {
            QFileInfo info(normalizedPath);
            bool exists = info.exists();
            qint64 size = exists ? info.size() : 0;

            m_files[normalizedPath] = FileState(exists, size);
            m_logger->logFileAdded(normalizedPath);
        } else {
            m_logger->logError("Файл уже отслеживается: " + normalizedPath);
        }
    }
}

void FileManager::removeFile(const QString& path)
{
    if (!m_logger) return;

    QString normalizedPath = QDir::fromNativeSeparators(path.trimmed());

    if (m_files.contains(normalizedPath)) {
        m_files.remove(normalizedPath);
        m_logger->logFileRemoved(normalizedPath);
    } else {
        m_logger->logError("Файл не найден: " + normalizedPath);
    }
}

void FileManager::listFiles() const
{
    if (!m_logger) return;

    m_logger->logInfo("=== Отслеживаемые файлы ===");
    if (m_files.isEmpty()) {
        m_logger->logInfo("Список пуст");
    } else {
        for (auto it = m_files.constBegin(); it != m_files.constEnd(); ++it) {
            QString status = it.value().exists ? "существует" : "не существует";
            m_logger->logInfo(QStringLiteral("  %1 - %2 (размер: %3 байт)")
                                  .arg(it.key(), status, QString::number(it.value().size)));
        }
    }
}

void FileManager::startMonitoring()
{
    if (!m_logger) return;

    if (!m_isMonitoring) {
        if (m_files.isEmpty()) {
            m_logger->logError("Нет файлов для отслеживания");
            return;
        }
        m_isMonitoring = true;
        m_logger->logMonitoringStarted(100);
    } else {
        m_logger->logError("Мониторинг уже запущен");
    }

}

void FileManager::stopMonitoring()
{
    if (!m_logger) return;

    if (m_isMonitoring) {
        m_isMonitoring = false;
        m_logger->logMonitoringStopped();
    } else {
        m_logger->logError("Мониторинг не был запущен");
    }
}

void FileManager::checkFiles()
{
    if (!m_isMonitoring || !m_logger) return;

    for (auto it = m_files.begin(); it != m_files.end(); ++it) {
        const QString& path = it.key();
        FileState& oldState = it.value();

        QFileInfo info(path);
        bool nowExists = info.exists();
        qint64 nowSize = nowExists ? info.size() : 0;

        if (nowExists != oldState.exists) {
            if (nowExists) {
                m_logger->logFileCreated(path, nowSize);
            } else {
                m_logger->logFileDeleted(path);
            }
        }
        else if (nowExists && oldState.exists && nowSize != oldState.size) {
            m_logger->logFileModified(path, oldState.size, nowSize);
        }


        oldState.exists = nowExists;
        oldState.size = nowSize;
    }
}
