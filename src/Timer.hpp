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
#ifndef TIMER_HPP
#define TIMER_HPP

#include "SDL/SDL.h"
#include "SharedData.hpp"

class Timer
{
    public:
        Timer(int timeout);
        ~Timer();

        void increaseSpeed(int percentage);
        void setTimeout(int timeout);

    private:
        int _timeout;
        SDL_TimerID _timerID;
};

#endif //TIMER_HPP
