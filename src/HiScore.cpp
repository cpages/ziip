/*
    Copyright 2010 Carles Pagès

    This file is part of Ziip.

    Ziip is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Ziip is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ziip.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <string>
#include <fstream>
#include <cassert>
#include <algorithm>
#include "HiScore.hpp"

namespace
{
    const bool debug = true;
    const int hiscoreVersion = 1;

#ifdef GEKKO
    const std::string hiscoreFolder("sd:/apps/ziip/user/");
#else
    const std::string hiscoreFolder("/home/page/.ziip/");
#endif
    const std::string hiscoreName("hiscore.dat");
}

const int HiScore::NumScores = 10;

HiScore::HiScore(Resources *rsc):
    _rsc(rsc),
    _scores(NumScores)
{
    const std::string hiscoreFile = hiscoreFolder + hiscoreName;
    std::ifstream fsScore(hiscoreFile.c_str());

    //extract version
    int tmp;
    fsScore >> tmp;
    assert (tmp == 1);

    if (tmp == 1)
    {
        //check number of scores
        fsScore >> tmp;
        assert (tmp == NumScores);
        for (int i = 0; i < NumScores; ++i)
            fsScore >> _scores[i];
    }
}

HiScore::~HiScore()
{
    const std::string hiscoreFile = hiscoreFolder + hiscoreName;
    std::ofstream fsScore(hiscoreFile.c_str());

    //put version
    fsScore << hiscoreVersion << std::endl;
    //put number of scores
    fsScore << NumScores << std::endl;
    for (int i = 0; i < NumScores; ++i)
        fsScore << _scores[i] << std::endl;
}

void
HiScore::addScore(int score)
{
    int i = NumScores - 1;
    if (score > _scores[i])
    {
        while (i > 0 && score > _scores[i-1])
        {
            _scores[i] = _scores[i-1];
            i--;
        }
        _scores[i] = score;
    }
}

void
HiScore::renderHiScore()
{
    if (debug)
    {
        std::cout << "High scores:" << std::endl;
        for (int i = 0; i < NumScores; ++i)
            std::cout << _scores[i] << std::endl;
    }
}
