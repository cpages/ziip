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
