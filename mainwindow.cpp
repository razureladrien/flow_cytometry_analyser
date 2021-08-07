#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <time.h>
#include <fstream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("FlowCytoAnalyzer");
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
void MainWindow::parseFileHeader(QString fileName, DatasetContainer *d)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return;
    QByteArray line = file.read(100);
    QString stringLine = QString::fromStdString(line.constData());
    QStringList fields = stringLine.split(' ', QString::SkipEmptyParts);

    int start_text = fields[1].toInt();
    int end_text = fields[2].toInt();
    int start_data = fields[3].toInt();
    int end_data = fields[4].toInt();

    d->setHeaderData(start_text, end_text, start_data, end_data);

    file.close();
}

/* parsing "text data" to get metadata */
void MainWindow::parseFileText(QString fileName, DatasetContainer *d)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return;

    int number_of_params;
    int number_of_events;

    QList<int> head_data = d->getHeaderData();
    int start_text = head_data[0];
    int end_text = head_data[1];

    file.seek(start_text);
    QString delimiter = QString::fromStdString(file.read(1).constData());
    QByteArray line = file.read(end_text-start_text);
    QString stringLine = QString::fromStdString(line.constData());
    QStringList fields = stringLine.split(delimiter, QString::SkipEmptyParts);

    QList<int> byte_order;
    QString data_type;
    QList<QString> parameters = {};

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

    d->setMetaData(byte_order, data_type);
    d->setParameters(parameters);
    d->setDataInfo(number_of_params, number_of_events);

    file.close();
}

/* parsing data byte by byte */
void MainWindow::parseFileData(QString fileName, DatasetContainer *d)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return;
    QVector<float> tmp_data;
    QList<int> byte_order = d->getByteOrder();
    QString data_type = d->getDataType();
    QList<int> head_data = d->getHeaderData();
    int start_data = head_data[2];
    int end_data = head_data[3];

    QList<int> data_info = d->getDataInfo();
    int number_of_params = data_info[0];
    int number_of_events = data_info[1];

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
    QVector<QVector<float>> data = {};
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

    d->setData(data);

    file.close();

    //qDebug() << data.length() << data[0].length();
}

/* opening file */
void MainWindow::on_actionOpen_triggered()
{
    if (!data_container->getAnotatedData().isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText("Open new file?");
        msgBox.setInformativeText("The current annotations will be lost");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        msgBox.setIcon(QMessageBox::Warning);
        int ret = msgBox.exec();
        switch (ret) {
        case  QMessageBox::Ok:
        {
            delete data_container;
            break;
        }
        case QMessageBox::Cancel:
            return;
        }
    }

    QStringList file = QFileDialog::getOpenFileNames(this, tr("Open File"),"/flowData",tr("CSV/FCS Files (*.csv *.fcs)"));
    if (!file.empty())
    {
        for (auto id : plot_windows.keys())
            plot_windows[id]->close_window();

        plot_queue = {5,4,3,2,1,0};

        // parse file
        file_name = file[0];

        //data_container = new DatasetContainer(file_name, number_of_params, number_of_events, parameters, data);
        data_container = new DatasetContainer(file_name);

        parseFileHeader(file_name, data_container);
        parseFileText(file_name, data_container);
        parseFileData(file_name, data_container);


        on_actionAdd_plot_triggered();
    }
}

/* instanciate plot when action "Add plot" triggered */
void MainWindow::on_actionAdd_plot_triggered()
{
    if (number_of_plots == 6)
    {
        QMessageBox msgBox;
        msgBox.setText("The number of plots cannot exceed 6.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    } else if (number_of_plots < 6) {
        int id = plot_queue.last();
        plot_queue.pop_back();
        PlotWindow *p = new PlotWindow(this, data_container->getParameters(), data_container->getData(), id, global_scatter_size);

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

void MainWindow::on_actionSettings_triggered()
{
    settings_dialog = new Settings(this, global_scatter_size);
    settings_dialog->show();
    connect(settings_dialog, SIGNAL(marker_size(double)), SLOT(setMarkerSize(double)));
}

void MainWindow::setMarkerSize(double size)
{
    global_scatter_size = size;
    if (!plot_windows.isEmpty())
    {
        QList<int> keys = plot_windows[0]->getSelection()->getSelectionPoints();
        for (auto p : plot_windows.keys())
        {
            QMap<int, QVector<double>> plot_data = removeNonUnique(plot_windows[p]->getData(),keys);
            plot_windows[p]->setScatterSize(size);
            plot_windows[p]->plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, global_scatter_size),0, plot_data);
            plot_windows[p]->plot_values(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::red, global_scatter_size),1,plot_windows[p]->getSelection()->getSelectionPoints());
        }
    }
}

void MainWindow::on_actionInformations_triggered()
{
    QString f = file_name.split("/").last();
    QList<int> data_info = data_container->getDataInfo();
    QList<QString> params = data_container->getParameters();
    information_dialog = new InformationDialog(this, f, data_info[1], data_info[0], params);
    information_dialog->show();
}

void MainWindow::on_actionAnotate_selection_triggered()
{
    bool ok;
    QList<int> selection = plot_windows[0]->getSelection()->getSelectionPoints();
    QString text = QInputDialog::getText(this, tr("Annotation"),
                                         tr("Annotation name for the selection:"), QLineEdit::Normal,"", &ok);
    if (ok && !text.isEmpty() && !selection.isEmpty())
    {
        data_container->addAnotated(text, selection);
    }
    qDebug() << data_container->getAnotatedData().keys();
}
