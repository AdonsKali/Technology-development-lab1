#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <QVector>
#include <QFile>
#include <string>

class FileManager
{
private:
    QVector<QFile> files;
public:
    FileManager(std::string);
    ~FileManager();

};

#endif // FILEMANAGER_H
