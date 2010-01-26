#include "SharedData.hpp"
#include "Timer.hpp"

namespace
{
    unsigned int timerCB(unsigned int interval, void *param)
    {
        NewPieces *np = reinterpret_cast<NewPieces *>(param);
        while (SDL_mutexP(np->_mutex) != 0);
        np->_num++;
        SDL_mutexV(np->_mutex);

        return interval;
    }
}

Timer::Timer(int timeout, NewPieces &newPieces):
    _timeout(timeout),
    _newPieces(newPieces)
{
    _timerID = SDL_AddTimer(_timeout, timerCB,
            reinterpret_cast<void *>(&_newPieces));
}

Timer::~Timer()
{
    SDL_RemoveTimer(_timerID);
}

void
Timer::setTimeout(int timeout)
{
    if (_timeout != timeout)
    {
        SDL_RemoveTimer(_timerID);
        _timerID = SDL_AddTimer(_timeout, timerCB,
                reinterpret_cast<void *>(&_newPieces));
        _timeout = timeout;
    }
}
