#include <QCoreApplication>
#include <QMetaObject>
#include <thread>
#include <iostream>
#include <QDebug>
#include <QString>
#include "filemanager.h"

#ifdef Q_OS_WINDOWS
#include <windows.h>
#endif

void setupConsole()
{
#ifdef Q_OS_WINDOWS
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
#endif
}


int main(int argc, char *argv[])
{
    setupConsole();
    QCoreApplication app(argc, argv);
    qDebug() << QString::fromUtf8("Введите команду: ");


    FileManager& manager = FileManager::getInstance();

    std::thread inputThread([&]() {

        std::string input;

        while (true) {

            if (!std::getline(std::cin, input))
                continue;

            if (input.empty()) {
                continue;
            }

            QString line = QString::fromStdString(input);

            QMetaObject::invokeMethod(
                &app,
                [&, line]() {

                    QStringList parts =
                        line.split(' ', Qt::SkipEmptyParts);

                    if (parts.isEmpty()) {
                        return;
                    }

                    QString command =
                        parts[0].toLower();

                    if (command == "exit" ||
                        command == "quit") {
                        qDebug() << "Завершение программы...\n ";
                        app.quit();
                        return;
                    }
                    else if (command == "add" &&
                             parts.size() > 1) {
                        QStringList files =
                            parts[1].split(',', Qt::SkipEmptyParts);
                        manager.addFiles(files);
                    }
                    else if (command == "remove" &&
                             parts.size() > 1) {
                        manager.removeFile(parts[1]);
                    }
                    else if (command == "list") {
                        manager.listFiles();
                    }
                    else if (command == "start") {
                        int interval = 100;

                        if (parts.size() > 1) {
                            bool ok;
                            int customInterval =
                                parts[1].toInt(&ok);
                            if (ok && customInterval > 0)
                                interval = customInterval;
                        }

                        manager.startMonitoring(interval);
                    }
                    else if (command == "stop") {
                        manager.stopMonitoring();
                    }
                    else {
                        qDebug() << QString::fromUtf8("Неизвестная программа\n ");
                    }

                },
                Qt::QueuedConnection
                );
        }
    });

    inputThread.detach();

    return app.exec();
}
