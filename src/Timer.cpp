#include <iostream>
#include "SharedData.hpp"
#include "Timer.hpp"

namespace
{
    const bool debug = false;

    unsigned int timerCB(unsigned int interval, void *param)
    {
        SDL_Event event;
        //TODO: use a specific user event
        event.type = SDL_USEREVENT;
        SDL_PushEvent(&event);

        return interval;
    }
}

Timer::Timer(int timeout):
    _timeout(timeout)
{
    _timerID = SDL_AddTimer(_timeout, timerCB, NULL);
}

Timer::~Timer()
{
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
        //TODO: perhaps it would be cleaner to set the new timeout in the CB
        SDL_RemoveTimer(_timerID);
        _timerID = SDL_AddTimer(_timeout, timerCB, NULL);
        _timeout = timeout;
        if (debug)
            std::cout << "Timer set to: " << _timeout << std::endl;
    }
}
