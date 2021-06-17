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

    connect(cbox_x, SIGNAL(activated(int)), SLOT(on_cbox_x_activated()));
    connect(cbox_y, SIGNAL(activated(int)), SLOT(on_cbox_y_activated()));

    connect(getPlot(), SIGNAL(mousePress(QMouseEvent*)), SLOT(startEllipseSelection(QMouseEvent*)));
    connect(getPlot(), SIGNAL(mouseMove(QMouseEvent*)), SLOT(moveEllipseSelection(QMouseEvent*)));
    connect(getPlot(), SIGNAL(mouseRelease(QMouseEvent*)), SLOT(endEllipseSelection(QMouseEvent*)));
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
        plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 0, data_dic);
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
    plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 0, data_dic);
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
    /* reset ellipse when clicking on button and disable free form selection*/
    if (ellipse != nullptr)
    {
        ellipse->topLeft->setCoords(0,0);
        ellipse->bottomRight->setCoords(0,0);
        selectionObj->clearSelectionPoints();
        getPlot()->layer("selectionLayer")->replot();

    /* if first time clicking on button then instanciate ellipse */
    } else {
        ellipse = new QCPItemEllipse(getPlot());
        ellipse->setLayer("selectionLayer");
    }

    ellipse_select = true;
    free_form_select = false;
    while(!polygon.isEmpty())
        getPlot()->removeItem(polygon.takeFirst());

    ellipse->setPen(QPen(QBrush(Qt::red), 0, Qt::DashLine));
    getPlot()->setInteraction(QCP::iRangeDrag, false);
    getPlot()->setInteraction(QCP::iRangeZoom, false);
    getPlot()->setSelectionRectMode(QCP::srmNone);
    plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 0, data_dic);
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

        emit ellipse_selection_closed(selectionObj->getSelectionPoints());
        //plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::gray, 1), 0, getX(),getY());
        //plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 1, selectionObj->getSelectionPoints()[0],selectionObj->getSelectionPoints()[1]);
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
}
