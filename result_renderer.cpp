#include "result_renderer.h"
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>

namespace //anonymous
{
    class WinTableBuilder
    {
    private:
        size_t m_tableWidth;

        ///Draw string of '-' to delimit strings on table
        void DrawStringDelimiter(std::ostream& out)
        {
            for (size_t i = 0; i < m_tableWidth; ++i)
            {
                out << '-';
            }
            out << std::endl;
        }

        std::vector<std::string> GetWinners(const PlayersTable& table)
        {
            unsigned int maxPlayerResult = 0;
            std::vector<std::string> winners;
            for (const PlayerTable& player : table)
            {
                if (player.total > maxPlayerResult)
                {
                    maxPlayerResult = player.total;
                    winners.clear();
                    winners.push_back(player.playerName);
                }
                else if (player.total == maxPlayerResult)
                {
                    winners.push_back(player.playerName);
                }
            }
            return winners;
        }

        std::string GetWinnersString(const std::vector<std::string>& winners)
        {
            if (winners.size() == 1)
            {
                return winners[0] + " is winner! Congratulations!";
            }

            std::string result;
            for (size_t i = 0; i < winners.size(); ++i)
            {
                result += winners[i];
                if (i != winners.size() - 1)
                {
                    result += " and ";
                }
            }
            result += " are tied!";
            return result;
        }

    public:
        WinTableBuilder()
            : m_tableWidth(0)
        {
        }

        void Build(std::ostream& out, const PlayersTable& table)
        {
            m_tableWidth = 0;
            m_tableWidth += 1;     //open dash
            size_t maxPlayerNameLen = 0;
            for (const PlayerTable& player : table)
            {
                if (player.playerName.size() > maxPlayerNameLen)
                    maxPlayerNameLen = player.playerName.size();
            }
            m_tableWidth += maxPlayerNameLen; //player name field
            m_tableWidth += 4 * (FramesPerGame-1);    //size of all frames except 10th
            size_t maxTenFrameHits = 0;
            for (const auto& player : table)
            {
                const size_t hitCount = player.frames[9].hit.size();
                if (hitCount > maxTenFrameHits)
                    maxTenFrameHits = hitCount;
            }
            m_tableWidth += 1 + maxTenFrameHits * 2 - 1;  //10th frame
            m_tableWidth += 1 + 3;    //total summ
            m_tableWidth += 1;        //close dash

            unsigned int maxPlayerResult = 0;
            std::vector<std::string> winners;
            for (const PlayerTable& player : table)
            {
                if (player.total > maxPlayerResult)
                {
                    maxPlayerResult = player.total;
                    winners.clear();
                    winners.push_back(player.playerName);
                }
                else if (player.total == maxPlayerResult)
                {
                    winners.push_back(player.playerName);
                }
            }

            for (const PlayerTable& player : table)
            {
                DrawStringDelimiter(out);

                //1th string
                out << '|' << std::setfill(' ') << std::setw(maxPlayerNameLen) << std::left << player.playerName;
                for (size_t i = 0; i < player.frames.size() - 1; ++i)
                {
                    const Frame& frame = player.frames[i];
                    out << '|';
                    for (size_t i = 0; i < frame.hit.size(); ++ i)
                    {
                        out << frame.hit[i];
                        if (i != frame.hit.size() - 1)
                            out << ' ';
                    }
                    if (frame.hit.size() == 1)  //strike
                    {
                        out << "  ";
                    }
                }
                const Frame& tenFrame = player.frames[9];  //10th frame
                const size_t tenFrameWidth = maxTenFrameHits * 2 - 1;
                out << '|';
                for (size_t i = 0; i < tenFrame.hit.size(); ++i)
                {
                    out << tenFrame.hit[i];
                    if (i != tenFrame.hit.size() - 1)
                        out << ' ';
                }
                for (size_t i = 0; i < tenFrameWidth - (tenFrame.hit.size() * 2 - 1); ++i)
                    out << ' ';
                out << "|   ";   //len of total is always three
                out << '|' << std::endl;

                //2th string
                out << '|';
                for (size_t i = 0; i < maxPlayerNameLen; ++i)
                    out << ' ';
                for (size_t i = 0; i < player.frames.size() - 1; ++i)
                {
                    const Frame& frame = player.frames[i];
                    out << '|';
                    const size_t frameWidth = std::max(2u, frame.hit.size()) * 2 - 1;
                    out << std::setfill(' ') << std::setw(frameWidth) << std::left << frame.result;
                }
                out << '|';
                out << std::setfill(' ') << std::setw(tenFrameWidth) << std::left << tenFrame.result;

                out << '|' << std::setfill(' ') << std::setw(3) << std::left << player.total;
                out << '|' << std::endl;
            }

            //hint
            DrawStringDelimiter(out);
            out << '|';
            std::string hint = GetWinnersString(GetWinners(table));
            const unsigned int indent = (m_tableWidth - 2 - hint.size()) / 2;
            assert(indent >= 0);
            for (size_t i = 0; i < indent; ++i)
                out << ' ';
            out << hint;
            for (size_t i = 0; i < (m_tableWidth - 2 - hint.size() - indent); ++i)
                out << ' ';
            out << '|' << std::endl;

            DrawStringDelimiter(out);
        }
    };

    class ConsoleRenderer : public Renderer
    {
    private:
        WinTableBuilder m_builder;

    public:
        void Render(const PlayersTable& table) override
        {
            m_builder.Build(std::cout, table);
        }
    };

    class FileRenderer : public Renderer
    {
    private:
        WinTableBuilder m_builder;
        const std::string m_filename;

    public:
        FileRenderer(const std::string& filename)
            : m_filename(filename)
        {
        }

        void Render(const PlayersTable& table) override
        {
            std::ofstream outFile(m_filename, 'w');
            m_builder.Build(outFile, table);
            outFile.close();
        }
    };
}   //namespace anonymous


RendererPtr getConsoleRenderer()
{
    return std::make_unique<ConsoleRenderer>();
}
RendererPtr getFileRenderer(const std::string& filename)
{
    return std::make_unique<FileRenderer>(filename);
}
