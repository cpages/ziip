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
#include <iostream>
#include "Resources.hpp"
#include "Menu.hpp"

namespace
{
    //for the current font size, this is the size of any generated surface
    //this could be dinamically set, but for now it's enough
    const int textHeight = 28;
    const int selWidth = 26;
    const int optsSpace = 50;
}

//TODO: for the moment, I forget about the title
Menu::Menu(Resources *rsc, const std::string &title,
        const std::vector<std::string> &opts):
    _rsc(rsc),
    _selOpt(0)
{
    const SDL_Color goCol = {255, 255, 255};

    int scrW, scrH;
    _rsc->getScreenSize(scrW, scrH);
    const int top = scrH * 60 / 100 - (optsSpace * opts.size())/2 -
        textHeight/2;

    _nOpts = int(opts.size());
    std::vector<SDL_Surface *> optsText(_nOpts);

    int maxWidth = 0;
    for (int i = 0; i < _nOpts; ++i)
    {
        optsText[i] = _rsc->renderText(opts[i], goCol);
        maxWidth = std::max(maxWidth, optsText[i]->w);
    }

    SDL_Rect rect;
    rect.x = scrW / 2 - maxWidth / 2 + selWidth;
    rect.y = top;

    _menu = _rsc->getSfc(Resources::SfcMainMenu);
    for (int i = 0; i < _nOpts; ++i)
    {
        SDL_BlitSurface(optsText[i], NULL, _menu, &rect);
        SDL_FreeSurface(optsText[i]);
        rect.x -= optsSpace;
        _selOptRect.push_back(rect);
        rect.x += optsSpace;
        rect.y += optsSpace;
    }
}

int
Menu::operator()()
{
    SDL_Event event;

    int option = -1;
    while (option == -1)
    {
        SDL_BlitSurface(_menu, NULL, _rsc->screen(), NULL);
        SDL_BlitSurface(_rsc->getSfc(Resources::SfcMMSel), NULL,
                _rsc->screen(), &_selOptRect[_selOpt]);
        SDL_Flip(_rsc->screen());

        if (SDL_WaitEvent(&event))
        {
            InputMgr::KeyPressed keyPress;
            keyPress = _rsc->getKey(event);
            if (keyPress.key != InputMgr::INV_EVT)
            {
                switch (keyPress.key)
                {
                    case InputMgr::UP:
                        if (_selOpt > 0)
                            _selOpt--;
                        break;
                    case InputMgr::DOWN:
                        if (_selOpt < _nOpts - 1)
                            _selOpt++;
                        break;
                    case InputMgr::BUT_A:
                        option = _selOpt;
                        break;
                    case InputMgr::QUIT:
                        option = -2;
                        break;
                    default:
                        break;
                }
            }
            else if (event.type == SDL_QUIT)
            {
                option = -2;
            }
        }
    }

    return option;
}
