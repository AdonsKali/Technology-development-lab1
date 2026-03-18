#ifndef CONSOLELOGGER_H
#define CONSOLELOGGER_H

#include <QTextStream>
#include "logger.h"
#include <iostream>

class ConsoleLogger : public Logger
{
public:
    ConsoleLogger() = default;

    void logFileAdded(const QString &path) override
    {
        std::cout << "\n[" << currentTimestamp().toStdString()
                  << "] ДОБАВЛЕН: " << path.toStdString() << std::endl;
    }

    void logFileRemoved(const QString &path) override
    {
        std::cout << "\n[" << currentTimestamp().toStdString()
                  << "] УДАЛЕН ИЗ ОТСЛЕЖИВАНИЯ: " << path.toStdString() << std::endl;
    }

    void logFileCreated(const QString &path, qint64 size) override
    {
        std::cout << "\n[" << currentTimestamp().toStdString() << "] СОЗДАН: " << path.toStdString()
                  << " (размер: " << size << " байт)" << std::endl;
    }

    void logFileDeleted(const QString &path) override
    {
        std::cout << "\n[" << currentTimestamp().toStdString() << "] УДАЛЕН: " << path.toStdString()
                  << std::endl;
    }

    void logFileModified(const QString &path, qint64 oldSize, qint64 newSize) override
    {
        std::cout << "\n[" << currentTimestamp().toStdString()
                  << "] ИЗМЕНЕН: " << path.toStdString() << " (был: " << oldSize
                  << ", стал: " << newSize << " байт)" << std::endl;
    }

    void logMonitoringStarted(int interval) override
    {
        std::cout << "\n[" << currentTimestamp().toStdString()
                  << "] МОНИТОРИНГ ЗАПУЩЕН. Интервал: " << interval << " мс" << std::endl;
    }

    void logMonitoringStopped() override
    {
        std::cout << "\n[" << currentTimestamp().toStdString() << "] МОНИТОРИНГ ОСТАНОВЛЕН"
                  << std::endl;
    }

    void logError(const QString &message) override
    {
        std::cout << "\n[" << currentTimestamp().toStdString()
                  << "] ОШИБКА: " << message.toStdString() << std::endl;
    }

    void logInfo(const QString &message) override
    {
        std::cout << "\n[" << currentTimestamp().toStdString()
                  << "] ИНФО: " << message.toStdString() << std::endl;
    }

    void logPrompt() override { std::cout << "\n> Введите команду: " << std::flush; }
};

#endif
