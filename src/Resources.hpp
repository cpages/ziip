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
            SfcPlayer,
            SfcGameOver,
            NumSurfaces
        };

        Resources();
        ~Resources();

        void prepareBoardGraphics(int newBlockSize);
        SDL_Surface *screen();
        SDL_Surface *mainMenu();
        SDL_Surface *mmSel();
        SDL_Surface *pieces();
        SDL_Surface *board();
        SDL_Surface *player();
        SDL_Surface *gameOver();
        void getScreenSize(int &width, int &height);
        float getProportion();
        int getBlockSize();

    private:
        void prepareBGGraphics();

        std::vector<std::string> _surfaceFiles;
        std::vector<SDL_Surface *> _surfaces;
        SDL_Surface *_screen;
        SDL_Surface *_origMMenu;
        SDL_Surface *_mMenu;
        SDL_Surface *_origMMSel;
        SDL_Surface *_mmSel;
        SDL_Surface *_origPieces;
        SDL_Surface *_pieces;
        SDL_Surface *_origBoard;
        SDL_Surface *_board;
        SDL_Surface *_origPlayer;
        SDL_Surface *_player;
        SDL_Surface *_origGOver;
        SDL_Surface *_gOver;
        float _proportion;
        int _currBlockSize;
};

#endif //RESOURCES_HPP
