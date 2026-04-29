#include "filemanager.h"
#include <QDir>

FileManager::FileManager(QObject* parent)
    : QObject(parent)
    , m_isMonitoring(false)
    , m_monitorTimer(new QTimer(this))
{
    connect(m_monitorTimer, &QTimer::timeout, this, &FileManager::checkFiles);
}

FileManager& FileManager::getInstance()
{
    static FileManager instance;
    return instance;
}

void FileManager::setLogger(std::shared_ptr<Logger> logger)
{
    disconnect(this, nullptr, nullptr, nullptr);
    m_logger = logger;

    if (!m_logger) return;
    connect(this, &FileManager::fileAdded, m_logger.get(), &Logger::logFileAdded);
    connect(this, &FileManager::fileRemoved, m_logger.get(), &Logger::logFileRemoved);
    connect(this, &FileManager::fileCreated, m_logger.get(), &Logger::logFileCreated);
    connect(this, &FileManager::fileDeleted, m_logger.get(), &Logger::logFileDeleted);
    connect(this, &FileManager::fileModified, m_logger.get(), &Logger::logFileModified);
    connect(this, &FileManager::monitoringStarted, m_logger.get(), &Logger::logMonitoringStarted);
    connect(this, &FileManager::monitoringStopped, m_logger.get(), &Logger::logMonitoringStopped);
    connect(this, &FileManager::errorOccurred, m_logger.get(), &Logger::logError);
    connect(this, &FileManager::infoMessage, m_logger.get(), &Logger::logInfo);
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
            emit fileAdded(normalizedPath);
        } else {
            emit infoMessage("Файл уже отслеживается: " + normalizedPath);
        }
    }
}

void FileManager::removeFile(const QString& path)
{
    if (!m_logger) return;

    QString normalizedPath = QDir::fromNativeSeparators(path.trimmed());

    if (m_files.contains(normalizedPath)) {
        m_files.remove(normalizedPath);
        emit fileRemoved(normalizedPath);
    } else {
        emit errorOccurred("Файл не найден: " + normalizedPath);
    }
}

void FileManager::listFiles()
{
    if (!m_logger) return;

    emit infoMessage("=== Отслеживаемые файлы ===");
    if (m_files.isEmpty()) {
        emit infoMessage("Список пуст");
    } else {
        for (auto it = m_files.constBegin(); it != m_files.constEnd(); ++it) {
            QString status = it.value().exists ? "существует" : "не существует";
            emit infoMessage(QStringLiteral("  %1 - %2 (размер: %3 байт)")
                                 .arg(it.key(), status, QString::number(it.value().size)));
        }
    }
}

void FileManager::startMonitoring(int intervalMs)
{
    if (!m_logger) return;

    if (!m_isMonitoring) {
        if (m_files.isEmpty()) {
            emit infoMessage("Нет файлов для отслеживания");
            return;
        }
        m_isMonitoring = true;
        m_monitorTimer->start(intervalMs);
        emit monitoringStarted(intervalMs);
    } else {
        emit infoMessage("Мониторинг уже запущен");
    }
}

void FileManager::stopMonitoring()
{
    if (!m_logger) return;

    if (m_isMonitoring) {
        m_isMonitoring = false;
        m_monitorTimer->stop();
        emit monitoringStopped();
    } else {
        emit errorOccurred("Мониторинг не был запущен");
    }
}

void FileManager::checkFiles()
{
    if (!m_isMonitoring) return;

    for (auto it = m_files.begin(); it != m_files.end(); ++it) {
        const QString& path = it.key();
        FileState& oldState = it.value();

        QFileInfo info(path);
        bool nowExists = info.exists();
        qint64 nowSize = nowExists ? info.size() : 0;

        if (nowExists != oldState.exists) {
            if (nowExists) {
                emit fileCreated(path, nowSize);
            } else {
                emit fileDeleted(path);
            }
        }
        else if (nowExists && oldState.exists && nowSize != oldState.size) {
            emit fileModified(path, oldState.size, nowSize);
        }

        oldState.exists = nowExists;
        oldState.size = nowSize;
    }
}
