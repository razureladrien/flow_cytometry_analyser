#include <QWidget>
#include <QMap>
#include <Eigen/Dense>
#include <QDebug>

#ifndef DATASETCONTAINER_H
#define DATASETCONTAINER_H

using namespace Eigen;

/*
 * Class that contains everything concerning the data :
 * Name of the file,
 * Metadata,
 * Parameters,
 * Rough data,
 * Data anotated by the user.
 */
class DatasetContainer
{
public:
    DatasetContainer(QString f, int n_params = 0, int n_events = 0, QList<QString> p = {}, QVector<QVector<float>> d = {});

    // geter and seters
    void setParameters(QList<QString> p){ parameters = p; };
    void setData(QVector<QVector<float>> d);;
    void setAnotatedData(QMap<QString, QList<int>> d){ anotated_data = d; };
    void setHeaderTextOffset(int start_t, int end_t);
    void setHeaderDataOffset(int start_d, int end_d);
    void setMetaData(QList<int> b_order, QString d_type);
    void setDataInfo(int n_params, int n_events);

    QList<QString> getParameters(){ return parameters; };
    MatrixXd getDataMat(){ return data_mat; };
    VectorXd getDataCol(int ind){ return data_mat.col(ind); }
    QMap<QString, QList<int>> getAnotatedData(){ return anotated_data; };
    QList<int> getDataInfo();
    QList<int> getByteOrder(){ return byte_order; };
    QList<int> getTextOffset(){ return {start_text, end_text}; }
    QList<int> getDataOffset(){ return {start_data, end_data}; }
    QString getDataType(){ return data_type; };

    void addAnotated(QString key, QList<int> points);

private:
    QString file_name;

    // FCS metadata
    int start_text, end_text, start_data, end_data;
    QList<int> byte_order;
    QString data_type;

    // global data
    int number_of_params = 0, number_of_events = 0;
    QList<QString> parameters;
    MatrixXd data_mat;

    QMap<QString, QList<int>> anotated_data = {};
};

#endif // DATASETCONTAINER_H
