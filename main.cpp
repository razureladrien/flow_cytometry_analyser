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

//    for (int i=0; i<100; i++)
//    {
//        w.addPoint(rand() % 100, rand()% 100);
//    }

//    w.plot(style, 0, w.getX(), w.getY());

    w.show();
    return a.exec();
}
