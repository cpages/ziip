#ifndef TIMER_HPP
#define TIMER_HPP

#include "SDL.h"
#include "SharedData.hpp"

class Timer
{
    public:
        Timer(int timeout, NewPieces &newPieces);
        ~Timer();

        void setTimeout(int timeout);

    private:
        int _timeout;
        NewPieces &_newPieces;
        SDL_TimerID _timerID;
};

#endif //TIMER_HPP
