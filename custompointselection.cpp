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
    qDebug() << selection_keys;
}

//void CustomPointSelection::pointsInPoly(QVector<double> v_x, QVector<double> v_y)
//{
//    // Copyright 2000 softSurfer, 2012 Dan Sunday

//    clearSelectionPoints();

//    QVector<double> vert_x = getVerteces()[0];
//    QVector<double> vert_y = getVerteces()[1];

//    int n = vert_x.length();

//    /* Bouding box for the selection */
//    double x_min = *std::min_element(vert_x.begin(), vert_x.end());
//    double x_max = *std::max_element(vert_x.begin(), vert_x.end());
//    double y_min = *std::min_element(vert_y.begin(), vert_y.end());
//    double y_max = *std::max_element(vert_y.begin(), vert_y.end());

//    for(int i =0; i<v_x.length(); i++)
//    {

//        int    cn = 0; // crossing number
//        if ( (x_min < v_x[i]) && (v_x[i] < x_max) && (y_min < v_y[i]) && (v_y[i] < y_max) )
//        {
//            for (int j=0; j<n-1; j++)
//            {
//                if ( ((vert_y[j] <= v_y[i]) && (vert_y[j+1] > v_y[i]))     // upward crossing
//                        || ((vert_y[j] > v_y[i]) && (vert_y[j+1] <=  v_y[i])) ) // downward crossing
//                {
//                    double vt = (v_y[i]  - vert_y[j]) / (vert_y[j+1] - vert_y[j]);
//                    if (v_x[i] <  vert_x[j] + vt * (vert_x[j+1] - vert_x[j]))
//                        cn ++;
//                }
//            }
//        }
//        if (cn&1)
//            addSelectionPoint(v_x[i], v_y[i]);
//    }
//}
