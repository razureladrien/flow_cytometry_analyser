#include "parser.h"

#ifndef PARSERCSV_H
#define PARSERCSV_H


class ParserCSV : public Parser
{
public:
    ParserCSV();
    void parseFileInfo(QString fileName, DatasetContainer *d);
    void parseFileData(QString fileName, DatasetContainer *d);

private:
    QString delimiter;
};

#endif // PARSERCSV_H
