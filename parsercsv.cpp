#include "parsercsv.h"
#include <QDebug>

ParserCSV::ParserCSV()
{

}


void ParserCSV::parseFileInfo(QString fileName, DatasetContainer *d)
{
    QFile file(fileName);
    int n_params, n_events=0;
    if (!file.open(QIODevice::ReadOnly)) return;
    QTextStream in(&file);
    QString line = in.readLine();
    delimiter = ";";
    QList<QString> params = line.split(delimiter);
    n_params = params.length();
    while (!in.atEnd())
    {
        in.readLine();
        n_events++;
    }

    d->setParameters(params);
    d->setDataInfo(n_params, n_events);
    file.close();
}

void ParserCSV::parseFileData(QString fileName, DatasetContainer *d)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return;
    QTextStream in(&file);
    QString line = in.readLine();

    QList<int> infos = d->getDataInfo();

    int it = 0;
    QVector<QVector<float>> data;

    while (!in.atEnd())
    {
        line = in.readLine();
        QList<QString> splitted_line = line.split(delimiter);
        QVector<float> tmp;
        for(auto i=0; i < splitted_line.length(); i++)
        {
            tmp.push_back(splitted_line[i].toFloat());
        }
        data.push_back(tmp);
        it++;
    }
    d->setData(data);
    file.close();
}
