#include "plotwindow.h"

PlotWindow::PlotWindow(QWidget *parent, QList<QString> params, DatasetContainer *data, CustomPointSelection *select, int id, double size)
{
    selectionObj = select;
    setParameters(params);
    setData(data);
    setID(id);
    setScatterSize(size);

    /*** Plot UI setup ***/
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
    plot_widget->setNoAntialiasingOnDrag(true);
    plot_widget->xAxis->setBasePen(QPen(Qt::gray));
    plot_widget->xAxis->setLabelColor(Qt::gray);
    plot_widget->xAxis->setTickPen(QPen(Qt::gray));
    plot_widget->xAxis->setSubTickPen(QPen(Qt::gray));
    plot_widget->yAxis->setBasePen(QPen(Qt::gray));
    plot_widget->yAxis->setLabelColor(Qt::gray);
    plot_widget->yAxis->setTickPen(QPen(Qt::gray));
    plot_widget->yAxis->setSubTickPen(QPen(Qt::gray));

    poly_closed = new QCPItemEllipse(getPlot());
    poly_closed->setPen(Qt::NoPen);
    poly_closed->setLayer("selectionLayer");

    button_box = new QGroupBox(plot_box);
    QGridLayout *button_layout = new QGridLayout;

    btn_zoom = new QPushButton("Zoom",button_box);
    tool_buttons.append(btn_zoom);
    btn_zoom->setCheckable(true);
    btn_navigate = new QPushButton("Navigate",button_box);
    tool_buttons.append(btn_navigate);
    btn_navigate->setCheckable(true);
    btn_ellipse = new QPushButton("Ellipse",button_box);
    tool_buttons.append(btn_ellipse);
    btn_ellipse->setCheckable(true);
    btn_free_form = new QPushButton("Free form",button_box);
    tool_buttons.append(btn_free_form);
    btn_free_form->setCheckable(true);
    btn_reset = new QPushButton("Reset", button_box);
    btn_logx = new QCheckBox("x log scale", button_box);
    btn_logy = new QCheckBox("y log scale", button_box);

    cbox_x = new QComboBox(button_box);
    cbox_y = new QComboBox(button_box);
    cbox_x->setMinimumSize(40,5);
    cbox_y->setMinimumSize(40,5);

    cbox_x->clear();
    cbox_y->clear();
    for (int i=0; i<parameters.length(); i++)
    {
        cbox_x->addItem(parameters[i]);
        cbox_x->setItemData(i, parameters[i], Qt::ToolTipRole);
        cbox_y->addItem(parameters[i]);
        cbox_y->setItemData(i, parameters[i], Qt::ToolTipRole);
    }
    cbox_x->setCurrentIndex(0);
    cbox_y->setCurrentIndex(1);

    cbox_x->setToolTip(cbox_x->currentText());
    cbox_y->setToolTip(cbox_y->currentText());

    setDataFromParam(cbox_x->currentIndex(),cbox_y->currentIndex());

    QCPScatterStyle style;
    style.setShape(QCPScatterStyle::ssDisc);
    style.setSize(scatter_size);
    style.setPen(QPen(Qt::black));

    plot(style, 0, data_dic);

    button_layout->addWidget(btn_zoom,0,0,2,1);
    button_layout->addWidget(btn_navigate,0,1,2,1);
    button_layout->addWidget(btn_ellipse,0,2,2,1);
    button_layout->addWidget(btn_free_form,0,3,2,1);
    button_layout->addWidget(btn_reset, 0,4,2,1);
    button_layout->addWidget(cbox_x,0,5);
    button_layout->addWidget(cbox_y,1,5);
    button_layout->addWidget(btn_logx,0,6);
    button_layout->addWidget(btn_logy,1,6);
    button_layout->setSizeConstraint(QLayout::SetMaximumSize);

    button_box->setLayout(button_layout);

    layout->addWidget(plot_widget);
    layout->addWidget(button_box, 0);
    plot_box->setLayout(layout);

    close_btn = new QPushButton("x",plot_box);
    close_btn->setGeometry(-2,-2,19,18);
    QRect rect(-1,-1,20,20);
    QRegion region(rect, QRegion::Ellipse);
    //close_btn->setMask(region);
    close_btn->raise();
    close_btn->setStyleSheet("QPushButton{background-color:rgb(200,200,200); padding-bottom:0px; padding-left:2px; font-size:13px; border: none;} QPushButton:hover{ background-color:rgb(232,17,35); color:white; }");

    /*** buttons connections ***/
    connect(close_btn, SIGNAL(released()), SLOT(close_window()));
    connect(btn_zoom, SIGNAL(clicked()), SLOT(on_btn_zoom_clicked()));
    connect(btn_navigate, SIGNAL(clicked()), SLOT(on_btn_navigate_clicked()));
    connect(btn_ellipse, SIGNAL(clicked()), SLOT(on_btn_ellipse_clicked()));
    connect(btn_free_form, SIGNAL(clicked()), SLOT(on_btn_free_form_clicked()));
    connect(btn_reset, SIGNAL(clicked()), SLOT(on_btn_reset_clicked()));

    connect(cbox_x, SIGNAL(activated(int)), SLOT(on_cbox_x_activated()));
    connect(cbox_y, SIGNAL(activated(int)), SLOT(on_cbox_y_activated()));

    connect(getPlot(), SIGNAL(mousePress(QMouseEvent*)), SLOT(startEllipseSelection(QMouseEvent*)));
    connect(getPlot(), SIGNAL(mouseMove(QMouseEvent*)), SLOT(moveEllipseSelection(QMouseEvent*)));
    connect(getPlot(), SIGNAL(mouseRelease(QMouseEvent*)), SLOT(endEllipseSelection(QMouseEvent*)));

    connect(getPlot(), SIGNAL(mousePress(QMouseEvent*)), SLOT(startEndLine(QMouseEvent*)));
    connect(getPlot(), SIGNAL(mouseMove(QMouseEvent*)), SLOT(moveLine(QMouseEvent*)));

    connect(btn_logx, SIGNAL(stateChanged(int)), SLOT(axisScaleX(int)));
    connect(btn_logy, SIGNAL(stateChanged(int)), SLOT(axisScaleY(int)));

    connect(getPlot(), SIGNAL(mouseMove(QMouseEvent*)), SLOT(mouseOverAxis(QMouseEvent*)));

    connect(getPlot()->xAxis, SIGNAL(selectionChanged(QCPAxis::SelectableParts)), SLOT(xAxisSelect(QCPAxis::SelectableParts)));
    connect(getPlot()->yAxis, SIGNAL(selectionChanged(QCPAxis::SelectableParts)), SLOT(yAxisSelect(QCPAxis::SelectableParts)));

    connect(getPlot(), SIGNAL(mouseWheel(QWheelEvent*)), SLOT(moveAxisDragging(QWheelEvent*)));
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
    emit deleted(getID());
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
    MatrixXd data_mat = data_container->getDataMat();
    int l = data_mat.rows();
    for (int i=0; i<l; i++)
    {
        //addPoint(i, data[i][x_param], data[i][y_param]);

        addPoint(i, data_mat(i,x_param), data_mat(i,y_param));
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

void PlotWindow::setData(DatasetContainer *d)
{
    data_container = d;
}


void PlotWindow::on_btn_zoom_clicked()
{
    for (const auto& b : tool_buttons)
        b->setChecked(false);

    /* disable range dragging */
    if (xAxis_selected)
        getPlot()->xAxis->setSelectedParts(QCPAxis::spNone);
    if (yAxis_selected)
        getPlot()->yAxis->setSelectedParts(QCPAxis::spNone);

    /* change cursor */
    current_cursor = Qt::CrossCursor;
    getPlot()->setCursor(current_cursor);

    /* disable ellipse and free form selection */
    ellipse_select = false;
    free_form_select = false;
    lActive = false;
    while(!polygon.isEmpty())
        getPlot()->removeItem(polygon.takeFirst());

    if (ellipse != nullptr)
    {
        ellipse->topLeft->setCoords(0,0);
        ellipse->bottomRight->setCoords(0,0);
        selectionObj->clearSelectionPoints();
    }

    /* set zoom interaction */
    getPlot()->setInteraction(QCP::iRangeDrag, false);
    getPlot()->setInteraction(QCP::iRangeZoom);
    getPlot()->setSelectionRectMode(QCP::srmZoom);
    getPlot()->replot();
    btn_zoom->setChecked(true);
}

void PlotWindow::on_btn_navigate_clicked()
{
    for (const auto& b : tool_buttons)
        b->setChecked(false);

    /* disable range dragging */
    if (xAxis_selected)
        getPlot()->xAxis->setSelectedParts(QCPAxis::spNone);
    if (yAxis_selected)
        getPlot()->yAxis->setSelectedParts(QCPAxis::spNone);

    /* change cursor */
    current_cursor = Qt::SizeAllCursor;
    getPlot()->setCursor(current_cursor);

    /* disable ellipse and free form selection */
    ellipse_select = false;
    free_form_select = false;
    lActive = false;
    while(!polygon.isEmpty())
        getPlot()->removeItem(polygon.takeFirst());
    selectionObj->clearSelectionPoints();

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
    getPlot()->replot();
    btn_navigate->setChecked(true);
}

void PlotWindow::on_btn_ellipse_clicked()
{
    for (const auto& b : tool_buttons)
        b->setChecked(false);

    /* disable range dragging */
    if (xAxis_selected)
        getPlot()->xAxis->setSelectedParts(QCPAxis::spNone);
    if (yAxis_selected)
        getPlot()->yAxis->setSelectedParts(QCPAxis::spNone);

    /* change cursor */
    current_cursor = Qt::ArrowCursor;
    getPlot()->setCursor(current_cursor);

    /* reset ellipse when clicking on button and disable free form selection*/
    if (ellipse != nullptr)
    {
        ellipse->topLeft->setCoords(0,0);
        ellipse->bottomRight->setCoords(0,0);

    /* if first time clicking on button then instanciate ellipse */
    } else {
        ellipse = new QCPItemEllipse(getPlot());
        ellipse->setLayer("selectionLayer");
        ellipse->topLeft->setCoords(0,0);
        ellipse->bottomRight->setCoords(0,0);
    }

    ellipse_select = true;
    free_form_select = false;
    lActive = false;
    while(!polygon.isEmpty())
        getPlot()->removeItem(polygon.takeFirst());

    selectionObj->clearSelectionPoints();
    getPlot()->graph(1)->data()->clear();
    ellipse->setPen(QPen(QBrush(Qt::red), 1.5, Qt::DashLine));
    getPlot()->setInteraction(QCP::iRangeDrag, false);
    getPlot()->setInteraction(QCP::iRangeZoom, false);
    getPlot()->setSelectionRectMode(QCP::srmNone);
    plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, scatter_size), 0, data_dic);
    btn_ellipse->setChecked(true);
}

void PlotWindow::on_btn_free_form_clicked()
{
    for (const auto& b : tool_buttons)
        b->setChecked(false);

    current_cursor = Qt::ArrowCursor;
    getPlot()->setCursor(current_cursor);
    free_form_select = true;
    ellipse_select = false;
    lActive = false;

    if (ellipse != nullptr)
    {
        ellipse->topLeft->setCoords(0, 0);
        ellipse->bottomRight->setCoords(0,0);
    }

    while(!polygon.isEmpty())
        getPlot()->removeItem(polygon.takeFirst());

    selectionObj->clearSelectionPoints();
    getPlot()->graph(1)->data()->clear();
    plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, scatter_size), 0, data_dic);

    getPlot()->setInteraction(QCP::iRangeDrag, false);
    getPlot()->setInteraction(QCP::iRangeZoom, false);
    getPlot()->setSelectionRectMode(QCP::srmNone);
    getPlot()->replot();
    btn_free_form->setChecked(true);
}

void PlotWindow::on_btn_reset_clicked()
{
    getPlot()->xAxis->setRange(0,1);
    getPlot()->yAxis->setRange(0,1);
    getPlot()->graph(0)->rescaleAxes(true);
    getPlot()->replot();
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
        selectionObj->pointsInEllipse(ellipse, data_dic, xscale, yscale);

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
        line->setPen(QPen(QBrush(Qt::red), 1.5, Qt::DashLine));
        line->start->setCoords(x_pos, y_pos);
        line->end->setCoords(x_pos, y_pos);
        line->setLayer("selectionLayer");
        getPlot()->replot();

        polygon.append(line);
        poly_closed->setPen(Qt::NoPen);

        /* circle that when clicked on, free selection is ended and points selected revealed */
        if ((xscale == "lin") & (yscale == "lin"))
        {
            radius_x = qAbs((getPlot()->xAxis->range().upper - getPlot()->xAxis->range().lower)/plot_widget->size().width()*5);
            radius_y = (getPlot()->yAxis->range().upper - getPlot()->yAxis->range().lower)/plot_widget->size().height()*5;
        }
        else if ((xscale == "log") & (yscale == "log"))
        {
            radius_x = qAbs((log10(getPlot()->xAxis->range().upper) - log10(getPlot()->xAxis->range().lower))/plot_widget->size().width()*10*x_pos);
            radius_y = qAbs((log10(getPlot()->yAxis->range().upper) - log10(getPlot()->yAxis->range().lower))/plot_widget->size().height()*10*y_pos);
        }
        else if ((xscale == "lin") & (yscale == "log"))
        {
            radius_x = qAbs((getPlot()->xAxis->range().upper - getPlot()->xAxis->range().lower)/plot_widget->size().width()*5);
            radius_y = qAbs((log10(getPlot()->yAxis->range().upper) - log10(getPlot()->yAxis->range().lower))/plot_widget->size().height()*10*y_pos);
        }
        else if ((xscale == "log") & (yscale == "lin"))
        {
            radius_x = qAbs((log10(getPlot()->xAxis->range().upper) - log10(getPlot()->xAxis->range().lower))/plot_widget->size().width()*10*x_pos);
            radius_y = qAbs((getPlot()->yAxis->range().upper - getPlot()->yAxis->range().lower)/plot_widget->size().height()*5);
        }
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
    } else if ( ( ((x_pos-start_v_x)*(x_pos-start_v_x) / (radius_x*radius_x) + (y_pos-start_v_y)*(y_pos-start_v_y) / (radius_y*radius_y)) <= 1) && free_form_select && lActive)
    {
        line->end->setCoords(start_v_x, start_v_y);
        selectionObj->addVertex(start_v_x, start_v_y);
        lActive = false;
        poly_closed->setBrush(Qt::NoBrush);
//        int t = clock();
        selectionObj->pointsInPoly(data_dic, xscale, yscale);

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
        line->setPen(QPen(QBrush(Qt::red), 1.5, Qt::DashLine));
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
    getPlot()->graph(1)->data()->clear();
    setDataFromParam(cbox_x->currentIndex(),cbox_y->currentIndex());
    cbox_x->setToolTip(cbox_x->currentText());
    QCPScatterStyle style;
    style.setShape(QCPScatterStyle::ssDisc);
    style.setSize(scatter_size);
    style.setPen(QPen(Qt::black));

    QMap<int, QVector<double>> plot_data = removeNonUnique(getData(),selectionObj->getSelectionPoints());
    plot(style, 0, plot_data);
    plot_values(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::red, scatter_size),1,selectionObj->getSelectionPoints());
    getPlot()->graph(0)->rescaleAxes(true);
    getPlot()->replot();
}

void PlotWindow::on_cbox_y_activated()
{
    getPlot()->graph(1)->data()->clear();
    setDataFromParam(cbox_x->currentIndex(),cbox_y->currentIndex());
    cbox_y->setToolTip(cbox_y->currentText());
    QCPScatterStyle style;
    style.setShape(QCPScatterStyle::ssDisc);
    style.setSize(scatter_size);
    style.setPen(QPen(Qt::black));

    QMap<int, QVector<double>> plot_data = removeNonUnique(getData(),selectionObj->getSelectionPoints());
    plot(style, 0, plot_data);
    plot_values(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::red, scatter_size),1,selectionObj->getSelectionPoints());
    getPlot()->graph(0)->rescaleAxes(true);
    getPlot()->replot();
}


QMap<int, QVector<double>> PlotWindow::removeNonUnique(QMap<int, QVector<double>> map1, QList<int> keys)
{
    QMap<int, QVector<double>> unique_map = map1;
    for (auto k: keys)
    {
        if(map1.contains(k))
            unique_map.remove(k);
    }
    return unique_map;
}


void PlotWindow::axisScaleX(int s)
{
    if (s==2)
    {
        QSharedPointer<QCPAxisTickerLog> xlogTicker(new QCPAxisTickerLog);
        getPlot()->xAxis->setScaleType(QCPAxis::stLogarithmic);
        getPlot()->xAxis->setTicker(xlogTicker);
        getPlot()->xAxis->setNumberFormat("eb");
        getPlot()->xAxis->setNumberPrecision(0);
        getPlot()->replot();
        xscale = "log";
    } else if (s==0)
    {
        QSharedPointer<QCPAxisTickerFixed> xfixedTicker(new QCPAxisTickerFixed);
        getPlot()->xAxis->setScaleType(QCPAxis::stLinear);
        getPlot()->xAxis->setTicker(xfixedTicker);
        xfixedTicker->setTickStep(1.0); // tick step shall be 1.0
        xfixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples); // and no scaling of the tickstep (like multiples or powers) is allowed
        getPlot()->xAxis->setNumberFormat("f");
        xscale = "lin";
        getPlot()->replot();
    }
}

void PlotWindow::axisScaleY(int s)
{
    if (s==2)
    {
        QSharedPointer<QCPAxisTickerLog> ylogTicker(new QCPAxisTickerLog);
        getPlot()->yAxis->setScaleType(QCPAxis::stLogarithmic);
        getPlot()->yAxis->setTicker(ylogTicker);
        getPlot()->yAxis->setNumberFormat("eb");
        getPlot()->yAxis->setNumberPrecision(0);
        getPlot()->replot();
        yscale = "log";
    } else if (s==0)
    {
        QSharedPointer<QCPAxisTickerFixed> yfixedTicker(new QCPAxisTickerFixed);
        getPlot()->yAxis->setScaleType(QCPAxis::stLinear);
        getPlot()->yAxis->setTicker(yfixedTicker);
        yfixedTicker->setTickStep(1.0); // tick step shall be 1.0
        yfixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples); // and no scaling of the tickstep (like multiples or powers) is allowed
        getPlot()->yAxis->setNumberFormat("f");
        yscale = "lin";
        getPlot()->replot();
    }
}

void PlotWindow::mouseOverAxis(QMouseEvent *event)
{
    double x_range = (getPlot()->xAxis->range().upper - getPlot()->xAxis->range().lower)/100;
    double y_range = (getPlot()->yAxis->range().upper - getPlot()->yAxis->range().lower)/100;
    if ((xscale == "lin") & (yscale == "log"))
    {
        y_range = (log10(getPlot()->yAxis->range().upper) - log10(getPlot()->yAxis->range().lower))*getPlot()->yAxis->range().lower/100;
    }
    else if ((xscale == "log") & (yscale == "lin"))
    {
        x_range = (log10(getPlot()->xAxis->range().upper) - log10(getPlot()->xAxis->range().lower))*getPlot()->xAxis->range().lower/100;
    }
    else if ((xscale == "log") & (yscale == "log"))
    {
        x_range = (log10(getPlot()->xAxis->range().upper) - log10(getPlot()->xAxis->range().lower))*getPlot()->xAxis->range().lower/100;
        y_range = (log10(getPlot()->yAxis->range().upper) - log10(getPlot()->yAxis->range().lower))*getPlot()->yAxis->range().lower/100;
    }

    //qDebug()<<x_range << y_range;

    double x = getPlot()->xAxis->pixelToCoord(event->pos().x());
    double y = getPlot()->yAxis->pixelToCoord(event->pos().y());
    if ((y < getPlot()->yAxis->range().lower+y_range)
            & (y > getPlot()->yAxis->range().lower-y_range) & !xAxis_selected)
    {
        getPlot()->setCursor(Qt::PointingHandCursor);
    } else if ((x < getPlot()->xAxis->range().lower+x_range)
               & (x > getPlot()->xAxis->range().lower-x_range) & !yAxis_selected)
    {
        getPlot()->setCursor(Qt::PointingHandCursor);
    } else {
        getPlot()->setCursor(current_cursor);
    }
}

void PlotWindow::xAxisSelect(QCPAxis::SelectableParts)
{
    for (const auto& b : tool_buttons)
        b->setChecked(false);
    xAxis_selected = !xAxis_selected;
    if (xAxis_selected){

        current_cursor = Qt::SplitHCursor;
        getPlot()->setCursor(current_cursor);

        /* disable ellipse and free form selection */
        ellipse_select = false;
        free_form_select = false;
        while(!polygon.isEmpty())
            getPlot()->removeItem(polygon.takeFirst());

        getPlot()->setInteraction(QCP::iRangeDrag, false);
        getPlot()->setInteraction(QCP::iRangeZoom, false);
        getPlot()->setSelectionRectMode(QCP::srmNone);
    } else if (!xAxis_selected)
    {
        current_cursor = Qt::ArrowCursor;
    }
}

void PlotWindow::yAxisSelect(QCPAxis::SelectableParts)
{
    for (const auto& b : tool_buttons)
        b->setChecked(false);
    yAxis_selected = !yAxis_selected;
    if (yAxis_selected){

        current_cursor = Qt::SplitVCursor;
        getPlot()->setCursor(current_cursor);

        /* disable ellipse and free form selection */
        ellipse_select = false;
        free_form_select = false;
        while(!polygon.isEmpty())
            getPlot()->removeItem(polygon.takeFirst());

        getPlot()->setInteraction(QCP::iRangeDrag, false);
        getPlot()->setInteraction(QCP::iRangeZoom, false);
        getPlot()->setSelectionRectMode(QCP::srmNone);
    } else if (!yAxis_selected)
    {
        current_cursor = Qt::ArrowCursor;
    }
}

void PlotWindow::moveAxisDragging(QWheelEvent *event)
{
    if (xAxis_selected)
    {

        if (xscale == "lin")
        {
            int pttt = event->angleDelta().y();
            double lower = getPlot()->xAxis->range().lower;
            double upper = getPlot()->xAxis->range().upper;
            getPlot()->xAxis->setRange(lower-pttt*(upper-lower)/1000, upper+pttt*(upper-lower)/1000);
            getPlot()->replot();
        } else if (xscale == "log")
        {
            int pttt = event->angleDelta().y();
            double lower = getPlot()->xAxis->range().lower;
            double upper = getPlot()->xAxis->range().upper;
            getPlot()->xAxis->setRange(lower-pttt*(log10(upper)-log10(lower))/100, upper+pttt*(log10(upper)-log10(lower))/100);
            getPlot()->replot();
        }
    } else if (yAxis_selected)
    {
        if (yscale == "lin")
        {
            int pttt = event->angleDelta().y();
            double lower = getPlot()->yAxis->range().lower;
            double upper = getPlot()->yAxis->range().upper;
            getPlot()->yAxis->setRange(lower-pttt*(upper-lower)/1000, upper+pttt*(upper-lower)/1000);
            getPlot()->replot();
        } else if (yscale == "log")
        {
            int pttt = event->angleDelta().y();
            double lower = getPlot()->yAxis->range().lower;
            double upper = getPlot()->yAxis->range().upper;
            getPlot()->yAxis->setRange(lower-pttt*(log10(upper)-log10(lower))/100, upper+pttt*(log10(upper)-log10(lower))/100);
            getPlot()->replot();
        }
    }
}
