#include "datasetcontainer.h"

DatasetContainer::DatasetContainer(QString f, int n_params, int n_events, QList<QString> p, QVector<QVector<float>> d)
{
    file_name = f;
    number_of_params = n_params;
    number_of_events = n_events;
    parameters = p;
    data = d;
}

void DatasetContainer::setHeaderData(int start_t, int end_t, int start_d, int end_d)
{
    start_text = start_t;
    end_text = end_t;
    start_data = start_d;
    end_data = end_d;
}

void DatasetContainer::setMetaData(QList<int> b_order, QString d_type)
{
    byte_order = b_order;
    data_type = d_type;
}

void DatasetContainer::setDataInfo(int n_params, int n_events)
{
    number_of_events = n_events;
    number_of_params = n_params;
}

QList<int> DatasetContainer::getDataInfo()
{
    return {number_of_params, number_of_events};
}

void DatasetContainer::addAnotated(QString key, QList<int> points)
{
    anotated_data[key] = points;
}
