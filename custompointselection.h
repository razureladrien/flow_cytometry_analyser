#include "qcustomplot.h"

#ifndef CUSTOMPOINTSELECTION_H
#define CUSTOMPOINTSELECTION_H



class CustomPointSelection
{
private:
    QVector<double> select_x, select_y;
    QVector<double> verteces_x, verteces_y;
public:
    CustomPointSelection();
    ~CustomPointSelection();

    QVector<QVector<double>> getSelectionPoints();
    QVector<QVector<double>> getVerteces();

    void addSelectionPoint(double x, double y);
    void addVertex(double x, double y);
    void clearVerteces();
    void clearSelectionPoints();

    void pointsInEllipse(QCPItemEllipse *ellipse, QVector<double> vec_x, QVector<double> vec_y);
    void pointsInPoly(QVector<double> vec_x, QVector<double> vec_y);
};

#endif // CUSTOMPOINTSELECTION_H
