#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QWidget>
#include <Eigen/Dense>
#include "datasetcontainer.h"
#include "qcustomplot.h"
#include "custompointselection.h"

using namespace Eigen;
/*
 * Class that contains everything concerning interactive plot window
 * with its associated QPushButtons (navigate, ellipse selection, polygonal selection,
 * scales, parameters, ...) and its QCustomPlot (see library QCustomPlot).
 */
class PlotWindow : public QObject
{
    Q_OBJECT
public:
    PlotWindow(QWidget *parent = nullptr, QList<QString> params = {}, DatasetContainer *data = nullptr, CustomPointSelection *select = nullptr,
               int id = 0, double size = 1, QCPScatterStyle::ScatterShape d_shape = QCPScatterStyle::ssDisc, QCPScatterStyle::ScatterShape = QCPScatterStyle::ssDisc);
    ~PlotWindow();

    QGroupBox* getWindow();
    QCustomPlot* getPlot();
    QMap<int, QVector<double>> getData() {return data_dic;};
    CustomPointSelection* getSelection() {return selectionObj;};
    int getID() {return plot_id;};

    void addPoint(int key, double x, double y);
    void clearData();
    void plot(QCPScatterStyle scatterStyle,double graph_id, QMap<int, QVector<double>> qv_data); // scatter plot values
                                                                                                 // in qv_data
    void plot_values(QCPScatterStyle scatterStyle, double graph_id, QList<int> keys); // retrieve values that correspond
                                                                                      // to keys and scatter plot them

    void setParameters(QList<QString> p);
    void setData(DatasetContainer *d);
    void setID(int id){plot_id = id;};
    void setScatterSize(double s){scatter_size = s;};

    void setDataFromParam(int x_param, int y_param);
    void setScatterShape(QCPScatterStyle::ScatterShape d_shape, QCPScatterStyle::ScatterShape s_shape) { data_shape = d_shape, selection_shape = s_shape; };

    QMap<int, QVector<double> > removeNonUnique(QMap<int, QVector<double> > map1, QList<int> keys);

public slots:
    // buttons click
    void close_window();
    void on_btn_zoom_clicked();
    void on_btn_navigate_clicked();
    void on_btn_ellipse_clicked();
    void on_btn_free_form_clicked();
    void on_btn_reset_clicked();

    // ellipse drawing and selection
    void startEllipseSelection(QMouseEvent *event);
    void moveEllipseSelection(QMouseEvent *event);
    void endEllipseSelection(QMouseEvent *event);

    // polygon drawing and selection
    void startEndLine(QMouseEvent *event);
    void moveLine(QMouseEvent *event);

//    void on_btn_resolution_clicked();

    // changing parameters
    void on_cbox_x_activated();
    void on_cbox_y_activated();

    // changing axis scale (logarithmic or linear)
    void axisScaleX(int s);
    void axisScaleY(int s);

    // click on the axis to change its scale individualy
    // with the mouse wheel
    void mouseOverAxis(QMouseEvent *event);
    void xAxisSelect(QCPAxis::SelectableParts);
    void yAxisSelect(QCPAxis::SelectableParts);
    void moveAxisDragging(QWheelEvent*);
    void adaptativeSampling(QMouseEvent *event);

signals:
    void deleted(int); // signal emitted when a plot is closed
    void ellipse_selection_closed(QList<int> selection); // signal emitted when an ellipse selection has been made
    void free_selection_closed(QList<int> selection); // signal emitted when a free form selection has been made

private:
    QCustomPlot *plot_widget;
    QGroupBox *button_box;
    QGroupBox *plot_box;
    QSize size;
    QCPScatterStyle::ScatterShape data_shape;
    QCPScatterStyle::ScatterShape selection_shape;

    QCheckBox *btn_logx, *btn_logy;
    QList<QPushButton *> tool_buttons;
    QPushButton *btn_zoom, *btn_navigate, *btn_ellipse, *btn_free_form, *btn_resolution, *close_btn, *btn_reset;
    QComboBox *cbox_x, *cbox_y;

    int plot_id;

    QString xscale = "lin";
    QString yscale = "lin";

    QList<QString> parameters;
    DatasetContainer *data_container = nullptr;

    QMap<int, QVector<double>> data_dic;
    QMap<int, QVector<double>> data_dic_low_res;

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

    // variables to control if some actions are in progress
    bool eActive=false;
    bool lActive=false;
    bool xdActive=false;
    bool ydActive=false;
    bool started_line=false;
    bool ellipse_select=false;
    bool free_form_select=false;
    bool rescale_flag=true;
    //bool resolution_flag=true; // true = high res, false = low res
    bool xAxis_selected=false;
    bool yAxis_selected=false;

    bool adapt_sampling = true;

    // used to change de aspect of the cursor
    QCursor current_cursor;

};

#endif // PLOTWINDOW_H
