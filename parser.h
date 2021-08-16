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
    virtual void parseFileHeader(QString fileName, DatasetContainer *d) = 0;
    virtual void parseFileText(QString fileName, DatasetContainer *d) = 0;
    virtual void parseFileData(QString fileName, DatasetContainer *d) = 0;
};

#endif // PARSER_H
