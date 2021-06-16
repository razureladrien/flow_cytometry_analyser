#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <time.h>
#include <fstream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->plot->addGraph();
    ui->plot->addGraph();
    ui->plot->setInteraction(QCP::iSelectAxes);
    ui->plot->xAxis->setSelectableParts(QCPAxis::spAxis);
    ui->plot->yAxis->setSelectableParts(QCPAxis::spAxis);
    ui->plot->addLayer("selectionLayer", 0, QCustomPlot::limAbove);
    ui->plot->layer("selectionLayer")->setMode(QCPLayer::lmBuffered);

    selectionObj = new CustomPointSelection();
    poly_closed = new QCPItemEllipse(ui->plot);
    poly_closed->setPen(Qt::NoPen);
    poly_closed->setLayer("selectionLayer");

    /* buttons onclicked connections */
    // handled by default. If name of the method is "on_pushButton_clicked()"
    //, then it is auto-connected to the button

    /* ellipse selection mouse event connections */
    connect(ui->plot, SIGNAL(mousePress(QMouseEvent*)), SLOT(startEllipseSelection(QMouseEvent*)));
    connect(ui->plot, SIGNAL(mouseMove(QMouseEvent*)), SLOT(moveEllipseSelection(QMouseEvent*)));
    connect(ui->plot, SIGNAL(mouseRelease(QMouseEvent*)), SLOT(endEllipseSelection(QMouseEvent*)));

    /* line drawing */
    connect(ui->plot, SIGNAL(mousePress(QMouseEvent*)), SLOT(startEndLine(QMouseEvent*)));
    connect(ui->plot, SIGNAL(mouseMove(QMouseEvent*)), SLOT(moveLine(QMouseEvent*)));

    // axis scale change
    connect(ui->plot, SIGNAL(mouseDoubleClick(QMouseEvent*)), SLOT(axisScale(QMouseEvent*)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addPoint(double x, double y)
{
    qv_x.append(x);
    qv_y.append(y);
}

void MainWindow::clearData()
{
    qv_x.clear();
    qv_y.clear();
}

/* range of the global plot (not used yet) */
QVector<double> MainWindow::getRange()
{
    double min_x, min_y, max_x, max_y;
    QVector<double> res = QVector<double>();

    min_x = *std::min_element(qv_x.constBegin(), qv_x.constEnd());
    max_x = *std::max_element(qv_x.constBegin(), qv_x.constEnd());
    min_y = *std::min_element(qv_y.constBegin(), qv_y.constEnd());
    max_y = *std::max_element(qv_y.constBegin(), qv_y.constEnd());

    res.append(min_x);
    res.append(max_x);
    res.append(min_y);
    res.append(max_y);

    return res;
}




/* scatter plot function to plot [x,y] on the graph graph_id */
void MainWindow::plot(QCPScatterStyle scatterStyle, double graph_id, QVector<double> x, QVector<double> y)
{
    ui->plot->graph(graph_id)->setData(x, y);

    /* scale the axis for the first display */
    if (rescale_flag)
    {
        ui->plot->graph(graph_id)->rescaleAxes(true);
        rescale_flag = false;
        qv_x_to_plot = qv_x;
        qv_y_to_plot = qv_y;
    }
    ui->plot->graph(graph_id)->setScatterStyle(scatterStyle);
    ui->plot->graph(graph_id)->setLineStyle(QCPGraph::lsNone);
    ui->plot->replot();
    ui->plot->update();

}

void MainWindow::setDataFromParam(int x_param, int y_param)
{
    clearData();
    for (int i=0; i<number_of_events; i++)
    {
        addPoint(data[i][x_param], data[i][y_param]);
    }
}




void MainWindow::on_btn_zoom_clicked()
{
    /* disable ellipse and free form selection */
    ellipse_select = false;
    free_form_select = false;
    while(!polygon.isEmpty())
        ui->plot->removeItem(polygon.takeFirst());

    if (ellipse != nullptr)
    {
        ellipse->topLeft->setCoords(0,0);
        ellipse->bottomRight->setCoords(0,0);
        selectionObj->clearSelectionPoints();
        plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 0, getX(),getY());
        plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 1, selectionObj->getSelectionPoints()[0],selectionObj->getSelectionPoints()[1]);
    }

    /* set zoom interaction */
    ui->plot->setInteraction(QCP::iRangeDrag, false);
    ui->plot->setInteraction(QCP::iRangeZoom);
    ui->plot->setSelectionRectMode(QCP::srmZoom);
}

void MainWindow::on_btn_navigate_clicked()
{
    /* disable ellipse and free form selection */
    ellipse_select = false;
    free_form_select = false;
    while(!polygon.isEmpty())
        ui->plot->removeItem(polygon.takeFirst());
    selectionObj->clearSelectionPoints();
    plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 0, getX(),getY());
    plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 1, selectionObj->getSelectionPoints()[0],selectionObj->getSelectionPoints()[1]);

    if (ellipse != nullptr)
    {
        ellipse->topLeft->setCoords(0, 0);
        ellipse->bottomRight->setCoords(0,0);
    }

    /* set naviguate interaction */
    ui->plot->setInteraction(QCP::iRangeDrag, true);
    ui->plot->setNoAntialiasingOnDrag(true);
    ui->plot->setInteraction(QCP::iRangeZoom);
    ui->plot->setSelectionRectMode(QCP::srmNone);
}

void MainWindow::on_btn_ellipse_clicked()
{
    /* reset ellipse when clicking on button and disable free form selection*/
    if (ellipse != nullptr)
    {
        ellipse->topLeft->setCoords(0,0);
        ellipse->bottomRight->setCoords(0,0);
        selectionObj->clearSelectionPoints();
        ui->plot->layer("selectionLayer")->replot();

    /* if first time clicking on button then instanciate ellipse */
    } else {
        ellipse = new QCPItemEllipse(ui->plot);
        ellipse->setLayer("selectionLayer");
    }

    ellipse_select = true;
    free_form_select = false;
    while(!polygon.isEmpty())
        ui->plot->removeItem(polygon.takeFirst());

    ellipse->setPen(QPen(QBrush(Qt::red), 0, Qt::DashLine));
    ui->plot->setInteraction(QCP::iRangeDrag, false);
    ui->plot->setInteraction(QCP::iRangeZoom, false);
    ui->plot->setSelectionRectMode(QCP::srmNone);
    plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 0, getX(),getY());
    plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 1, selectionObj->getSelectionPoints()[0],selectionObj->getSelectionPoints()[1]);
}

void MainWindow::on_btn_free_form_clicked()
{
    free_form_select = true;
    ellipse_select = false;

    if (ellipse != nullptr)
    {
        ellipse->topLeft->setCoords(0, 0);
        ellipse->bottomRight->setCoords(0,0);
    }

    while(!polygon.isEmpty())
        ui->plot->removeItem(polygon.takeFirst());

    selectionObj->clearSelectionPoints();
    plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 0, getX(),getY());
    plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 1, selectionObj->getSelectionPoints()[0],selectionObj->getSelectionPoints()[1]);

    ui->plot->setInteraction(QCP::iRangeDrag, false);
    ui->plot->setInteraction(QCP::iRangeZoom, false);
    ui->plot->setSelectionRectMode(QCP::srmNone);
}

void MainWindow::on_btn_resolution_clicked()
{
    resolution_flag = !resolution_flag;
    if (resolution_flag == true)
    {
        qv_x_to_plot = qv_x;
        qv_y_to_plot = qv_y;
    } else if (resolution_flag == false) {
        qv_x_to_plot = {};
        qv_y_to_plot = {};
        double r;
        for (int i=0; i<10000; i++)
        {
            r = rand() % qv_x.length();
            qv_x_to_plot.append(qv_x[r]);
            qv_y_to_plot.append(qv_y[r]);
        }
    }
    plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 0, qv_x_to_plot,qv_y_to_plot);
}

void MainWindow::startEllipseSelection(QMouseEvent *event)
{
    if (ellipse_select)
    {
    eActive = true;
    double x_pos = ui->plot->xAxis->pixelToCoord(event->pos().x());
    double y_pos = ui->plot->yAxis->pixelToCoord(event->pos().y());
    ellipse->topLeft->setCoords(x_pos, y_pos);
    ellipse->bottomRight->setCoords(x_pos, y_pos);
    }
}

void MainWindow::moveEllipseSelection(QMouseEvent *event)
{
    if (ellipse_select && eActive)
    {
        double x_pos = ui->plot->xAxis->pixelToCoord(event->pos().x());
        double y_pos = ui->plot->yAxis->pixelToCoord(event->pos().y());
        ellipse->bottomRight->setCoords(x_pos, y_pos);
        ui->plot->layer("selectionLayer")->replot();
    }
}

void MainWindow::endEllipseSelection(QMouseEvent *event)
{
    if (ellipse_select && eActive)
    {
        double x_pos = ui->plot->xAxis->pixelToCoord(event->pos().x());
        double y_pos = ui->plot->yAxis->pixelToCoord(event->pos().y());
        ellipse->bottomRight->setCoords(x_pos, y_pos);
        eActive = false;
        selectionObj->pointsInEllipse(ellipse, qv_x, qv_y);
        plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::gray, 1), 0, getX(),getY());
        plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 1, selectionObj->getSelectionPoints()[0],selectionObj->getSelectionPoints()[1]);
    }
}

void MainWindow::startEndLine(QMouseEvent *event)
{
    double x_pos = ui->plot->xAxis->pixelToCoord(event->pos().x());
    double y_pos = ui->plot->yAxis->pixelToCoord(event->pos().y());

    /* selection started*/
    if (free_form_select && !lActive)
    {
        /* clear all lines registered from the previous selection.
        It's a reset */
        selectionObj->clearVerteces();
        while(!polygon.isEmpty())
            ui->plot->removeItem(polygon.takeFirst());

        /* Create the first line and compute the little cicle around
        the first point so it makes the selection feature easier to close */

        line = new QCPItemLine(ui->plot);
        line->setPen(QPen(QBrush(Qt::red), 1, Qt::DashLine));
        line->start->setCoords(x_pos, y_pos);
        line->end->setCoords(x_pos, y_pos);
        line->setLayer("selectionLayer");
        ui->plot->replot();

        polygon.append(line);
        poly_closed->setPen(Qt::NoPen);

        radius_x = (ui->plot->xAxis->range().upper - ui->plot->xAxis->range().lower)/200;
        radius_y = (ui->plot->yAxis->range().upper - ui->plot->yAxis->range().lower)/130;
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
        selectionObj->pointsInPoly(qv_x, qv_y);
//        t = clock()-t;
//        qDebug() << t;
        plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::gray, 1), 0, getX(),getY());
        plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 1, selectionObj->getSelectionPoints()[0],selectionObj->getSelectionPoints()[1]);

    /* selection continuing */
    } else if (free_form_select && lActive){
        line = new QCPItemLine(ui->plot);
        line->setPen(QPen(QBrush(Qt::red), 1, Qt::DashLine));
        line->start->setCoords(x_pos, y_pos);
        line->end->setCoords(x_pos, y_pos);
        line->setLayer("selectionLayer");
        ui->plot->replot();

        polygon.append(line);
        selectionObj->addVertex(x_pos, y_pos);
    }
}

void MainWindow::moveLine(QMouseEvent *event)
{
    if (free_form_select && lActive)
    {
        double x_pos = ui->plot->xAxis->pixelToCoord(event->pos().x());
        double y_pos = ui->plot->yAxis->pixelToCoord(event->pos().y());
        line->end->setCoords(x_pos, y_pos);
       // ui->plot->replot();

        double x_c = selectionObj->getVerteces()[0][0];
        double y_c = selectionObj->getVerteces()[1][0];

        if ( (x_pos-x_c)*(x_pos-x_c) / (radius_x*radius_x) + (y_pos-y_c)*(y_pos-y_c) / (radius_y*radius_y) <= 1)
        {
            poly_closed->setBrush(Qt::red);
            ui->plot->layer("selectionLayer")->replot();
        } else {
            poly_closed->setBrush(Qt::NoBrush);
            ui->plot->layer("selectionLayer")->replot();
        }

    }
}

void MainWindow::axisScale(QMouseEvent *event)
{

    if (ui->plot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    {
        if (xscale == "lin")
        {
            QSharedPointer<QCPAxisTickerLog> xlogTicker(new QCPAxisTickerLog);
            ui->plot->xAxis->setScaleType(QCPAxis::stLogarithmic);
            ui->plot->xAxis->setTicker(xlogTicker);
            ui->plot->xAxis->setNumberFormat("eb");
            ui->plot->xAxis->setNumberPrecision(0);
            xscale = "log";
        } else if (xscale == "log")
        {
            QSharedPointer<QCPAxisTickerFixed> xfixedTicker(new QCPAxisTickerFixed);
            ui->plot->xAxis->setScaleType(QCPAxis::stLinear);
            ui->plot->xAxis->setTicker(xfixedTicker);
            xfixedTicker->setTickStep(1.0); // tick step shall be 1.0
            xfixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples); // and no scaling of the tickstep (like multiples or powers) is allowed
            ui->plot->xAxis->setNumberFormat("f");
            xscale = "lin";
        }
    }
    else if (ui->plot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    {
        if (yscale == "lin")
        {
            QSharedPointer<QCPAxisTickerLog> ylogTicker(new QCPAxisTickerLog);
            ui->plot->yAxis->setScaleType(QCPAxis::stLogarithmic);
            ui->plot->yAxis->setTicker(ylogTicker);
            ui->plot->yAxis->setNumberFormat("eb");
            ui->plot->yAxis->setNumberPrecision(0);
            yscale = "log";
        } else if (yscale == "log")
        {
            QSharedPointer<QCPAxisTickerFixed> yfixedTicker(new QCPAxisTickerFixed);
            ui->plot->yAxis->setScaleType(QCPAxis::stLinear);
            ui->plot->yAxis->setTicker(yfixedTicker);
            yfixedTicker->setTickStep(1.0); // tick step shall be 1.0
            yfixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples); // and no scaling of the tickstep (like multiples or powers) is allowed
            ui->plot->yAxis->setNumberFormat("f");
            yscale = "lin";
        }
    }
}


void MainWindow::parseFileHeader(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return;
    QByteArray line = file.read(100);
    QString stringLine = QString::fromStdString(line.constData());
    QStringList fields = stringLine.split(' ', QString::SkipEmptyParts);

    start_text = fields[1].toInt();
    end_text = fields[2].toInt();
    start_data = fields[3].toInt();
    end_data = fields[4].toInt();

    file.close();
}

void MainWindow::parseFileText(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return;

    file.seek(start_text);
    QString delimiter = QString::fromStdString(file.read(1).constData());
    QByteArray line = file.read(end_text-start_text);
    QString stringLine = QString::fromStdString(line.constData());
    QStringList fields = stringLine.split(delimiter, QString::SkipEmptyParts);

    parameters.clear();

    for (int i=0; i<fields.length(); i++)
    {
        QString tmp = fields[i];
        if (tmp[0] == "$" && tmp[1] == "P" && tmp.at(tmp.size()-1)=="N")
            parameters.append(fields[i+1]);
        else if (tmp == "$PAR")
            number_of_params = fields[i+1].toInt();
        else if (tmp == "$TOT")
            number_of_events = fields[i+1].toInt();
        else if (tmp == "$BYTEORD")
        {
            QStringList ord = fields[i+1].split(',', QString::SkipEmptyParts);
            byte_order.clear();
            for (int j=0; j<ord.length(); j++)
                byte_order.append(ord[j].toInt());
        }
        else if (tmp == "$DATATYPE")
            data_type = fields[i+1];
    }

    file.close();
    qDebug() << start_text << end_text << start_data << end_data << number_of_params << number_of_events << byte_order << data_type;
}


void MainWindow::parseFileData(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return;
    QVector<float> tmp_data;

    if (data_type == "I" && byte_order.length() == 2) // integer 16 bits
    {
        if (byte_order[0] > 1) // reverted order
        {
            //qDebug() << "revert";
            file.seek(start_data);
            while (file.pos() < end_data)
            {
                QByteArray arr = file.read(byte_order[0]);
                QByteArray b(arr.size(),0);
                std::copy(arr.crbegin(),arr.crend(),b.begin());
                quint16 int_value;
                memcpy(&int_value, b.constData(), byte_order[0]);
                float float_value = int_value;
                tmp_data.append(float_value);
            }
        } else if (byte_order[0] == 1) // normal order
        {
            file.seek(start_data);
            while (file.pos() < end_data)
            {
                QByteArray arr = file.read(byte_order[byte_order.length()-1]);
                quint16 int_value;
                memcpy(&int_value, arr.constData(), byte_order[byte_order.length()-1]);
                float float_value = int_value;
                tmp_data.append(float_value);
            }
        }


    } else if (data_type == "I" && byte_order.length()==4) // integer 32 bits
    {
        if (byte_order[0] > 1) // reverted order
        {
            //qDebug() << "revert";
            file.seek(start_data);
            while (file.pos() < end_data)
            {
                QByteArray arr = file.read(byte_order[0]);
                QByteArray b(arr.size(),0);
                std::copy(arr.crbegin(),arr.crend(),b.begin());
                quint32 int_value;
                memcpy(&int_value, b.constData(), byte_order[0]);
                float float_value = int_value;
                tmp_data.append(float_value);
            }
        } else if (byte_order[0] == 1) // normal order
        {
            file.seek(start_data);
            while (file.pos() < end_data)
            {
                QByteArray arr = file.read(byte_order[byte_order.length()-1]);
                quint32 int_value;
                memcpy(&int_value, arr.constData(), byte_order[byte_order.length()-1]);
                float float_value = int_value;
                tmp_data.append(float_value);
            }
        }


    } else if (data_type == "F") // float
    {
        if (byte_order[0] > 1) // reverted order
        {
            file.seek(start_data);
            while (file.pos() < end_data)
            {
                QByteArray arr = file.read(byte_order[0]);
                QByteArray b(arr.size(),0);
                std::copy(arr.crbegin(),arr.crend(),b.begin());
                float f;
                memcpy(&f, b.constData(), byte_order[0]);
                tmp_data.append(f);
            }
        } else if (byte_order[0] == 1) // normal order
        {
            file.seek(start_data);
            while (file.pos() < end_data)
            {
                QByteArray arr = file.read(byte_order[byte_order.length()-1]);
                float f;
                memcpy(&f, arr.constData(), byte_order[byte_order.length()-1]);
                tmp_data.append(f);
            }
        }
    }

    //qDebug() << tmp_data[0] << tmp_data[1]<< tmp_data[2]<< tmp_data[3];
    data = {};

    int iter = 0;
    for (int i=0; i<number_of_events; i++)
    {
        QVector<float> tmp_vector(number_of_params); // data are stored as float vector
        for (int j=0; j<number_of_params; j++)
        {
            tmp_vector[j]=tmp_data[iter];
            iter ++;
        }
        data.push_back(tmp_vector);
    }
    //qDebug() << data[0][0] << data[0][1]<< data[0][2]<< data[0][3];

    file.close();
}

void MainWindow::on_actionOpen_triggered()
{
    for (int p=0; p<plot_windows.length(); p++)
    {
        plot_windows[p]->close_window();
    }
    plot_windows.clear();

    QStringList file = QFileDialog::getOpenFileNames(this, tr("Open File"),"/flowData",tr("CSV/FCS Files (*.csv *.fcs)"));
    if (!file.empty())
    {
        QString fileName = file[0];
        parseFileHeader(fileName);
        parseFileText(fileName);
        parseFileData(fileName);

        ui->cbox_x->clear();
        ui->cbox_y->clear();
        for (int i=0; i<parameters.length(); i++)
        {
            ui->cbox_x->addItem(parameters[i]);
            ui->cbox_y->addItem(parameters[i]);
        }
        ui->cbox_x->setCurrentIndex(0);
        ui->cbox_y->setCurrentIndex(1);

        setDataFromParam(ui->cbox_x->currentIndex(),ui->cbox_y->currentIndex());

        //qDebug() << x[1];

        QCPScatterStyle style;
        style.setShape(QCPScatterStyle::ssDisc);
        style.setSize(1);
        style.setPen(QPen(Qt::black));
        rescale_flag = true;
        plot(style, 0, getX(), getY());
        qDebug() << plot_windows.length();
    }
}

void MainWindow::on_cbox_x_activated(const QString &arg1)
{
    setDataFromParam(ui->cbox_x->currentIndex(),ui->cbox_y->currentIndex());
    QCPScatterStyle style;
    style.setShape(QCPScatterStyle::ssDisc);
    style.setSize(1);
    style.setPen(QPen(Qt::black));

    plot(style, 0, getX(), getY());
    ui->plot->graph(0)->rescaleAxes(true);
}

void MainWindow::on_cbox_y_activated(const QString &arg1)
{
    setDataFromParam(ui->cbox_x->currentIndex(),ui->cbox_y->currentIndex());
    QCPScatterStyle style;
    style.setShape(QCPScatterStyle::ssDisc);
    style.setSize(1);
    style.setPen(QPen(Qt::black));

    plot(style, 0, getX(), getY());
    ui->plot->graph(0)->rescaleAxes(true);
}

void MainWindow::on_actionAdd_plot_triggered()
{
    qDebug() << plot_windows.length();
    number_of_plots ++;
    PlotWindow *p = new PlotWindow(this, parameters, data);
    if (number_of_plots < 3)
        ui->gridLayout->addWidget(p->getWindow(),0,number_of_plots);
    else if (number_of_plots >= 3)
        ui->gridLayout->addWidget(p->getWindow(),1,number_of_plots-3);
    plot_windows.append(p); // keep every plot in a list

    connect(p, SIGNAL(deleted()), SLOT(plot_deleted()));
}

void MainWindow::plot_deleted()
{
    number_of_plots --;
    plot_windows.pop_back();
}
