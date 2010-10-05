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

InputMgr::InputMgr()
{
}

InputMgr::Keys
InputMgr::operator()(const SDL_Event &event)
{
    Keys keyPressed = INV_EVT;

    switch (event.type)
    {
        case SDL_KEYDOWN:
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
            break;
        case SDL_JOYHATMOTION:
            switch (event.jhat.value)
            {
                case SDL_HAT_UP:
                    keyPressed = UP;
                    break;
                case SDL_HAT_DOWN:
                    keyPressed = DOWN;
                    break;
                case SDL_HAT_LEFT:
                    keyPressed = LEFT;
                    break;
                case SDL_HAT_RIGHT:
                    keyPressed = RIGHT;
                    break;
                default:
                    //do nothing
                    break;
            }
            break;
        case SDL_JOYAXISMOTION:
            if ( ( event.jaxis.value < -3200 ) || (event.jaxis.value > 3200 ) ) 
            {
                if( event.jaxis.axis == 0) 
                {
                    if (event.jaxis.value < 0 )
                        keyPressed = LEFT;
                    else
                        keyPressed = RIGHT;
                }

                if( event.jaxis.axis == 1) 
                {
                    if (event.jaxis.value < 0 )
                        keyPressed = UP;
                    else
                        keyPressed = DOWN;
                }
            }
            break;
        case SDL_JOYBUTTONDOWN:
            switch (event.jbutton.button)
            {
                case 0: //A
                case 2: //1
                case 3: //2
                case 9: //a
                case 10: //b
                case 11: //x
                case 12: //y
                    keyPressed = BUT_A;
                    break;
                case 6: //home
                case 19: //home classic
                    keyPressed = QUIT;
                    break;
            }
            break;
        default:
            break;
    }

    return keyPressed;
}
