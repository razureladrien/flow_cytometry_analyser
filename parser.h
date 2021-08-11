#include "datasetcontainer.h"
#include <QFile>

#ifndef PARSER_H
#define PARSER_H

/*
 * Class used as a tool to parse a FCS file and organise the
 * given DatasetContainer.
 */
class Parser
{
public:
    Parser();

    void parseFileHeader(QString fileName, DatasetContainer *d);
    void parseFileText(QString fileName, DatasetContainer *d);
    void parseFileData(QString fileName, DatasetContainer *d);
};

#endif // PARSER_H
