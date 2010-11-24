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
#include <sstream>
#include <cassert>
#include <algorithm>
#include <cmath>
#include "HiScore.hpp"

namespace
{
    const bool debug = false;
    const int hiscoreVersion = 1;

#ifdef GEKKO
    const std::string hiscoreFolder("sd:/apps/ziip/user/");
#else
    const std::string hiscoreFolder("/home/amqs/.ziip/");
#endif
    const std::string hiscoreName("hiscore.dat");
}

const int HiScore::NumScores = 10;

HiScore::HiScore(Resources *rsc):
    _rsc(rsc),
    _scores(NumScores),
    _lastSet(NumScores) //invalid pos
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
        _lastSet = i;
    }
    else
        _lastSet = NumScores;
}

SDL_Surface *
HiScore::renderHiScore()
{
    if (debug)
    {
        std::cout << "High scores:" << std::endl;
        for (int i = 0; i < NumScores; ++i)
            std::cout << _scores[i] << std::endl;
    }
    // dirty trick to copy sfc
    SDL_Surface *hiScoreSfc =
        SDL_DisplayFormatAlpha(_rsc->getSfc(Resources::SfcHiScores));
    int width, height;
    _rsc->getScreenSize(width, height);
    const int yInc = std::floor(float(height)/(NumScores+2));
    SDL_Rect dst;
    dst.x = std::floor(float(width)/4);
    dst.y = yInc/2;
    const std::string hsStr("High Scores:");
    const SDL_Color col = {255, 255, 255};
    const SDL_Color hsCol = {255, 0, 0};
    SDL_Surface *sfc = _rsc->renderText(hsStr, col);
    SDL_BlitSurface(sfc, NULL, hiScoreSfc, &dst);
    SDL_FreeSurface(sfc);
    for (int i = 0; i < NumScores; ++i)
    {
        std::ostringstream sstr;
        sstr << i << ". " << _scores[i];
        sfc = _rsc->renderText(sstr.str(), i==_lastSet?hsCol:col);
        dst.y += yInc;
        SDL_BlitSurface(sfc, NULL, hiScoreSfc, &dst);
        SDL_FreeSurface(sfc);
    }

    return hiScoreSfc;
}
