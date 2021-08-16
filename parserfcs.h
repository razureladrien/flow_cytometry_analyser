#include "parser.h"

#ifndef PARSERFCS_H
#define PARSERFCS_H


class ParserFCS : public Parser
{
public:
    ParserFCS();
    void parseFileHeader(QString fileName, DatasetContainer *d);
    void parseFileText(QString fileName, DatasetContainer *d);
    void parseFileData(QString fileName, DatasetContainer *d);
};

#endif // PARSERFCS_H
