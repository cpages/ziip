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
#include "InputMgr.hpp"

InputMgr::InputMgr(const std::vector<int> &controls):
    _controls(controls)
{
}

InputMgr::KeyPressed
InputMgr::operator()(const SDL_Event &event)
{
    KeyPressed kp;
    kp.playerId = -1;
    kp.key = INV_EVT;

    switch (event.type)
    {
        case SDL_KEYDOWN:
            kp.playerId = _controls[4];
            switch (event.key.keysym.sym)
            {
                case SDLK_UP:
                    kp.key = UP;
                    break;
                case SDLK_DOWN:
                    kp.key = DOWN;
                    break;
                case SDLK_LEFT:
                    kp.key = LEFT;
                    break;
                case SDLK_RIGHT:
                    kp.key = RIGHT;
                    break;
                case SDLK_SPACE:
                case SDLK_RETURN:
                    kp.key = BUT_A;
                    break;
                case SDLK_q:
                    kp.key = QUIT;
                    break;
                default:
                    //do nothing
                    break;
            }
            break;
        case SDL_JOYHATMOTION:
            kp.playerId = _controls[event.jhat.which];
            switch (event.jhat.value)
            {
                case SDL_HAT_UP:
                    kp.key = UP;
                    break;
                case SDL_HAT_DOWN:
                    kp.key = DOWN;
                    break;
                case SDL_HAT_LEFT:
                    kp.key = LEFT;
                    break;
                case SDL_HAT_RIGHT:
                    kp.key = RIGHT;
                    break;
                default:
                    //do nothing
                    break;
            }
            break;
        case SDL_JOYAXISMOTION:
            kp.playerId = _controls[event.jaxis.which];
            if ( ( event.jaxis.value < -3200 ) || (event.jaxis.value > 3200 ) ) 
            {
                if( event.jaxis.axis == 0) 
                {
                    if (event.jaxis.value < 0 )
                        kp.key = LEFT;
                    else
                        kp.key = RIGHT;
                }

                if( event.jaxis.axis == 1) 
                {
                    if (event.jaxis.value < 0 )
                        kp.key = UP;
                    else
                        kp.key = DOWN;
                }
            }
            break;
        case SDL_JOYBUTTONDOWN:
            kp.playerId = _controls[event.jbutton.which];
            switch (event.jbutton.button)
            {
                case 0: //A
                case 2: //1
                case 3: //2
                case 9: //a
                case 10: //b
                case 11: //x
                case 12: //y
                    kp.key = BUT_A;
                    break;
                case 6: //home
                case 19: //home classic
                    kp.key = QUIT;
                    break;
            }
            break;
        default:
            break;
    }

    return kp;
}
