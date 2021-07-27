#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QWidget>
#include "qcustomplot.h"
#include "custompointselection.h"

class PlotWindow : public QObject
{
    Q_OBJECT
public:
    PlotWindow(QWidget *parent = nullptr, QList<QString> params = {}, QVector<QVector<float>> data = {}, int id = 0, double size = 1);
    ~PlotWindow();

    QGroupBox* getWindow();
    QCustomPlot* getPlot();
    QMap<int, QVector<double>> getData() {return data_dic;};
    CustomPointSelection* getSelection() {return selectionObj;};
    int getID() {return plot_id;};

    void addPoint(int key, double x, double y);
    void clearData();
    void plot(QCPScatterStyle scatterStyle,double graph_id, QMap<int, QVector<double>> qv_data);//QVector<double> x, QVector<double> y);
    void plot_values(QCPScatterStyle scatterStyle, double graph_id, QList<int> keys);

    void setParameters(QList<QString> p);
    void setData(QVector<QVector<float>> d);
    void setID(int id){plot_id = id;};
    void setScatterSize(double s){scatter_size = s;};

    void setDataFromParam(int x_param, int y_param);

    QMap<int, QVector<double> > removeNonUnique(QMap<int, QVector<double> > map1, QList<int> keys);

public slots:
    void close_window();
    void on_btn_zoom_clicked();
    void on_btn_navigate_clicked();
    void on_btn_ellipse_clicked();
    void on_btn_free_form_clicked();
    void on_btn_reset_clicked();

    void startEllipseSelection(QMouseEvent *event);
    void moveEllipseSelection(QMouseEvent *event);
    void endEllipseSelection(QMouseEvent *event);

    void startEndLine(QMouseEvent *event);
    void moveLine(QMouseEvent *event);

//    void on_btn_resolution_clicked();

    void on_cbox_x_activated();
    void on_cbox_y_activated();

    void axisScaleX(int s);
    void axisScaleY(int s);

    void mouseOverAxis(QMouseEvent *event);
    void xAxisSelect(QCPAxis::SelectableParts);
    void yAxisSelect(QCPAxis::SelectableParts);

    void moveAxisDragging(QWheelEvent*);

signals:
    void deleted(int);
    void ellipse_selection_closed(QList<int> selection);
    void free_selection_closed(QList<int> selection);

private:
    QCustomPlot *plot_widget;
    QGroupBox *button_box;
    QGroupBox *plot_box;
    QSize size;

    QCheckBox *btn_logx, *btn_logy;
    QList<QPushButton *> tool_buttons;
    QPushButton *btn_zoom, *btn_navigate, *btn_ellipse, *btn_free_form, *btn_resolution, *close_btn, *btn_reset;
    QComboBox *cbox_x, *cbox_y;

    QVector<double> qv_x_to_plot, qv_y_to_plot;
    int plot_id;

    QString xscale = "lin";
    QString yscale = "lin";

    QList<QString> parameters;
    QVector<QVector<float>> data;

    QMap<int, QVector<double>> data_dic;
    QMap<int, QVector<double>> data_dic_low_res;

    bool eActive=false;
    bool lActive=false;
    bool xdActive=false;
    bool ydActive=false;
    bool started_line=false;

    QCPItemEllipse *ellipse = nullptr;
    CustomPointSelection *selectionObj = nullptr;
    QCPItemLine *line = nullptr;
    QCPItemEllipse *poly_closed = nullptr;
    QList<QCPItemLine *> polygon = {};
    double scatter_size;
    double radius_x, radius_y;
    double start_v_x, start_v_y;
    double start_drag_x, start_drag_y;
    double xdActive_tmp, ydActive_tmp;

    bool ellipse_select=false;
    bool free_form_select=false;
    bool rescale_flag=true;
    bool resolution_flag=true; // true = high res, false = low res
    bool xAxis_selected=false;
    bool yAxis_selected=false;
    QCursor current_cursor;

};

#endif // PLOTWINDOW_H
