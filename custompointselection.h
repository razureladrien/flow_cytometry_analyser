#include "qcustomplot.h"

#ifndef CUSTOMPOINTSELECTION_H
#define CUSTOMPOINTSELECTION_H


/*
 * This class is used to compute whether points are in a given
 * geometrical form (polygon, ellipse) or not
 */
class CustomPointSelection
{
private:
    QList<int> selection_keys; // contains the keys of the selected points
    QVector<double> verteces_x, verteces_y; // polygon coordinates
public:
    CustomPointSelection();
    ~CustomPointSelection();

    QList<int> getSelectionPoints() {return selection_keys;}
    QVector<QVector<double>> getVerteces();

    void addSelectionPoint(int key);
    void addVertex(double x, double y);
    void clearVerteces();
    void clearSelectionPoints();
    void setKeys(QList<int> keys);

    void pointsInEllipse(QCPItemEllipse *ellipse, QMap<int, QVector<double>> data, QString xscale, QString yscale);
    void pointsInPoly(QMap<int, QVector<double>> data, QString xscale, QString yscale);
};

#endif // CUSTOMPOINTSELECTION_H
