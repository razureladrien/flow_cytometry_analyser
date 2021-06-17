#include "qcustomplot.h"

#ifndef CUSTOMPOINTSELECTION_H
#define CUSTOMPOINTSELECTION_H



class CustomPointSelection
{
private:
    QList<int> selection_keys;
    QVector<double> select_x, select_y;
    QVector<double> verteces_x, verteces_y;
public:
    CustomPointSelection();
    ~CustomPointSelection();

    QList<int> getSelectionPoints() {return selection_keys;}
    QVector<QVector<double>> getVerteces();

    void addSelectionPoint(int key);
    void addVertex(double x, double y);
    void clearVerteces();
    void clearSelectionPoints();

    void pointsInEllipse(QCPItemEllipse *ellipse, QMap<int, QVector<double>> data);
    //void pointsInPoly(QVector<double> vec_x, QVector<double> vec_y);
};

#endif // CUSTOMPOINTSELECTION_H
