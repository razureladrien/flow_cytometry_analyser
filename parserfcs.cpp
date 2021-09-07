#include "parserfcs.h"

ParserFCS::ParserFCS()
{

}

void ParserFCS::parseFileHeader(QString fileName, DatasetContainer *d)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return;
    QByteArray line = file.read(100);
    QString stringLine = QString::fromStdString(line.constData());
    QStringList fields = stringLine.split(' ', QString::SkipEmptyParts);

    int start_text = fields[1].toInt();
    int end_text = fields[2].toInt();
    int start_data = fields[3].toInt();
    int end_data = fields[4].toInt();

    d->setHeaderTextOffset(start_text, end_text);
    d->setHeaderDataOffset(start_data,end_data);
    file.close();
}

void ParserFCS::parseFileInfo(QString fileName, DatasetContainer *d)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return;

    int number_of_params;
    int number_of_events;

    QList<int> head_data = d->getTextOffset();
    int start_text = head_data[0];
    int end_text = head_data[1];

    int start_data, end_data;

    file.seek(start_text);
    QString delimiter = QString::fromStdString(file.read(1).constData());
    QByteArray line = file.read(end_text-start_text);
    QString stringLine = QString::fromStdString(line.constData());
    QStringList fields = stringLine.split(delimiter, QString::SkipEmptyParts);

    QList<int> byte_order;
    QString data_type;
    QList<QString> parameters = {};

    for (int i=0; i<fields.length(); i++)
    {
        QString tmp = fields[i];
        if (tmp[0] == "$" && tmp[1] == "P" && tmp.at(tmp.size()-1)=="N")
            parameters.append(fields[i+1]);
        else if (tmp == "$PAR")
            number_of_params = fields[i+1].toInt();
        else if (tmp == "$TOT")
            number_of_events = fields[i+1].toInt();
        else if (tmp == "$BYTEORD")
        {
            QStringList ord = fields[i+1].split(',', QString::SkipEmptyParts);
            byte_order.clear();
            for (int j=0; j<ord.length(); j++)
                byte_order.append(ord[j].toInt());
        }
        else if (tmp == "$DATATYPE")
            data_type = fields[i+1];
        else if (tmp == "$BEGINDATA")
            start_data = fields[i+1].toInt();
        else if (tmp == "$ENDDATA")
            end_data = fields[i+1].toInt();
    }

    d->setMetaData(byte_order, data_type);
    d->setParameters(parameters);
    d->setDataInfo(number_of_params, number_of_events);
    d->setHeaderDataOffset(start_data, end_data);

    file.close();
}

/* parsing data byte by byte */
void ParserFCS::parseFileData(QString fileName, DatasetContainer *d)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return;
    QVector<float> tmp_data;
    QList<int> byte_order = d->getByteOrder();
    QString data_type = d->getDataType();
    QList<int> head_data = d->getDataOffset();
    int start_data = head_data[0];
    int end_data = head_data[1];
    qDebug() << start_data << end_data;

    QList<int> data_info = d->getDataInfo();
    int number_of_params = data_info[0];
    int number_of_events = data_info[1];

    if (data_type == "I" && byte_order.length() == 2) // integer 16 bits
    {
        if (byte_order[0] > 1) // reverted order
        {
            //qDebug() << "revert";
            file.seek(start_data);
            while (file.pos() < end_data)
            {
                QByteArray arr = file.read(byte_order[0]);
                QByteArray b(arr.size(),0);
                std::copy(arr.crbegin(),arr.crend(),b.begin());
                quint16 int_value;
                memcpy(&int_value, b.constData(), byte_order[0]);
                float float_value = int_value;
                tmp_data.append(float_value);
            }
        } else if (byte_order[0] == 1) // normal order
        {
            file.seek(start_data);
            while (file.pos() < end_data)
            {
                QByteArray arr = file.read(byte_order[byte_order.length()-1]);
                quint16 int_value;
                memcpy(&int_value, arr.constData(), byte_order[byte_order.length()-1]);
                float float_value = int_value;
                tmp_data.append(float_value);
            }
        }


    } else if (data_type == "I" && byte_order.length()==4) // integer 32 bits
    {
        if (byte_order[0] > 1) // reverted order
        {
            //qDebug() << "revert";
            file.seek(start_data);
            while (file.pos() < end_data)
            {
                QByteArray arr = file.read(byte_order[0]);
                QByteArray b(arr.size(),0);
                std::copy(arr.crbegin(),arr.crend(),b.begin());
                quint32 int_value;
                memcpy(&int_value, b.constData(), byte_order[0]);
                float float_value = int_value;
                tmp_data.append(float_value);
            }
        } else if (byte_order[0] == 1) // normal order
        {
            file.seek(start_data);
            while (file.pos() < end_data)
            {
                QByteArray arr = file.read(byte_order[byte_order.length()-1]);
                quint32 int_value;
                memcpy(&int_value, arr.constData(), byte_order[byte_order.length()-1]);
                float float_value = int_value;
                tmp_data.append(float_value);
            }
        }


    } else if (data_type == "F") // float
    {
        if (byte_order[0] > 1) // reverted order
        {
            file.seek(start_data);
            while (file.pos() < end_data)
            {
                QByteArray arr = file.read(byte_order[0]);
                QByteArray b(arr.size(),0);
                std::copy(arr.crbegin(),arr.crend(),b.begin());
                float f;
                memcpy(&f, b.constData(), byte_order[0]);
                tmp_data.append(f);
            }
        } else if (byte_order[0] == 1) // normal order
        {
            file.seek(start_data);
            while (file.pos() < end_data)
            {
                QByteArray arr = file.read(byte_order[byte_order.length()-1]);
                float f;
                memcpy(&f, arr.constData(), byte_order[byte_order.length()-1]);
                tmp_data.append(f);
            }
        }
    }
    QVector<QVector<float>> data = {};
    int iter = 0;
    for (int i=0; i<number_of_events; i++)
    {
        QVector<float> tmp_vector(number_of_params); // data are stored as float vector
        for (int j=0; j<number_of_params; j++)
        {
            tmp_vector[j]=tmp_data[iter];
            iter ++;
        }
        data.push_back(tmp_vector);
    }

    d->setData(data);

    file.close();
}
