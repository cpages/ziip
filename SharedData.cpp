#include "SharedData.hpp"

NewPieces::NewPieces():
    _num(0)
{
    _mutex = SDL_CreateMutex();
}

NewPieces::~NewPieces()
{
    SDL_DestroyMutex(_mutex);
}
