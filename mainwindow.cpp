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

    // initialize with one plot opened
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

/* opening file */
void MainWindow::on_actionOpen_triggered()
{
    selectionObj->clearSelectionPoints();
    // check if user is ready to lose his annotations
    if (!data_container->getAnotatedData().isEmpty())
    {

        QMessageBox msgBox;
        msgBox.setText("Open new file? \n\nCurrent annotations will be lost..");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        msgBox.setIcon(QMessageBox::Warning);

        int ret = msgBox.exec();
        switch (ret) {
        case  QMessageBox::Ok:
        {
            break;
        }
        case QMessageBox::Cancel:
            return;
        }
    }

    QStringList file = QFileDialog::getOpenFileNames(this, tr("Open File"),"/flowData",tr("CSV/FCS Files (*.csv *.fcs)"));
    if (!file.empty())
    {
        delete data_container;
        for (auto id : plot_windows.keys())
            plot_windows[id]->close_window();

        plot_queue = {5,4,3,2,1,0};

        // parse file
        file_name = file[0];

        // creating the DatasetContainer
        data_container = new DatasetContainer(file_name);

        QFileInfo fi(file_name);
        QString file_extension = fi.suffix();

        if((file_extension=="fcs") || (file_extension=="FCS"))
        {
            // calling parser methods
            parser_FCS.parseFileHeader(file_name, data_container);
            parser_FCS.parseFileInfo(file_name, data_container);
            parser_FCS.parseFileData(file_name, data_container);
        } else if((file_extension=="csv") || (file_extension=="CSV"))
        {
            parser_CSV.parseFileInfo(file_name, data_container);
            parser_CSV.parseFileData(file_name, data_container);
        }


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
        PlotWindow *p = new PlotWindow(this, data_container->getParameters(), data_container,selectionObj, id, global_scatter_size);

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
    plot_windows[0]->getSelection()->setKeys(keys);
    //select in every plots
    QElapsedTimer timer;
    timer.start();
    for (auto p : plot_windows.keys())
    {
        QMap<int, QVector<double>> plot_data = removeNonUnique(plot_windows[p]->getData(),keys);
        plot_windows[p]->plot(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::black, global_scatter_size),0,plot_data);
        plot_windows[p]->plot_values(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::red, global_scatter_size),1,keys);
    }
    qDebug() << "Global plot time: " << timer.elapsed();
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
}
