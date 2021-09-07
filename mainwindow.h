#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"
#include "custompointselection.h"
#include "plotwindow.h"
#include "settings.h"
#include "information.h"
#include "datasetcontainer.h"
#include "parserfcs.h"
#include "parsercsv.h"

/*
 * Class that contains the main UI with a list of PlotWindow instances.
 * It is also responsible for events related to the menubar (see mainwindow.ui)
 */
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QMap<int, QVector<double> > removeNonUnique(QMap<int, QVector<double> > map1, QList<int> keys);
    DatasetContainer* getContainer() {return data_container;};


private slots:
//    void on_btn_resolution_clicked();
    void on_actionOpen_triggered(); // menubar->open
    void on_actionAdd_plot_triggered(); // menubar->Add Plot
    void plot_deleted(int id); // closing a plot
    void selection(QList<int> keys); // Doing a manual selection on plot
    void on_actionSettings_triggered(); // menubar->Settings
    void setMarkerSize(double size); // Validation of marker size settings
    void on_actionInformations_triggered(); // menubar->Informations
    void on_actionAnotate_selection_triggered(); // menubar->Annotate Selections

private:
    Ui::MainWindow *ui;
    Settings *settings_dialog;
    InformationDialog *information_dialog;

    DatasetContainer *data_container = new DatasetContainer("");
    ParserFCS parser_FCS = ParserFCS();
    ParserCSV parser_CSV = ParserCSV();
    CustomPointSelection *selectionObj = new CustomPointSelection();

    QString file_name;

    qint32 number_of_plots = 0;
    QList<int> plot_queue = {5,4,3,2,1,0};
    QMap<int, PlotWindow *> plot_windows = {};

    double global_scatter_size = 1;

};
#endif // MAINWINDOW_H
