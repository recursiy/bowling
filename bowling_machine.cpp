#include "bowling_machine.h"
#include "const.h"

namespace //anonymous
{
    class BowlingMachineImpl : public BowlingMachine
    {
    private:
        char MakeChar(unsigned int number)
        {
            if (number == 0)
                return MissSign;
            if (number == 10)
                return StrikeSign;
            return number + '0';
        }

#ifndef UNITTEST
    private:
#else
    public:
#endif
        PlayerTable CalcPlayerTable(const PlayerHits& hits)
        {
            PlayerTable result;
            result.playerName = hits.playerName;

            size_t frameNumber = 1;
            Frame currentFrame(frameNumber);
            std::vector<unsigned int> currentResult;
            for (size_t i = 0; i < hits.hits.size(); ++i)
            {
                unsigned int hit = hits.hits[i];
                if (hit > 10)
                    throw std::runtime_error("Hit value is more then 10");
                currentResult.push_back(hit);
                currentFrame.result += hit;

                bool frameOver = false;
                if (currentResult.size() == 2)
                {
                    //2 hit per frame if not strike
                    currentFrame.hit.push_back(MakeChar(currentResult[0]));

                    if (currentFrame.result == AllPinsDown)
                    {
                        //spare
                        currentFrame.hit.push_back(SpareSign);
                        if (hits.hits.size() < i + 1)
                            throw std::runtime_error("Not enought hit values");
                        currentFrame.result += hits.hits[i + 1];
                        if (frameNumber == FramesPerGame)   //10-th frame
                        {
                            ++i;    //i+1 hit belongs to 10th frame, so skip it
                            currentFrame.hit.push_back(MakeChar(hits.hits[i]));
                        }
                    }
                    else
                    {
                        currentFrame.hit.push_back(MakeChar(currentResult[1]));
                    }

                    frameOver = true;
                }
                else if (currentFrame.result == AllPinsDown)
                {
                    //strike
                    currentFrame.hit.push_back(StrikeSign);
                    if (hits.hits.size() < i + 2)
                        throw std::runtime_error("Not enought hit values");
                    currentFrame.result += hits.hits[i + 1] + hits.hits[i + 2];
                    if (frameNumber == FramesPerGame)   //10th frame
                    {
                        i += 2; //i+1 and i+2 hits belongs to 10th frame, so skip them
                        currentFrame.hit.push_back(MakeChar(hits.hits[i-1]));
                        currentFrame.hit.push_back(MakeChar(hits.hits[i]));
                    }

                    frameOver = true;
                }
                if (frameOver)
                {
                    result.total += currentFrame.result;
                    result.frames[frameNumber - 1] = currentFrame;
                    ++frameNumber;
                    currentFrame = Frame(frameNumber);
                    currentResult.clear();
                }
            }

            return result;
        }

    public:
        PlayersTable CalcPlayersTable(const PlayersHits& players) override
        {
            PlayersTable result;

            for (const PlayerHits& player : players)
            {
                result.push_back(CalcPlayerTable(player));
            }

            return result;
        }
    };

} //namespace anonymous


BowlingMachinePtr getBowlingMachine()
{
    return std::make_unique<BowlingMachineImpl>();
}

#ifdef UNITTEST

#include "gtest/gtest.h"

///Table without misses, strikes and spares
TEST(bowlingMachine, simpleTable)
{
    PlayerHits playerHits
    ({
        "TestPlayer1",
        {
            1, 1,   //frame1: 2
            2, 2,   //frame2: 4
            3, 3,   //frame3: 6
            4, 4,   //frame4: 8
            5, 4,   //frame5: 9
            6, 3,   //frame6: 9
            7, 2,   //frame7: 9
            8, 1,   //frame8: 9
            8, 1,   //frame9: 9
            1, 2,   //frame10: 3
                    //total: 68
        }
    });

    BowlingMachineImpl machine;

    PlayerTable result = machine.CalcPlayerTable(playerHits);
    std::array<Frame, FramesPerGame> expectedFrames =
    { {
        { 1, { '1', '1' }, 2 },
        { 2, { '2', '2' }, 4 },
        { 3, { '3', '3' }, 6 },
        { 4, { '4', '4' }, 8 },
        { 5, { '5', '4' }, 9 },
        { 6, { '6', '3' }, 9 },
        { 7, { '7', '2' }, 9 },
        { 8, { '8', '1' }, 9 },
        { 9, { '8', '1' }, 9 },
        { 10,{ '1', '2' }, 3 },
    } };

    EXPECT_EQ(result.playerName, "TestPlayer1");
    for (size_t i = 0; i < expectedFrames.size(); ++i)
    {
        EXPECT_EQ(expectedFrames[i], result.frames[i]);
    }
    EXPECT_EQ(result.total, 68);
}

///Table with spares, but not with spare in 10th frame
TEST(bowlingMachine, simpleSpare)
{
    PlayerHits playerHits
    ({
        "TestPlayer1",
        {
            1, 9,   //frame1: 12
            2, 2,   //frame2: 4
            3, 7,   //frame3: 14
            4, 4,   //frame4: 8
            5, 4,   //frame5: 9
            6, 3,   //frame6: 9
            7, 2,   //frame7: 9
            8, 1,   //frame8: 9
            9, 1,   //frame9: 11
            1, 2,   //frame10: 3
                    //total: 88
        }
    });

    BowlingMachineImpl machine;

    PlayerTable result = machine.CalcPlayerTable(playerHits);
    std::array<Frame, FramesPerGame> expectedFrames =
    { {
        { 1,{ '1', SpareSign }, 12},
        { 2,{ '2', '2'       }, 4 },
        { 3,{ '3', SpareSign }, 14},
        { 4,{ '4', '4'       }, 8 },
        { 5,{ '5', '4'       }, 9 },
        { 6,{ '6', '3'       }, 9 },
        { 7,{ '7', '2'       }, 9 },
        { 8,{ '8', '1'       }, 9 },
        { 9,{ '9', SpareSign }, 11},
        { 10,{ '1', '2'      }, 3 },
    } };


    EXPECT_EQ(result.playerName, "TestPlayer1");
    for (size_t i = 0; i < expectedFrames.size(); ++i)
    {
        EXPECT_EQ(expectedFrames[i], result.frames[i]);
    }
    EXPECT_EQ(result.total, 88);
}


///Table with strikes, but not with strike in 10th frame
TEST(bowlingMachine, simpleStrike)
{
    PlayerHits playerHits
    ({
        "TestPlayer1",
        {
            1, 1,   //frame1: 2
            10,     //frame2: 16
            3, 3,   //frame3: 6
            10,     //frame4: 26
            10,     //frame5: 19
            6, 3,   //frame6: 9
            7, 2,   //frame7: 9
            8, 1,   //frame8: 9
            8, 1,   //frame9: 9
            1, 2,   //frame10: 3
                    //total: 108
        }
    });

    BowlingMachineImpl machine;

    PlayerTable result = machine.CalcPlayerTable(playerHits);
    std::array<Frame, FramesPerGame> expectedFrames =
    { {
        { 1,{ '1', '1'   }, 2 },
        { 2,{ StrikeSign }, 16},
        { 3,{ '3', '3'   }, 6 },
        { 4,{ StrikeSign }, 26},
        { 5,{ StrikeSign }, 19},
        { 6,{ '6', '3'   }, 9 },
        { 7,{ '7', '2'   }, 9 },
        { 8,{ '8', '1'   }, 9 },
        { 9,{ '8', '1'   }, 9 },
        { 10,{ '1', '2'  }, 3 },
    } };

    EXPECT_EQ(result.playerName, "TestPlayer1");
    for (size_t i = 0; i < expectedFrames.size(); ++i)
    {
        EXPECT_EQ(expectedFrames[i], result.frames[i]);
    }
    EXPECT_EQ(result.total, 108);
}

///Spare at ten frame
TEST(bowlingMachine, tenFrameSpare)
{
    PlayerHits playerHits
    ({
        "TestPlayer1",
        {
            1, 1,   //frame1: 2
            2, 2,   //frame2: 4
            3, 3,   //frame3: 6
            4, 4,   //frame4: 8
            5, 4,   //frame5: 9
            6, 3,   //frame6: 9
            7, 2,   //frame7: 9
            8, 1,   //frame8: 9
            8, 1,   //frame9: 9
            2, 8, 5 //frame10: 15
                    //total: 80
        }
    });

    BowlingMachineImpl machine;

    PlayerTable result = machine.CalcPlayerTable(playerHits);
    std::array<Frame, FramesPerGame> expectedFrames =
    { {
        { 1,{ '1', '1' }, 2 },
        { 2,{ '2', '2' }, 4 },
        { 3,{ '3', '3' }, 6 },
        { 4,{ '4', '4' }, 8 },
        { 5,{ '5', '4' }, 9 },
        { 6,{ '6', '3' }, 9 },
        { 7,{ '7', '2' }, 9 },
        { 8,{ '8', '1' }, 9 },
        { 9,{ '8', '1' }, 9 },
        { 10,{ '2', SpareSign, '5' }, 15 },
    } };

    EXPECT_EQ(result.playerName, "TestPlayer1");
    for (size_t i = 0; i < expectedFrames.size(); ++i)
    {
        EXPECT_EQ(expectedFrames[i], result.frames[i]);
    }
    EXPECT_EQ(result.total, 80);
}

///Strike at ten frame
TEST(bowlingMachine, tenFrameStrike)
{
    PlayerHits playerHits
    ({
        "TestPlayer1",
        {
            1, 1,   //frame1: 2
            2, 2,   //frame2: 4
            3, 3,   //frame3: 6
            4, 4,   //frame4: 8
            5, 4,   //frame5: 9
            6, 3,   //frame6: 9
            7, 2,   //frame7: 9
            8, 1,   //frame8: 9
            8, 1,   //frame9: 9
            10, 4, 5 //frame10: 19
                    //total: 84
        }
    });

    BowlingMachineImpl machine;

    PlayerTable result = machine.CalcPlayerTable(playerHits);
    std::array<Frame, FramesPerGame> expectedFrames =
    { {
        { 1,{ '1', '1' }, 2 },
        { 2,{ '2', '2' }, 4 },
        { 3,{ '3', '3' }, 6 },
        { 4,{ '4', '4' }, 8 },
        { 5,{ '5', '4' }, 9 },
        { 6,{ '6', '3' }, 9 },
        { 7,{ '7', '2' }, 9 },
        { 8,{ '8', '1' }, 9 },
        { 9,{ '8', '1' }, 9 },
        { 10,{ StrikeSign, '4', '5' }, 19 },
    } };

    EXPECT_EQ(result.playerName, "TestPlayer1");
    for (size_t i = 0; i < expectedFrames.size(); ++i)
    {
        EXPECT_EQ(expectedFrames[i], result.frames[i]);
    }
    EXPECT_EQ(result.total, 84);
}

///Table with misses
TEST(bowlingMachine, miss)
{
    PlayerHits playerHits
    ({
        "TestPlayer1",
        {
            1, 1,   //frame1: 2
            2, 0,   //frame2: 2
            3, 3,   //frame3: 6
            4, 4,   //frame4: 8
            5, 4,   //frame5: 9
            6, 3,   //frame6: 9
            0, 2,   //frame7: 2
            8, 1,   //frame8: 9
            0, 10,  //frame9: 11
            1, 2,   //frame10: 3
                    //total: 61
        }
    });

    BowlingMachineImpl machine;

    PlayerTable result = machine.CalcPlayerTable(playerHits);
    std::array<Frame, FramesPerGame> expectedFrames =
    { {
        { 1,{ '1', '1' }, 2 },
        { 2,{ '2', MissSign }, 2 },
        { 3,{ '3', '3' }, 6 },
        { 4,{ '4', '4' }, 8 },
        { 5,{ '5', '4' }, 9 },
        { 6,{ '6', '3' }, 9 },
        { 7,{ MissSign, '2' }, 2 },
        { 8,{ '8', '1' }, 9 },
        { 9,{ MissSign, SpareSign }, 11 },
        { 10,{ '1', '2' }, 3 },
    } };

    EXPECT_EQ(result.playerName, "TestPlayer1");
    for (size_t i = 0; i < expectedFrames.size(); ++i)
    {
        EXPECT_EQ(expectedFrames[i], result.frames[i]);
    }
    EXPECT_EQ(result.total, 61);
}

///Complex table
TEST(bowlingMachine, complex)
{
    PlayerHits playerHits
    ({
        "TestPlayer1",
        {
            0, 10,  //frame1: 12
            2, 0,   //frame2: 2
            10,     //frame3: 18
            4, 4,   //frame4: 8
            5, 5,   //frame5: 10
            0, 3,   //frame6: 3
            7, 3,   //frame7: 20
            10,     //frame8: 19
            8, 1,   //frame9: 9
            10, 10, 10,//frame10: 30
                    //total: 131
        }
    });

    BowlingMachineImpl machine;

    PlayerTable result = machine.CalcPlayerTable(playerHits);
    std::array<Frame, FramesPerGame> expectedFrames =
    { {
        { 1,{ MissSign, SpareSign   }, 12 },
        { 2,{ '2',      MissSign    }, 2 },
        { 3,{ StrikeSign            }, 18 },
        { 4,{ '4',      '4'         }, 8 },
        { 5,{ '5',      SpareSign   }, 10 },
        { 6,{ MissSign, '3'         }, 3 },
        { 7,{ '7',      SpareSign   }, 20 },
        { 8,{ StrikeSign            }, 19 },
        { 9,{ '8',      '1'         }, 9 },
        { 10,{ StrikeSign, StrikeSign, StrikeSign }, 30 },
    } };

    EXPECT_EQ(result.playerName, "TestPlayer1");
    for (size_t i = 0; i < expectedFrames.size(); ++i)
    {
        EXPECT_EQ(expectedFrames[i], result.frames[i]);
    }
    EXPECT_EQ(result.total, 131);
}

///Max points - all strikes
TEST(bowlingMachine, maxPoints)
{
    PlayerHits playerHits
    ({
        "TestPlayer1",
        {
            10,     //frame1: 30
            10,     //frame2: 30
            10,     //frame3: 30
            10,     //frame4: 30
            10,     //frame5: 30
            10,     //frame6: 30
            10,     //frame7: 30
            10,     //frame8: 30
            10,     //frame9: 30
            10, 10, 10 //frame10: 30
                    //total: 300
        }
    });

    BowlingMachineImpl machine;

    PlayerTable result = machine.CalcPlayerTable(playerHits);
    std::array<Frame, FramesPerGame> expectedFrames =
    { {
        { 1,{ StrikeSign }, 30 },
        { 2,{ StrikeSign }, 30 },
        { 3,{ StrikeSign }, 30 },
        { 4,{ StrikeSign }, 30 },
        { 5,{ StrikeSign }, 30 },
        { 6,{ StrikeSign }, 30 },
        { 7,{ StrikeSign }, 30 },
        { 8,{ StrikeSign }, 30 },
        { 9,{ StrikeSign }, 30 },
        { 10,{StrikeSign, StrikeSign, StrikeSign }, 30 },
    } };

    EXPECT_EQ(result.playerName, "TestPlayer1");
    for (size_t i = 0; i < expectedFrames.size(); ++i)
    {
        EXPECT_EQ(expectedFrames[i], result.frames[i]);
    }
    EXPECT_EQ(result.total, 300);
}

///Min points - all misses
TEST(bowlingMachine, minPoints)
{
    PlayerHits playerHits
    ({
        "TestPlayer1",
        {
            0, 0,     //frame1: 0
            0, 0,     //frame2: 0
            0, 0,     //frame3: 0
            0, 0,     //frame4: 0
            0, 0,     //frame5: 0
            0, 0,     //frame6: 0
            0, 0,     //frame7: 0
            0, 0,     //frame8: 0
            0, 0,     //frame9: 0
            0, 0,     //frame10: 0
                      //total: 0
        }
    });

    BowlingMachineImpl machine;

    PlayerTable result = machine.CalcPlayerTable(playerHits);
    std::array<Frame, FramesPerGame> expectedFrames =
    { {
        { 1,{ MissSign, MissSign }, 0 },
        { 2,{ MissSign, MissSign }, 0 },
        { 3,{ MissSign, MissSign }, 0 },
        { 4,{ MissSign, MissSign }, 0 },
        { 5,{ MissSign, MissSign }, 0 },
        { 6,{ MissSign, MissSign }, 0 },
        { 7,{ MissSign, MissSign }, 0 },
        { 8,{ MissSign, MissSign }, 0 },
        { 9,{ MissSign, MissSign }, 0 },
        { 10,{ MissSign, MissSign }, 0 },
    } };

    EXPECT_EQ(result.playerName, "TestPlayer1");
    for (size_t i = 0; i < expectedFrames.size(); ++i)
    {
        EXPECT_EQ(expectedFrames[i], result.frames[i]);
    }
    EXPECT_EQ(result.total, 0);
}

#endif

