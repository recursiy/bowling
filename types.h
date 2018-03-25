#ifndef TYPES_H
#define TYPES_H

#include <array>
#include <string>
#include <vector>

#include "const.h"

//input types
typedef std::vector<unsigned int> Hits;

struct PlayerHits
{
    std::string playerName;
    Hits hits;
};

typedef std::vector<PlayerHits> PlayersHits;

//output types
struct Frame
{
    Frame()
        : frameNumber(0)
        , result(0)
    {
    }

    Frame(unsigned int i_frameNumber)
        : frameNumber(i_frameNumber)
        , result(0)
    {
    }

    Frame(unsigned int i_frameNumber, std::vector<char> i_hit, unsigned int i_result)
        : frameNumber(i_frameNumber)
        , hit(i_hit)
        , result(i_result)
    {
    }

    unsigned int frameNumber;
    std::vector<char> hit;  ///contains hit information - '1'-'9' for hit, 'x' for strike, '/' for spare and '-' for miss
    unsigned int result;    ///contains frame result

    bool operator == (const Frame& other) const
    {
        return
            frameNumber == other.frameNumber
            && hit == other.hit
            && result == other.result;
    }
};

struct PlayerTable
{
    PlayerTable()
        : total(0)
    {
    }

    std::string playerName;
    std::array<Frame, FramesPerGame> frames;
    unsigned int total;             ///total result for player
};

typedef std::vector<PlayerTable> PlayersTable;

#endif
