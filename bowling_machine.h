#ifndef BOWLING_MACHINE_H
#define BOWLING_MACHINE_H

#include "types.h"

#include <memory>

class BowlingMachine
{
public:
    virtual PlayersTable CalcPlayersTable(const PlayersHits& players) = 0;
};

typedef std::unique_ptr<BowlingMachine> BowlingMachinePtr;

BowlingMachinePtr getBowlingMachine();

#endif //BOWLING_MACHINE_H
