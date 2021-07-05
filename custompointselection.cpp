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

void CustomPointSelection::setKeys(QList<int> keys)
{
    selection_keys = keys;
}

void CustomPointSelection::pointsInEllipse(QCPItemEllipse *ellipse, QMap<int, QVector<double>> data, QString xscale, QString yscale)
{
    clearSelectionPoints();
    QPointF top_left = ellipse->topLeft->coords();
    QPointF bot_right = ellipse->bottomRight->coords();

    if ((xscale == "log") & (yscale == "lin"))
    {
        double center_y = (top_left.y() + bot_right.y())/2;
        double center_x = (log10(top_left.x()) + log10(bot_right.x()))/2;
        double semi_x_axis = qAbs(log10(top_left.x())-log10(bot_right.x()))/2.0;
        double semi_y_axis = qAbs(top_left.y()-bot_right.y())/2.0;
        double x_h;
        double y_k;

        for (auto i : data.keys())
        {
            x_h = log10(data[i][0]) - center_x;
            y_k = data[i][1]-center_y;
            if ((x_h*x_h) / (semi_x_axis*semi_x_axis) + (y_k*y_k) / (semi_y_axis*semi_y_axis) <= 1)
            {
                addSelectionPoint(i);
            }
        };
    }
    else if ((xscale == "lin") & (yscale == "log"))
    {
        double center_y = (log10(top_left.y()) + log10(bot_right.y()))/2;
        double center_x = (top_left.x() + bot_right.x())/2;
        double semi_x_axis = qAbs(top_left.x()-bot_right.x())/2.0;
        double semi_y_axis = qAbs(log10(top_left.y())-log10(bot_right.y()))/2.0;
        double x_h;
        double y_k;

        for (auto i : data.keys())
        {
            x_h = data[i][0] - center_x;
            y_k = log10(data[i][1]) - center_y;
            if ((x_h*x_h) / (semi_x_axis*semi_x_axis) + (y_k*y_k) / (semi_y_axis*semi_y_axis) <= 1)
            {
                addSelectionPoint(i);
            }
        };
    }
    else if ((xscale == "lin") & (yscale == "lin"))
    {
        double center_y = (top_left.y() + bot_right.y())/2;
        double center_x = (top_left.x() + bot_right.x())/2;
        double semi_x_axis = qAbs(top_left.x()-bot_right.x())/2.0;
        double semi_y_axis = qAbs(top_left.y()-bot_right.y())/2.0;
        double x_h;
        double y_k;

        for (auto i : data.keys())
        {
            x_h = data[i][0] - center_x;
            y_k = data[i][1] - center_y;
            if ((x_h*x_h) / (semi_x_axis*semi_x_axis) + (y_k*y_k) / (semi_y_axis*semi_y_axis) <= 1)
            {
                addSelectionPoint(i);
            }
        };
    }
    else if ((xscale == "log") & (yscale == "log"))
    {
        double center_y = (log10(top_left.y()) + log10(bot_right.y()))/2;
        double center_x = (log10(top_left.x()) + log10(bot_right.x()))/2;
        double semi_x_axis = qAbs(log10(top_left.x())-log10(bot_right.x()))/2.0;
        double semi_y_axis = qAbs(log10(top_left.y())-log10(bot_right.y()))/2.0;
        double x_h;
        double y_k;

        for (auto i : data.keys())
        {
            x_h = log10(data[i][0]) - center_x;
            y_k = log10(data[i][1]) - center_y;
            if ((x_h*x_h) / (semi_x_axis*semi_x_axis) + (y_k*y_k) / (semi_y_axis*semi_y_axis) <= 1)
            {
                addSelectionPoint(i);
            }
        };
    }
}

void CustomPointSelection::pointsInPoly(QMap<int, QVector<double>> data, QString xscale, QString yscale)
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

    if ((xscale == "log") & (yscale == "lin"))
    {
        for(auto i : data.keys())
        {

            int  cn = 0; // crossing number
            if ( (log10(x_min) < log10(data[i][0])) && (log10(data[i][0]) < log10(x_max)) && (y_min < data[i][1]) && (data[i][1] < y_max) )
            {
                for (int j=0; j<n-1; j++)
                {
                    if ( ((vert_y[j] <= data[i][1]) && (vert_y[j+1] > data[i][1]))     // upward crossing
                            || ((vert_y[j] > data[i][1]) && (vert_y[j+1] <=  data[i][1])) ) // downward crossing
                    {
                        double vt = (data[i][1]  - vert_y[j]) / (vert_y[j+1] - vert_y[j]);
                        if (log10(data[i][0]) <  log10(vert_x[j]) + vt * (log10(vert_x[j+1]) - log10(vert_x[j])))
                            cn ++;
                    }
                }
            }
            if (cn&1)
                addSelectionPoint(i);
        }
    }
    else if ((xscale == "lin") & (yscale == "log"))
    {
        for(auto i : data.keys())
        {

            int  cn = 0; // crossing number
            if ( (x_min < data[i][0]) && (data[i][0] < x_max) && (log10(y_min) < log10(data[i][1])) && (log10(data[i][1]) < log10(y_max)) )
            {
                for (int j=0; j<n-1; j++)
                {
                    if ( ((log10(vert_y[j]) <= log10(data[i][1])) && (log10(vert_y[j+1]) > log10(data[i][1])))     // upward crossing
                            || ((log10(vert_y[j]) > log10(data[i][1])) && (log10(vert_y[j+1]) <=  log10(data[i][1]))) ) // downward crossing
                    {
                        double vt = (log10(data[i][1])  - log10(vert_y[j])) / (log10(vert_y[j+1]) - log10(vert_y[j]));
                        if (data[i][0] <  vert_x[j] + vt * (vert_x[j+1] - vert_x[j]))
                            cn ++;
                    }
                }
            }
            if (cn&1)
                addSelectionPoint(i);
        }
    }
    else if ((xscale == "lin") & (yscale == "lin"))
    {
        for(auto i : data.keys())
        {

            int  cn = 0; // crossing number
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
    else if ((xscale == "log") & (yscale == "log"))
    {
        for(auto i : data.keys())
        {

            int  cn = 0; // crossing number
            if ( (log10(x_min) < log10(data[i][0])) && (log10(data[i][0]) < log10(x_max)) && (log10(y_min) < log10(data[i][1])) && (log10(data[i][1]) < log10(y_max)) )
            {
                for (int j=0; j<n-1; j++)
                {
                    if ( ((log10(vert_y[j]) <= log10(data[i][1])) && (log10(vert_y[j+1]) > log10(data[i][1])))     // upward crossing
                            || ((log10(vert_y[j]) > log10(data[i][1])) && (log10(vert_y[j+1]) <=  log10(data[i][1]))) ) // downward crossing
                    {
                        double vt = (log10(data[i][1])  - log10(vert_y[j])) / (log10(vert_y[j+1]) - log10(vert_y[j]));
                        if (log10(data[i][0]) <  log10(vert_x[j]) + vt * (log10(vert_x[j+1]) - log10(vert_x[j])))
                            cn ++;
                    }
                }
            }
            if (cn&1)
                addSelectionPoint(i);
        }
    }
}
