#include "ellipseselection.h"

EllipseSelection::EllipseSelection(QCustomPlot *parentPlot) :
    QCPAbstractItem(parentPlot)
{
    ellipse = new QCPItemEllipse(parentPlot);
}

EllipseSelection::~EllipseSelection()
{
}

void EllipseSelection::startSelection(QMouseEvent *event)
{
    mActive = true;
    ellipse->topLeft->setCoords(event->pos());
    ellipse->bottomRight->setCoords(event->pos());
}

void EllipseSelection::moveSelection(QMouseEvent *event)
{
    ellipse->bottomRight->setCoords(event->pos());
    layer()->replot();
}

void EllipseSelection::endSelection(QMouseEvent *event)
{
    ellipse->bottomRight->setCoords(event->pos());
    mActive = false;
}
