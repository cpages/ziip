#ifndef SHARED_DATA_HPP
#define SHARED_DATA_HPP

#include "SDL.h"
#include "SDL_thread.h"

struct NewPieces
{
    NewPieces();
    ~NewPieces();

    SDL_mutex *_mutex;
    int _num;
};

#endif //SHARED_DATA_HPP
