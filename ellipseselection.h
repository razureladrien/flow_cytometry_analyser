#include "qcustomplot.h"

#ifndef ELLIPSESELECTION_H
#define ELLIPSESELECTION_H


class EllipseSelection : public QCPAbstractItem
{
public:
    EllipseSelection(QCustomPlot *parentPlot);
    ~EllipseSelection();

    bool isActive() const { return mActive; }

protected:
    bool mActive;
    QCPItemEllipse *ellipse;

    void startSelection(QMouseEvent *event);
    void moveSelection(QMouseEvent *event);
    void endSelection(QMouseEvent *event);

};

#endif // ELLIPSESELECTION_H
