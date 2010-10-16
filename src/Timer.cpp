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
#include <sstream>
#include <cassert>
#include <stdexcept>
#include "SharedData.hpp"
#include "Timer.hpp"

namespace
{
    const bool debug = false;

    unsigned int timerCB(unsigned int interval, void *param)
    {
        SDL_Event event;
        event.type = EVT_TIMER;
        event.user.code = *static_cast<int *>(param);
        SDL_PushEvent(&event);

        return interval;
    }
}

Timer::Timer(int id, int timeout):
    _id(id),
    _timeout(timeout),
    _running(false)
{
    _timerID = SDL_AddTimer(_timeout, timerCB, static_cast<void *>(&_id));
    if (_timerID)
    {
        _running = true;
    }
    else
    {
        std::ostringstream msg;
        msg << "Error creating timer: " << SDL_GetError();
        throw std::runtime_error(msg.str());
    }
}

Timer::~Timer()
{
    if (_running)
        SDL_RemoveTimer(_timerID);
}

void
Timer::increaseSpeed(int percentage)
{
    const int decrease = _timeout * percentage / 100;
    setTimeout(_timeout - decrease);
}

void
Timer::setTimeout(int timeout)
{
    if (_timeout != timeout)
    {
        SDL_RemoveTimer(_timerID);
        _timeout = timeout;
        _timerID = SDL_AddTimer(_timeout, timerCB, static_cast<void *>(&_id));
        if (debug)
            std::cout << "Timer set to: " << _timeout << std::endl;
    }
}

void
Timer::stop()
{
    if (_running)
    {
        SDL_RemoveTimer(_timerID);
        _running = false;
    }
}
