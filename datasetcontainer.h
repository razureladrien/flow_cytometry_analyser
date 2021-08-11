#include <QWidget>
#include <QMap>

#ifndef DATASETCONTAINER_H
#define DATASETCONTAINER_H

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
    void setData(QVector<QVector<float>> d){ data = d; };
    void setAnotatedData(QMap<QString, QList<int>> d){ anotated_data = d; };
    void setHeaderData(int start_t, int end_t, int start_d, int end_d);
    void setMetaData(QList<int> b_order, QString d_type);
    void setDataInfo(int n_params, int n_events);

    QList<QString> getParameters(){ return parameters; };
    QVector<QVector<float>> getData(){ return data; };
    QMap<QString, QList<int>> getAnotatedData(){ return anotated_data; };
    QList<int> getDataInfo();
    QList<int> getByteOrder(){ return byte_order; };
    QList<int> getHeaderData(){ return {start_text, end_text, start_data, end_data}; }
    QString getDataType(){ return data_type; };

    void addAnotated(QString key, QList<int> points);

private:
    QString file_name;

    // FCS metadata
    int start_text, end_text, start_data, end_data, number_of_params = 0, number_of_events = 0;
    QList<int> byte_order;
    QString data_type;

    // global data
    QList<QString> parameters;
    QVector<QVector<float>> data;

    QMap<QString, QList<int>> anotated_data = {};
};

#endif // DATASETCONTAINER_H
