#include "mainwindow.h"
#include <QApplication>

#include <vector>

int main(int argc, char *argv[])
{
    qRegisterMetaType<std::vector<float>>("std::vector<float>");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
