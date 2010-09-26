#ifndef INPUT_MGR_HPP
#define INPUT_MGR_HPP

#include "SDL/SDL.h"

class InputMgr
{
    public:
        enum Keys
        {
            INV_EVT,    //non-input evt
            UP,
            DOWN,
            LEFT,
            RIGHT,
            BUT_A,
            QUIT
        };

        InputMgr();

        Keys operator()(const SDL_Event &event);
};

#endif //INPUT_MGR_HPP
