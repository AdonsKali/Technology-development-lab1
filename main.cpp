#include <QCoreApplication>
#include <thread>
#include <chrono>
#include <memory>
#include <iostream>
#include <winnls.h>
#include "filemanager.h"
#include "consolelogger.h"

#ifdef Q_OS_WINDOWS
#include <windows.h>
#endif

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
void printHelp()
{
    std::cout << "КОМАНДЫ:\n";
    std::cout << "  add <путь1,путь2,...> - добавить файлы\n";
    std::cout << "  remove <путь> - удалить файл\n";
    std::cout << "  list - показать файлы\n";
    std::cout << "  start - запустить мониторинг\n";
    std::cout << "  stop - остановить мониторинг\n";
    std::cout << "  help - справка\n";
    std::cout << "  exit - выход\n";
}


int main()
{
    setupConsole();
    auto logger = std::make_shared<ConsoleLogger>();
    FileManager& manager = FileManager::getInstance();
    manager.setLogger(logger);

    printHelp();

    bool running = true;

    std::thread monitorThread([&]() {
        while (running) {
            if (manager.isMonitoring()) {
                manager.checkFiles();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    while (true) {
        std::cout << "\n> Введите команду: " << std::flush;
        std::string input;
        std::getline(std::cin, input);

        QString line =toQStringCP866(input);
        if (line.isEmpty()) continue;

        QStringList parts = line.split(' ', Qt::SkipEmptyParts);
        QString cmd = parts[0].toLower();

        if (cmd == "exit" || cmd == "quit") {
            std::cout << "Завершение программы..." << std::endl;
            running = false;
            break;
        }
        else if (cmd == "help") {
            printHelp();
        }
        else if (cmd == "add" && parts.size() > 1) {
            QStringList files = parts[1].split(',', Qt::SkipEmptyParts);
            manager.addFiles(files);
        }
        else if (cmd == "remove" && parts.size() > 1) {
            manager.removeFile(parts[1]);
        }
        else if (cmd == "list") {
            manager.listFiles();
        }
        else if (cmd == "start") {
            if (parts.size() > 1) {
                int interval = parts[1].toInt();
                manager.startMonitoring(interval);
            } else {
                manager.startMonitoring();
            }
        }
        else if (cmd == "stop") {
            manager.stopMonitoring();
        }
        else {
            std::cout << "Неизвестная команда" << std::endl;
        }
    }

    monitorThread.join();
    return 0;
}
