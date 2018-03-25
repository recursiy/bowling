#include "input_parser.h"
#include "bowling_machine.h"
#include "result_renderer.h"
#include <iostream>
#include <fstream>

int main(int argc, char* argv[])
{
    try
    {
        if (argc < 2)
        {
            std::cout << "Usage: bowling.exe input.txt [output.txt]";
        }
        std::string inputFileName = argv[1];
        std::string outputFileName;
        if (argc > 2)
        {
            outputFileName = argv[2];
        }

        std::ifstream input(inputFileName);
        const auto& playersHits = getInputParser()->Parse(input);
        input.close();
        const auto& playersResults = getBowlingMachine()->CalcPlayersTable(playersHits);

        getConsoleRenderer()->Render(playersResults);
        if (outputFileName != "")
        {
            getFileRenderer(outputFileName)->Render(playersResults);
        }
    }
    catch (std::exception e)
    {
        std::cout << "Exception occured: " << e.what();
    }
}
