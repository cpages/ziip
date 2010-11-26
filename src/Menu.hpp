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
#ifndef MENU_HPP
#define MENU_HPP

#include <vector>
#include <string>
#include "SDL.h"

//forward declaration
class Resources;

class Menu
{
    public:
        Menu(Resources *rsc, const std::string &title,
                const std::vector<std::string> &opts);

        int operator()();

    private:
        Resources *_rsc;
        int _nOpts;
        int _selOpt;
        std::vector<SDL_Rect> _selOptRect;
        SDL_Surface *_menu;
};

#endif //MENU_HPP
