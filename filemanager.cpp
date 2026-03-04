#include "filemanager.h"
#include <QDir>
#include <QTextStream>
#include <QDebug>

FileManager::FileManager(QObject* parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_isMonitoring(false)
{
    connect(m_timer, &QTimer::timeout, this, &FileManager::checkFiles);
}

FileManager::~FileManager()
{
    stopMonitoring();
    m_files.clear();
}

FileManager& FileManager::getInstance()
{
    static FileManager instance;
    return instance;
}

void FileManager::addFiles(const QStringList& paths)
{

    for (const QString& path : paths) {
        QString trimmedPath = path.trimmed();
        if (trimmedPath.isEmpty()) continue;

        QString normalizedPath = QDir::fromNativeSeparators(trimmedPath);

        if (!m_files.contains(normalizedPath)) {
            QSharedPointer<File> file = QSharedPointer<File>::create(normalizedPath, this);

            connect(file.data(), &File::fileCreated,
                    this, &FileManager::onFileCreated);
            connect(file.data(), &File::fileDeleted,
                    this, &FileManager::onFileDeleted);
            connect(file.data(), &File::fileModified,
                    this, &FileManager::onFileModified);

            m_files[normalizedPath] = file;
            qDebug() << "Файл добавлен " + normalizedPath;
        } else {
            qDebug() << ("Файл уже отслеживается: " + normalizedPath);
        }
    }
}

void FileManager::removeFile(const QString& path)
{

    QString normalizedPath = QDir::fromNativeSeparators(path.trimmed());

    if (m_files.contains(normalizedPath)) {
        m_files.remove(normalizedPath);
        qDebug() << ("Файл "+normalizedPath+" удален из отслеживания");
    } else {
        qDebug() << ("Файл не найден: " + normalizedPath);
    }
}

void FileManager::listFiles() const
{


    qDebug() << ("Отслеживаемые файлы");
    if (m_files.isEmpty()) {
        qDebug() << ("Список пуст");
    } else {
        for (auto it = m_files.constBegin(); it != m_files.constEnd(); ++it) {
            QString status = it.value()->exists() ? "существует" : "не существует";
            qint64 size = it.value()->getSize();
            qDebug() << QString("  %1 - %2 (размер: %3 байт)").arg(it.key()).arg(status).arg(size);
        }
    }
}

void FileManager::startMonitoring(int intervalMs)
{

    if (!m_isMonitoring) {
        if (m_files.isEmpty()) {
            qDebug() << QString::fromUtf8("Нет файлов для отслеживания");
            return;
        }
        m_isMonitoring = true;
        m_timer->start(intervalMs);
    } else {
        qDebug() << QString::fromUtf8("Мониторинг уже запущен");
    }
}

void FileManager::stopMonitoring()
{

    if (m_isMonitoring) {
        m_isMonitoring = false;
        m_timer->stop();
        qDebug() << QString::fromUtf8("Мониторинг остановлен");
    } else {
        qDebug() << QString::fromUtf8("Мониторинг не был запущен");
    }
}

void FileManager::checkFiles()
{
    if (!m_isMonitoring) return;
    for (auto it = m_files.begin(); it != m_files.end(); ++it) {
        if (it.value()) {
            it.value()->checkState();
        }
    }
}

void FileManager::onFileCreated(const QString& path, int size)
{
    qDebug() << ("Файл создан" + path + "Размер" + QString::number(size));
}

void FileManager::onFileDeleted(const QString& path)
{
    qDebug() << ("Файл удален" + path);
}

void FileManager::onFileModified(const QString& path, int newSize, int oldSize)
{
    qDebug() << ("Файл изменен" + path + "размер до " + QString::number(newSize) +"размер после " + QString::number(oldSize));
}
