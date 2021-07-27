#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <time.h>
#include <fstream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    on_actionAdd_plot_triggered();
}

MainWindow::~MainWindow()
{
    delete ui;
}


//void MainWindow::on_btn_resolution_clicked()
//{
//    resolution_flag = !resolution_flag;
//    if (resolution_flag == true)
//    {
//        qv_x_to_plot = qv_x;
//        qv_y_to_plot = qv_y;
//    } else if (resolution_flag == false) {
//        qv_x_to_plot = {};
//        qv_y_to_plot = {};
//        double r;
//        for (int i=0; i<10000; i++)
//        {
//            r = rand() % qv_x.length();
//            qv_x_to_plot.append(qv_x[r]);
//            qv_y_to_plot.append(qv_y[r]);
//        }
//    }
//    plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, 1), 0, qv_x_to_plot,qv_y_to_plot);
//}

/* parsing header of the FCS file
   returns bytes positions of meta data and data */
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

/* parsing "text data" to get metadata */
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
}

/* parsing data byte by byte */
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
    file.close();

    //qDebug() << data.length() << data[0].length();
}

/* opening file */
void MainWindow::on_actionOpen_triggered()
{
    QStringList file = QFileDialog::getOpenFileNames(this, tr("Open File"),"/flowData",tr("CSV/FCS Files (*.csv *.fcs)"));
    if (!file.empty())
    {
        for (auto id : plot_windows.keys())
            plot_windows[id]->close_window();
//        //clear all plots
//        while(!plot_windows.isEmpty())
//        {
//            plot_windows.last()->close_window();
//        }
        plot_queue = {5,4,3,2,1,0};

        // parse file
        QString fileName = file[0];
        parseFileHeader(fileName);
        parseFileText(fileName);
        parseFileData(fileName);

        on_actionAdd_plot_triggered();
    }
}

/* instanciate plot when action "Add plot" triggered */
void MainWindow::on_actionAdd_plot_triggered()
{
    if (number_of_plots == 6)
    {
        QErrorMessage* errorMessageDialog = new QErrorMessage(this);
        errorMessageDialog->showMessage("The number of plots cannot exceed 6");
    } else if (number_of_plots < 6) {
        int id = plot_queue.last();
        plot_queue.pop_back();
        PlotWindow *p = new PlotWindow(this, parameters, data, id, 2);

        if (id < 3)
            ui->gridLayout->addWidget(p->getWindow(),0,id);
        else if (id >= 3)
            ui->gridLayout->addWidget(p->getWindow(),1,id-3);
        plot_windows[id]=p; // keep every plot in a list
        number_of_plots ++;

        connect(p, SIGNAL(deleted(int)), SLOT(plot_deleted(int)));
        connect(p, SIGNAL(ellipse_selection_closed(QList<int>)), SLOT(selection(QList<int>)));
        connect(p, SIGNAL(free_selection_closed(QList<int>)), SLOT(selection(QList<int>)));

        //qDebug() << plot_queue;
    }
}

/* close a plot window */
void MainWindow::plot_deleted(int id)
{
    number_of_plots --;
    plot_windows.remove(id);
    int i = 0;
    if (!plot_queue.isEmpty())
    {
        while (id < plot_queue[i])
        {
            i++;
            if (i == plot_queue.length())
                break;
        }
    }
    plot_queue.insert(i, id);
}

/* receiving ellipse_selection_closed or ellipse_selection_closed SIGNAL triggers the slot
   to select data in every plots */
void MainWindow::selection(QList<int> keys)
{
    //select in every plots
    for (auto p : plot_windows.keys())
    {
        QMap<int, QVector<double>> plot_data = removeNonUnique(plot_windows[p]->getData(),keys);
        plot_windows[p]->plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, global_scatter_size),0,plot_data);
        plot_windows[p]->plot_values(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::red, global_scatter_size),1,keys);
        plot_windows[p]->getSelection()->setKeys(keys);
    }
}

QMap<int, QVector<double>> MainWindow::removeNonUnique(QMap<int, QVector<double>> map1, QList<int> keys)
{
    QMap<int, QVector<double>> unique_map = map1;
    for (auto k: keys)
    {
        if(map1.contains(k))
            unique_map.remove(k);
    }
    return unique_map;
}
