#include "InputMgr.hpp"

InputMgr::InputMgr()
{
}

InputMgr::Keys
InputMgr::operator()(const SDL_Event &event)
{
    Keys keyPressed = INV_EVT;

    if( event.type == SDL_KEYDOWN )
    {
        switch (event.key.keysym.sym)
        {
            case SDLK_UP:
                keyPressed = UP;
                break;
            case SDLK_DOWN:
                keyPressed = DOWN;
                break;
            case SDLK_LEFT:
                keyPressed = LEFT;
                break;
            case SDLK_RIGHT:
                keyPressed = RIGHT;
                break;
            case SDLK_SPACE:
            case SDLK_RETURN:
                keyPressed = BUT_A;
                break;
            case SDLK_q:
                keyPressed = QUIT;
                break;
            default:
                //do nothing
                break;
        }
    }

    return keyPressed;
}
