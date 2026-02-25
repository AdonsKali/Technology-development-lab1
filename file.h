#ifndef FILE_H
#define FILE_H
#include <QFile>
#include <QFileInfo>

class File
{
    private:
        float size;
        bool exist;

        QFileInfo flieInfo;
        std::string path;
        bool isChanged();
    public:
        File();
        float getSize();
        bool isExist();
};

#endif // FILE_H
