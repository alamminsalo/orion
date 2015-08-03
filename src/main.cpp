#include "ui/mainwindow.h"
#include <QApplication>
#include <QSystemTrayIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow main;
    main.show();

    return a.exec();
}
