#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QCPScatterStyle style;
    style.setShape(QCPScatterStyle::ssDisc);
    style.setSize(1);
    style.setPen(QPen(Qt::black));


    w.show();
    return a.exec();
}
