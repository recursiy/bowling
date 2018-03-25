#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include "types.h"
#include <iostream>
#include <memory>

class InputParser
{
public:
    virtual PlayersHits Parse(std::istream& input) = 0;
};

typedef std::unique_ptr<InputParser> InputParserPtr;
InputParserPtr getInputParser();

#endif //INPUT_PARSER_H
