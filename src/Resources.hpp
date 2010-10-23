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
#include "SDL.h"
#include "SDL_ttf.h"
#include "Config.hpp"
#include "InputMgr.hpp"

class Resources
{
    public:
        enum Surface
        {
            SfcScreen = 0,
            SfcMainMenu,
            SfcMPMenu,
            SfcMMSel,
            SfcPieces,
            SfcBoard,
            SfcGrid,
            SfcPlayer,
            SfcHiScores,
            NumSurfaces
        };

        Resources();
        ~Resources();

        void prepareBoardGraphics(int numPlayers);
        SDL_Surface *screen();
        SDL_Surface *getSfc(Surface sfc);
        void getScreenSize(int &width, int &height);
        float getBgScale();
        int getBlockSize();
        SDL_Rect getBoardArea(int id) const;
        SDL_Rect getGridArea(int id) const;

        SDL_Surface *renderText(const std::string &str, const SDL_Color &color);

        InputMgr::KeyPressed getKey(const SDL_Event &event);

    private:
        class FontMgr
        {
            public:
                FontMgr();
                ~FontMgr();

                SDL_Surface *renderText(const std::string &str,
                        const SDL_Color &color);

            private:
                TTF_Font *_font;
        };

        void prepareSurface(Surface sfc, float prop);
        void prepareBGGraphics();
        void divideScreen(int numPlayers);
        void fillGridRects();

        FontMgr _fontMgr;
        Config _cfg;
        int _winWidth;
        int _winHeight;
        InputMgr _inputMgr;
        std::vector<std::string> _surfaceFiles;
        std::vector<std::string> _bgFiles;
        std::vector<SDL_Surface *> _surfaces;
        float _blockScale;
        int _currBlockSize;
        std::vector<SDL_Rect> _boardAreas;
        std::vector<SDL_Rect> _gridAreas;
};

#endif //RESOURCES_HPP
