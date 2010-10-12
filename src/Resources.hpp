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
#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include <vector>
#include <string>
#include "SDL/SDL.h"

class Resources
{
    public:
        enum Surface
        {
            SfcScreen = 0,
            SfcMainMenu,
            SfcMMSel,
            SfcPieces,
            SfcBoard,
            SfcGrid,
            SfcPlayer,
            SfcGameOver,
            NumSurfaces
        };

        Resources();
        ~Resources();

        void prepareBoardGraphics(int numPlayers);
        SDL_Surface *screen();
        SDL_Surface *getSfc(Surface sfc);
        void getScreenSize(int &width, int &height);
        float getProportion();
        int getBlockSize();
        SDL_Rect getBoardArea(int id) const;
        SDL_Rect getGridArea(int id) const;

    private:
        void prepareSurface(Surface sfc, float prop);
        void prepareBGGraphics();
        void divideScreen(int numPlayers);
        void fillGridRects();

        int _winWidth;
        int _winHeight;
        std::vector<std::string> _surfaceFiles;
        std::vector<std::string> _bgFiles;
        std::vector<SDL_Surface *> _surfaces;
        float _proportion;
        int _currBlockSize;
        std::vector<SDL_Rect> _boardAreas;
        std::vector<SDL_Rect> _gridAreas;
};

#endif //RESOURCES_HPP
