#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"
#include "custompointselection.h"
#include "plotwindow.h"
#include "settings.h"
#include "information.h"
#include "datasetcontainer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void addPoint(double x, double y);
    void clearData();
    void plot(QCPScatterStyle scatterStyle,double graph_id, QVector<double> x, QVector<double> y);
    void setDataFromParam(int x_param, int y_param);

    QVector<double> getRange();
    void parseFileHeader(QString fileName, DatasetContainer *data);
    void parseFileText(QString fileName, DatasetContainer *data);
    void parseFileData(QString fileName, DatasetContainer *data);

    void queuePlot(PlotWindow *p, QList<int> keys);

    QMap<int, QVector<double> > removeNonUnique(QMap<int, QVector<double> > map1, QList<int> keys);
protected:


private slots:
//    void on_btn_resolution_clicked();
    void on_actionOpen_triggered();
    void on_actionAdd_plot_triggered();
    void plot_deleted(int id);
    void selection(QList<int> keys);
    void on_actionSettings_triggered();
    void setMarkerSize(double size);
    void on_actionInformations_triggered();

    void on_actionAnotate_selection_triggered();

private:
    Ui::MainWindow *ui;
    Settings *settings_dialog;
    InformationDialog *information_dialog;

    DatasetContainer *data_container = new DatasetContainer("");

    QString file_name;

    qint32 number_of_plots = 0;
    QList<int> plot_queue = {5,4,3,2,1,0};
    QMap<int, PlotWindow *> plot_windows = {};

    QString xscale = "lin";
    QString yscale = "lin";

    // metadata
    //int start_text, end_text, start_data, end_data, number_of_params = 0, number_of_events = 0;
    //QList<int> byte_order;
    //QString data_type;

    //QList<QString> parameters;
    //QVector<QVector<float>> data;

    bool eActive=false;
    bool lActive=false;
    bool started_line=false;

    QCPItemEllipse *ellipse = nullptr;
    CustomPointSelection *selectionObj = nullptr;
    QCPItemLine *line = nullptr;
    QCPItemEllipse *poly_closed = nullptr;
    QList<QCPItemLine *> polygon = {};
    double global_scatter_size = 1;
    double radius_x, radius_y;
    double start_v_x, start_v_y;

    bool ellipse_select=false;
    bool free_form_select=false;
    bool rescale_flag=true;
    bool resolution_flag=true; // true = high res, false = low res

};
#endif // MAINWINDOW_H
