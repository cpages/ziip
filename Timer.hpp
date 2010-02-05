#ifndef TIMER_HPP
#define TIMER_HPP

#include "SDL.h"
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
