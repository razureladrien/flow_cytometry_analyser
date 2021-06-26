#include "custompointselection.h"

CustomPointSelection::CustomPointSelection()
{
}

CustomPointSelection::~CustomPointSelection()
{
}


QVector<QVector<double> > CustomPointSelection::getVerteces()
{
    QVector<QVector<double>> verteces;
    verteces.append(verteces_x);
    verteces.append(verteces_y);

    return verteces;
}

void CustomPointSelection::addSelectionPoint(int key)
{
    selection_keys.append(key);
//    select_x.append(x);
//    select_y.append(y);
}

void CustomPointSelection::addVertex(double x, double y)
{
    verteces_x.append(x);
    verteces_y.append(y);
}

void CustomPointSelection::clearVerteces()
{
    verteces_x.clear();
    verteces_y.clear();
}

void CustomPointSelection::clearSelectionPoints()
{
    selection_keys.clear();
    select_x.clear();
    select_y.clear();
}

void CustomPointSelection::pointsInEllipse(QCPItemEllipse *ellipse, QMap<int, QVector<double>> data)
{
    clearSelectionPoints();

    QPointF top_left = ellipse->topLeft->coords();
    QPointF bot_right = ellipse->bottomRight->coords();
    QPointF center = (top_left+bot_right)/2;
    double semi_x_axis = qAbs(top_left.x()-bot_right.x())/2.0;
    double semi_y_axis = qAbs(top_left.y()-bot_right.y())/2.0;
    double x_h;
    double y_k;

    for (auto i : data.keys())
    {
        x_h = data[i][0]-center.x();
        y_k = data[i][1]-center.y();
        if ((x_h*x_h) / (semi_x_axis*semi_x_axis) + (y_k*y_k) / (semi_y_axis*semi_y_axis) <= 1)
        {
            addSelectionPoint(i);
        }
    };
}

void CustomPointSelection::pointsInPoly(QMap<int, QVector<double>> data)
{
    // Copyright 2000 softSurfer, 2012 Dan Sunday

    clearSelectionPoints();

    QVector<double> vert_x = getVerteces()[0];
    QVector<double> vert_y = getVerteces()[1];

    int n = vert_x.length();

    /* Bouding box for the selection */
    double x_min = *std::min_element(vert_x.begin(), vert_x.end());
    double x_max = *std::max_element(vert_x.begin(), vert_x.end());
    double y_min = *std::min_element(vert_y.begin(), vert_y.end());
    double y_max = *std::max_element(vert_y.begin(), vert_y.end());

    /* x vector : data[i][0]
       y vector : data[i][1] */
    for(auto i : data.keys())
    {

        int    cn = 0; // crossing number
        if ( (x_min < data[i][0]) && (data[i][0] < x_max) && (y_min < data[i][1]) && (data[i][1] < y_max) )
        {
            for (int j=0; j<n-1; j++)
            {
                if ( ((vert_y[j] <= data[i][1]) && (vert_y[j+1] > data[i][1]))     // upward crossing
                        || ((vert_y[j] > data[i][1]) && (vert_y[j+1] <=  data[i][1])) ) // downward crossing
                {
                    double vt = (data[i][1]  - vert_y[j]) / (vert_y[j+1] - vert_y[j]);
                    if (data[i][0] <  vert_x[j] + vt * (vert_x[j+1] - vert_x[j]))
                        cn ++;
                }
            }
        }
        if (cn&1)
            addSelectionPoint(i);
    }
}
