#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QWidget>
#include "qcustomplot.h"
#include "custompointselection.h"

class PlotWindow : public QObject
{
    Q_OBJECT
public:
    PlotWindow(QWidget *parent = nullptr, QList<QString> params = {}, QVector<QVector<float>> data = {});
    ~PlotWindow();

    QGroupBox* getWindow();
    QCustomPlot* getPlot();
    QMap<int, QVector<double>> getData() {return data_dic;};

    void addPoint(int key, double x, double y);
    void clearData();
    void plot(QCPScatterStyle scatterStyle,double graph_id, QMap<int, QVector<double>> qv_data);//QVector<double> x, QVector<double> y);
    void plot_values(QCPScatterStyle scatterStyle, double graph_id, QList<int> keys);

    void setParameters(QList<QString> p);
    void setData(QVector<QVector<float>> d);

    void setDataFromParam(int x_param, int y_param);

public slots:
    void close_window();
    void on_btn_zoom_clicked();
    void on_btn_navigate_clicked();
    void on_btn_ellipse_clicked();
    void on_btn_free_form_clicked();

    void startEllipseSelection(QMouseEvent *event);
    void moveEllipseSelection(QMouseEvent *event);
    void endEllipseSelection(QMouseEvent *event);

    void startEndLine(QMouseEvent *event);
    void moveLine(QMouseEvent *event);

//    void axisScale(QMouseEvent *event);

//    void on_btn_resolution_clicked();

    void on_cbox_x_activated();
    void on_cbox_y_activated();

signals:
    void deleted();
    void ellipse_selection_closed(QList<int> selection);
    void free_selection_closed(QList<int> selection);

private:
    QCustomPlot *plot_widget;
    QGroupBox *button_box;
    QGroupBox *plot_box;
    QSize size;

    QPushButton *btn_zoom, *btn_navigate, *btn_ellipse, *btn_free_form, *btn_resolution, *close_btn;
    QComboBox *cbox_x, *cbox_y;

    QVector<double> qv_x_to_plot, qv_y_to_plot;

    QString xscale = "lin";
    QString yscale = "lin";

    QList<QString> parameters;
    QVector<QVector<float>> data;

    QMap<int, QVector<double>> data_dic;



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

#endif // PLOTWINDOW_H
