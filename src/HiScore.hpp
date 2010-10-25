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
#ifndef HI_SCORE_HPP
#define HI_SCORE_HPP

#include <vector>
#include <string>
#include "Resources.hpp"

class HiScore
{
    public:
        HiScore(Resources *rsc);
        ~HiScore();

        void addScore(int score);
        SDL_Surface *renderHiScore();

    private:
        static const int NumScores;

        void populateEmptyFile() const;

        Resources *_rsc;
        std::vector<int> _scores;
        int _lastSet;
        std::string _hiscoreFile;
};

#endif //HI_SCORE_HPP
