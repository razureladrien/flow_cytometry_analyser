#include "plotwindow.h"

PlotWindow::PlotWindow(QWidget *parent, QList<QString> params, QVector<QVector<float>> data)
{
    // enregistre toutes les information relatives au plot et
    // dans le mainwindow on va avoir la liste des plots qui sera saved

    setParameters(params);
    setData(data);
    plot_box = new QGroupBox(parent);
    QVBoxLayout *layout = new QVBoxLayout(plot_box);

    plot_widget = new QCustomPlot(plot_box);
    plot_widget->addGraph();
    plot_widget->addGraph();
    plot_widget->setInteraction(QCP::iSelectAxes);
    plot_widget->xAxis->setSelectableParts(QCPAxis::spAxis);
    plot_widget->yAxis->setSelectableParts(QCPAxis::spAxis);
    plot_widget->addLayer("selectionLayer", 0, QCustomPlot::limAbove);
    plot_widget->layer("selectionLayer")->setMode(QCPLayer::lmBuffered);

    selectionObj = new CustomPointSelection();
    poly_closed = new QCPItemEllipse(plot_widget);
    poly_closed->setPen(Qt::NoPen);
    poly_closed->setLayer("selectionLayer");

    button_box = new QGroupBox(plot_box);
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    btn_zoom = new QPushButton("Zoom",button_box);
    btn_navigate = new QPushButton("Navigate",button_box);
    btn_ellipse = new QPushButton("Ellipse",button_box);
    btn_free_form = new QPushButton("Free form",button_box);
    btn_resolution = new QPushButton("Res",button_box);

    cbox_x = new QComboBox(button_box);
    cbox_y = new QComboBox(button_box);

    cbox_x->clear();
    cbox_y->clear();
    for (int i=0; i<parameters.length(); i++)
    {
        cbox_x->addItem(parameters[i]);
        cbox_y->addItem(parameters[i]);
    }
    cbox_x->setCurrentIndex(0);
    cbox_y->setCurrentIndex(1);

    setDataFromParam(cbox_x->currentIndex(),cbox_y->currentIndex());

    QCPScatterStyle style;
    style.setShape(QCPScatterStyle::ssDisc);
    style.setSize(1);
    style.setPen(QPen(Qt::black));

    plot(style, 0, data_dic);

    buttonLayout->addWidget(btn_zoom);
    buttonLayout->addWidget(btn_navigate);
    buttonLayout->addWidget(btn_ellipse);
    buttonLayout->addWidget(btn_free_form);
    buttonLayout->addWidget(btn_resolution);
    buttonLayout->addWidget(cbox_x);
    buttonLayout->addWidget(cbox_y);
    buttonLayout->setSizeConstraint(QLayout::SetMaximumSize);

    button_box->setLayout(buttonLayout);

    layout->addWidget(plot_widget);
    layout->addWidget(button_box, 0);
    plot_box->setLayout(layout);

    close_btn = new QPushButton("x",plot_box);
    close_btn->setGeometry(0,0,22,22);
    QRect rect(1,1,19,19);
    QRegion region(rect, QRegion::Ellipse);
    close_btn->setMask(region);
    close_btn->raise();

    connect(close_btn, SIGNAL(released()), SLOT(close_window()));
    connect(btn_zoom, SIGNAL(clicked()), SLOT(on_btn_zoom_clicked()));
    connect(btn_navigate, SIGNAL(clicked()), SLOT(on_btn_navigate_clicked()));
    connect(btn_ellipse, SIGNAL(clicked()), SLOT(on_btn_ellipse_clicked()));
    connect(btn_free_form, SIGNAL(clicked()), SLOT(on_btn_free_form_clicked()));

    connect(cbox_x, SIGNAL(activated(int)), SLOT(on_cbox_x_activated()));
    connect(cbox_y, SIGNAL(activated(int)), SLOT(on_cbox_y_activated()));

    connect(getPlot(), SIGNAL(mousePress(QMouseEvent*)), SLOT(startEllipseSelection(QMouseEvent*)));
    connect(getPlot(), SIGNAL(mouseMove(QMouseEvent*)), SLOT(moveEllipseSelection(QMouseEvent*)));
    connect(getPlot(), SIGNAL(mouseRelease(QMouseEvent*)), SLOT(endEllipseSelection(QMouseEvent*)));

    connect(getPlot(), SIGNAL(mousePress(QMouseEvent*)), SLOT(startEndLine(QMouseEvent*)));
    connect(getPlot(), SIGNAL(mouseMove(QMouseEvent*)), SLOT(moveLine(QMouseEvent*)));
}

PlotWindow::~PlotWindow()
{
}

QGroupBox* PlotWindow::getWindow()
{
    return plot_box;
}

QCustomPlot* PlotWindow::getPlot()
{
    return plot_widget;
}

void PlotWindow::close_window()
{
    emit deleted();
    delete(plot_box);
    delete(this);
}

void PlotWindow::addPoint(int key, double x, double y)
{
    data_dic.insert(key, {x,y});
}

void PlotWindow::clearData()
{
    data_dic.clear();
}

void PlotWindow::setDataFromParam(int x_param, int y_param)
{
    clearData();
    for (int i=0; i<data.length(); i++)
    {
        addPoint(i, data[i][x_param], data[i][y_param]);
    }
}


/* scatter plot function to plot [x,y] on the graph graph_id */
/* input : QMap */
void PlotWindow::plot(QCPScatterStyle scatterStyle, double graph_id, QMap<int, QVector<double>> qv_data)//QVector<double> x, QVector<double> y)
{

    QVector<double> x,y;
    QList<QVector<double>> values = qv_data.values();

    for (int i=0; i<values.length(); i++)
    {
        x.append(values[i][0]);
        y.append(values[i][1]);
    }

    getPlot()->graph(graph_id)->setData(x, y);

    /* scale the axis for the first display */
    if (rescale_flag)
    {
        getPlot()->graph(graph_id)->rescaleAxes(true);
        rescale_flag = false;
    }
    getPlot()->graph(graph_id)->setScatterStyle(scatterStyle);
    getPlot()->graph(graph_id)->setLineStyle(QCPGraph::lsNone);
    getPlot()->replot();
    getPlot()->update();

}

/* scatter plot function to plot [x,y] on the graph graph_id */
/* input : QMap */
void PlotWindow::plot_values(QCPScatterStyle scatterStyle, double graph_id, QList<int> keys)//QVector<double> x, QVector<double> y)
{

    QVector<double> x,y;
    QList<QVector<double>> values = {};
    for (auto i : keys)
    {
        values.append(data_dic[i]);
    }

    for (int i=0; i<values.length(); i++)
    {
        x.append(values[i][0]);
        y.append(values[i][1]);
    }

    getPlot()->graph(graph_id)->setData(x, y);

    /* scale the axis for the first display */
    if (rescale_flag)
    {
        getPlot()->graph(graph_id)->rescaleAxes(true);
        rescale_flag = false;
    }
    getPlot()->graph(graph_id)->setScatterStyle(scatterStyle);
    getPlot()->graph(graph_id)->setLineStyle(QCPGraph::lsNone);
    getPlot()->replot();
    getPlot()->update();

}

void PlotWindow::setParameters(QList<QString> p)
{
    parameters = p;
}

void PlotWindow::setData(QVector<QVector<float>> d)
{
    data = d;
}


void PlotWindow::on_btn_zoom_clicked()
{
    getPlot()->setCursor(Qt::CrossCursor);
    /* disable ellipse and free form selection */
    ellipse_select = false;
    free_form_select = false;
    while(!polygon.isEmpty())
        getPlot()->removeItem(polygon.takeFirst());

    if (ellipse != nullptr)
    {
        ellipse->topLeft->setCoords(0,0);
        ellipse->bottomRight->setCoords(0,0);
        selectionObj->clearSelectionPoints();
        //plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 0, data_dic);
        //plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 1, selectionObj->getSelectionPoints()[0],selectionObj->getSelectionPoints()[1]);
    }

    /* set zoom interaction */
    getPlot()->setInteraction(QCP::iRangeDrag, false);
    getPlot()->setInteraction(QCP::iRangeZoom);
    getPlot()->setSelectionRectMode(QCP::srmZoom);
}

void PlotWindow::on_btn_navigate_clicked()
{
    getPlot()->setCursor(Qt::SizeAllCursor);
    /* disable ellipse and free form selection */
    ellipse_select = false;
    free_form_select = false;
    while(!polygon.isEmpty())
        getPlot()->removeItem(polygon.takeFirst());
    selectionObj->clearSelectionPoints();
    //plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 0, data_dic);
    //plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 1, selectionObj->getSelectionPoints()[0],selectionObj->getSelectionPoints()[1]);

    if (ellipse != nullptr)
    {
        ellipse->topLeft->setCoords(0, 0);
        ellipse->bottomRight->setCoords(0,0);
    }
    /* set naviguate interaction */
    getPlot()->setInteraction(QCP::iRangeDrag, true);
    getPlot()->setNoAntialiasingOnDrag(true);
    getPlot()->setInteraction(QCP::iRangeZoom);
    getPlot()->setSelectionRectMode(QCP::srmNone);
}

void PlotWindow::on_btn_ellipse_clicked()
{
    getPlot()->setCursor(Qt::ArrowCursor);
    /* reset ellipse when clicking on button and disable free form selection*/
    if (ellipse != nullptr)
    {
        ellipse->topLeft->setCoords(0,0);
        ellipse->bottomRight->setCoords(0,0);

    /* if first time clicking on button then instanciate ellipse */
    } else {
        ellipse = new QCPItemEllipse(getPlot());
        ellipse->setLayer("selectionLayer");
    }

    ellipse_select = true;
    free_form_select = false;
    while(!polygon.isEmpty())
        getPlot()->removeItem(polygon.takeFirst());

    selectionObj->clearSelectionPoints();
    getPlot()->graph(1)->data()->clear();
    ellipse->setPen(QPen(QBrush(Qt::red), 0, Qt::DashLine));
    getPlot()->setInteraction(QCP::iRangeDrag, false);
    getPlot()->setInteraction(QCP::iRangeZoom, false);
    getPlot()->setSelectionRectMode(QCP::srmNone);
    plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 0, data_dic);
}

void PlotWindow::on_btn_free_form_clicked()
{
    getPlot()->setCursor(Qt::ArrowCursor);
    free_form_select = true;
    ellipse_select = false;

    if (ellipse != nullptr)
    {
        ellipse->topLeft->setCoords(0, 0);
        ellipse->bottomRight->setCoords(0,0);
    }

    while(!polygon.isEmpty())
        getPlot()->removeItem(polygon.takeFirst());

    selectionObj->clearSelectionPoints();
    getPlot()->graph(1)->data()->clear();
    plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 0, data_dic);
    //plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 1, selectionObj->getSelectionPoints()[0],selectionObj->getSelectionPoints()[1]);

    getPlot()->setInteraction(QCP::iRangeDrag, false);
    getPlot()->setInteraction(QCP::iRangeZoom, false);
    getPlot()->setSelectionRectMode(QCP::srmNone);
}

void PlotWindow::startEllipseSelection(QMouseEvent *event)
{
    if (ellipse_select)
    {
    eActive = true;
    double x_pos = getPlot()->xAxis->pixelToCoord(event->pos().x());
    double y_pos = getPlot()->yAxis->pixelToCoord(event->pos().y());
    ellipse->topLeft->setCoords(x_pos, y_pos);
    ellipse->bottomRight->setCoords(x_pos, y_pos);
    }
}

void PlotWindow::moveEllipseSelection(QMouseEvent *event)
{
    if (ellipse_select && eActive)
    {
        double x_pos = getPlot()->xAxis->pixelToCoord(event->pos().x());
        double y_pos = getPlot()->yAxis->pixelToCoord(event->pos().y());
        ellipse->bottomRight->setCoords(x_pos, y_pos);
        getPlot()->layer("selectionLayer")->replot();
    }
}

void PlotWindow::endEllipseSelection(QMouseEvent *event)
{
    if (ellipse_select && eActive)
    {
        double x_pos = getPlot()->xAxis->pixelToCoord(event->pos().x());
        double y_pos = getPlot()->yAxis->pixelToCoord(event->pos().y());
        ellipse->bottomRight->setCoords(x_pos, y_pos);
        eActive = false;
        selectionObj->pointsInEllipse(ellipse, data_dic);

        if (ellipse != nullptr)
        {
            ellipse->topLeft->setCoords(0, 0);
            ellipse->bottomRight->setCoords(0,0);
        }

        emit ellipse_selection_closed(selectionObj->getSelectionPoints());
        //plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::gray, 1), 0, getX(),getY());
        //plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 1, selectionObj->getSelectionPoints()[0],selectionObj->getSelectionPoints()[1]);
    }
}

void PlotWindow::startEndLine(QMouseEvent *event)
{
    double x_pos = getPlot()->xAxis->pixelToCoord(event->pos().x());
    double y_pos = getPlot()->yAxis->pixelToCoord(event->pos().y());

    /* selection started*/
    if (free_form_select && !lActive)
    {
        /* clear all lines registered from the previous selection.
        It's a reset */
        selectionObj->clearVerteces();
        while(!polygon.isEmpty())
            getPlot()->removeItem(polygon.takeFirst());

        /* Create the first line and compute the little cicle around
        the first point so it makes the selection feature easier to close */

        line = new QCPItemLine(getPlot());
        line->setPen(QPen(QBrush(Qt::red), 1, Qt::DashLine));
        line->start->setCoords(x_pos, y_pos);
        line->end->setCoords(x_pos, y_pos);
        line->setLayer("selectionLayer");
        getPlot()->replot();

        polygon.append(line);
        poly_closed->setPen(Qt::NoPen);

        /* circle that when clicked on, free selection is ended and points selected revealed */
        radius_x = (getPlot()->xAxis->range().upper - getPlot()->xAxis->range().lower)/plot_widget->size().width()*5;
        radius_y = (getPlot()->yAxis->range().upper - getPlot()->yAxis->range().lower)/plot_widget->size().height()*5;
        qDebug() << radius_x << radius_y;
        qDebug() << plot_widget->size();
        double a = x_pos-radius_x;
        double b = y_pos+radius_y;
        double c = x_pos+radius_x;
        double d = y_pos-radius_y;
        start_v_x = x_pos;
        start_v_y = y_pos;

        poly_closed->topLeft->setCoords(a, b);
        poly_closed->bottomRight->setCoords(c,d);

        lActive = true;
        selectionObj->addVertex(x_pos, y_pos);
        started_line = true;

    /* selection ended */
    } else if ( ( ((x_pos-start_v_x)*(x_pos-start_v_x) / (radius_x*radius_x) + (y_pos-start_v_y)*(y_pos-start_v_y) / (radius_y*radius_y)) <= 1) && free_form_select && lActive) {
        line->end->setCoords(start_v_x, start_v_y);
        selectionObj->addVertex(start_v_x, start_v_y);
        lActive = false;
        poly_closed->setBrush(Qt::NoBrush);
//        int t = clock();
        selectionObj->pointsInPoly(data_dic);

        /* erase polygon on the plot */
        while(!polygon.isEmpty())
            getPlot()->removeItem(polygon.takeFirst());

        /* send signal to mainwindow */
        emit free_selection_closed(selectionObj->getSelectionPoints());
//        t = clock()-t;
//        qDebug() << t;
        //plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::gray, 1), 0, getX(),getY());
        //plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 1, selectionObj->getSelectionPoints()[0],selectionObj->getSelectionPoints()[1]);

    /* selection continuing */
    } else if (free_form_select && lActive){
        line = new QCPItemLine(getPlot());
        line->setPen(QPen(QBrush(Qt::red), 1, Qt::DashLine));
        line->start->setCoords(x_pos, y_pos);
        line->end->setCoords(x_pos, y_pos);
        line->setLayer("selectionLayer");
        getPlot()->replot();

        polygon.append(line);
        selectionObj->addVertex(x_pos, y_pos);
    }
}

void PlotWindow::moveLine(QMouseEvent *event)
{
    if (free_form_select && lActive)
    {
        double x_pos = getPlot()->xAxis->pixelToCoord(event->pos().x());
        double y_pos = getPlot()->yAxis->pixelToCoord(event->pos().y());
        line->end->setCoords(x_pos, y_pos);
       // ui->plot->replot();

        double x_c = selectionObj->getVerteces()[0][0];
        double y_c = selectionObj->getVerteces()[1][0];

        if ( (x_pos-x_c)*(x_pos-x_c) / (radius_x*radius_x) + (y_pos-y_c)*(y_pos-y_c) / (radius_y*radius_y) <= 1)
        {
            poly_closed->setBrush(Qt::red);
            getPlot()->layer("selectionLayer")->replot();
        } else {
            poly_closed->setBrush(Qt::NoBrush);
            getPlot()->layer("selectionLayer")->replot();
        }

    }
}

void PlotWindow::on_cbox_x_activated()
{
    setDataFromParam(cbox_x->currentIndex(),cbox_y->currentIndex());
    QCPScatterStyle style;
    style.setShape(QCPScatterStyle::ssDisc);
    style.setSize(1);
    style.setPen(QPen(Qt::black));

    plot(style, 0, data_dic);
    getPlot()->graph(0)->rescaleAxes(true);
    getPlot()->replot();
}

void PlotWindow::on_cbox_y_activated()
{
    setDataFromParam(cbox_x->currentIndex(),cbox_y->currentIndex());
    QCPScatterStyle style;
    style.setShape(QCPScatterStyle::ssDisc);
    style.setSize(1);
    style.setPen(QPen(Qt::black));

    plot(style, 0, data_dic);
    getPlot()->graph(0)->rescaleAxes(true);
    getPlot()->replot();
}
