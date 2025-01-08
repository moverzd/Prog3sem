#include "mainwindow.h"

#include <QApplication>

#include <Qdir>

int main(int argc, char *argv[])
{
    QCoreApplication::addLibraryPath("/Users/USERNAME/Qt/6.8.0/macos/plugins");


    QApplication a(argc, argv);

    qDebug() << "Available SQL drivers:" << QSqlDatabase::drivers();
    qDebug() << "Текущая рабочая директория:" << QDir::currentPath();
    MainWindow w;
    w.show();
    return a.exec();
}
