#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    int numrow = 10;
    int numcolumn = 10;
    MainWindow w;
    w.setFixedSize(30*numrow,30*numcolumn + 70);
    w.show();
    return a.exec();
}
