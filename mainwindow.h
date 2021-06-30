#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"
#include "custompointselection.h"
#include "plotwindow.h"

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
    QVector<double> getX() {return qv_x; };
    QVector<double> getY() {return qv_y; };
    void parseFileHeader(QString fileName);
    void parseFileText(QString fileName);
    void parseFileData(QString fileName);

    QMap<int, QVector<double> > removeNonUnique(QMap<int, QVector<double> > map1, QList<int> keys);
protected:


private slots:
//    void on_btn_zoom_clicked();
//    void on_btn_navigate_clicked();
//    void on_btn_ellipse_clicked();
//    void on_btn_free_form_clicked();

//    void startEllipseSelection(QMouseEvent *event);
//    void moveEllipseSelection(QMouseEvent *event);
//    void endEllipseSelection(QMouseEvent *event);

//    void startEndLine(QMouseEvent *event);
//    void moveLine(QMouseEvent *event);

//    void axisScale(QMouseEvent *event);

//    void on_btn_resolution_clicked();

    void on_actionOpen_triggered();

//    void on_cbox_x_activated(const QString &arg1);

//    void on_cbox_y_activated(const QString &arg1);

    void on_actionAdd_plot_triggered();

    void plot_deleted();

    void selection(QList<int> keys);

private:
    Ui::MainWindow *ui;

    QVector<double> qv_x, qv_y;
    QVector<double> qv_x_to_plot, qv_y_to_plot;

    qint32 number_of_plots = 0;
    QList<PlotWindow *> plot_windows = {};

    QString xscale = "lin";
    QString yscale = "lin";

    // metadata
    int start_text, end_text, start_data, end_data, number_of_params, number_of_events;
    QList<int> byte_order;
    QString data_type;

    QList<QString> parameters;
    QVector<QVector<float>> data;

    bool eActive=false;
    bool lActive=false;
    bool started_line=false;

    QCPItemEllipse *ellipse = nullptr;
    CustomPointSelection *selectionObj = nullptr;
    QCPItemLine *line = nullptr;
    QCPItemEllipse *poly_closed = nullptr;
    QList<QCPItemLine *> polygon = {};
    double radius_x, radius_y;
    double start_v_x, start_v_y;

    bool ellipse_select=false;
    bool free_form_select=false;
    bool rescale_flag=true;
    bool resolution_flag=true; // true = high res, false = low res

};
#endif // MAINWINDOW_H
