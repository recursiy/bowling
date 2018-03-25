#include "input_parser.h"
#include <sstream>

namespace //anonymous
{

    class InputParserImpl : public InputParser
    {
    public:
        PlayersHits Parse(std::istream& input) override
        {
            PlayersHits result;
            char buffer[256];
            while (input.getline(buffer, sizeof(buffer)))
            {
                std::stringstream in(buffer);
                std::string name;
                in >> name;
                name = name.substr(0, name.size() - 1);
                std::vector<unsigned int> hits;
                while (!in.eof())
                {
                    unsigned int hit;
                    in >> hit;
                    hits.push_back(hit);
                }

                result.push_back({ name, hits });
            }
            return result;
        }
    };

}   //namespace anonymous

InputParserPtr getInputParser()
{
    return std::make_unique<InputParserImpl>();
}
