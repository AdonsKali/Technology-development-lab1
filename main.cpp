#include <QCoreApplication>
#include <memory>
#include <iostream>
#include <winnls.h>
#include "filemanager.h"
#include <QThread>
#include "consolelogger.h"
#include <windows.h>

QString toQStringCP866(const std::string& str)
{
#ifdef Q_OS_WINDOWS
    int wideSize = MultiByteToWideChar(866, 0, str.c_str(), -1, NULL, 0);
    if (wideSize > 0) {
        std::wstring wstr(wideSize, L'\0');
        MultiByteToWideChar(866, 0, str.c_str(), -1, &wstr[0], wideSize);
        wstr.resize(wideSize - 1);
        return QString::fromStdWString(wstr);
    }
    return QString::fromLocal8Bit(str.c_str());
#endif
}

void setupConsole()
{
#ifdef Q_OS_WINDOWS
    SetConsoleOutputCP(CP_UTF8);
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
#endif
}

QStringList parseArgs(const QString& line)
{
    QStringList result;
    QString current;
    bool inQuotes = false;

    for (QChar ch : line) {
        if (ch == '"') {
            inQuotes = !inQuotes;
        } else if (ch == ' ' && !inQuotes) {
            if (!current.isEmpty()) result.append(current);
            current.clear();
        } else {
            current.append(ch);
        }
    }
    if (!current.isEmpty()) result.append(current);
    return result;
}

QStringList extractPaths(const QString& arg)
{
    QStringList paths;
    QString current;
    bool inQuotes = false;

    for (QChar ch : arg) {
        if (ch == '"') {
            inQuotes = !inQuotes;
        } else if (ch == ',' && !inQuotes) {
            if (!current.isEmpty()) paths.append(current);
            current.clear();
        } else {
            current.append(ch);
        }
    }
    if (!current.isEmpty()) paths.append(current);

    for (QString& path : paths) {
        path = path.trimmed();
        if (path.startsWith('"') && path.endsWith('"')) {
            path = path.mid(1, path.length() - 2);
        }
    }
    return paths;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    setupConsole();

    auto logger = std::make_shared<ConsoleLogger>();
    FileManager& manager = FileManager::getInstance();
    manager.setLogger(logger);

    logger->printHelp();

    QThread inputThread;
    bool running = true;

    QObject::connect(&inputThread, &QThread::started, [&]() {
        while (running) {
            std::cout << "\n> Введите команду: " << std::flush;
            std::string input;
            std::getline(std::cin, input);

            if (!running) break;
            if (input.empty()) continue;

            QString line = toQStringCP866(input);
            QStringList parts = parseArgs(line);
            if (parts.isEmpty()) continue;

            QString cmd = parts[0].toLower();

            if (cmd == "exit" || cmd == "quit") {
                std::cout << "Завершение программы..." << std::endl;
                running = false;
                QMetaObject::invokeMethod(&app, &QCoreApplication::quit, Qt::QueuedConnection);
                break;
            }
            else if (cmd == "help") {
                logger->printHelp();
            }
            else if (cmd == "add" && parts.size() > 1) {
                QString argsPart = parts.mid(1).join(' ');
                QStringList files = extractPaths(argsPart);
                QMetaObject::invokeMethod(&manager, [&manager, files]() {
                    manager.addFiles(files);
                }, Qt::QueuedConnection);
            }
            else if (cmd == "remove" && parts.size() > 1) {
                QString path = parts.mid(1).join(' ');
                if (path.startsWith('"') && path.endsWith('"')) {
                    path = path.mid(1, path.length() - 2);
                }
                QMetaObject::invokeMethod(&manager, [&manager, path]() {
                    manager.removeFile(path);
                }, Qt::QueuedConnection);
            }
            else if (cmd == "list") {
                QMetaObject::invokeMethod(&manager, [&manager]() {
                    manager.listFiles();
                }, Qt::QueuedConnection);
            }
            else if (cmd == "start") {
                int interval = 100;
                if (parts.size() > 1) {
                    interval = parts[1].toInt();
                    if (interval <= 0) interval = 100;
                }
                QMetaObject::invokeMethod(&manager, [&manager, interval]() {
                    manager.startMonitoring(interval);
                }, Qt::QueuedConnection);
            }
            else if (cmd == "stop") {
                QMetaObject::invokeMethod(&manager, [&manager]() {
                    manager.stopMonitoring();
                }, Qt::QueuedConnection);
            }
            else {
                std::cout << "Неизвестная команда. Введите 'help' для справки." << std::endl;
            }
        }
    });

    inputThread.start();
    int result = app.exec();
    running = false;
    inputThread.quit();
    inputThread.wait();

    return result;
}
