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
#include <cassert>
#include <string>
#include <sstream>
#include <cstdlib>
#include <utility>
#include "SharedData.hpp"
#include "Resources.hpp"
#include "Player.hpp"
#include "Board.hpp"

const int Board::numRows = 16;
const int Board::horiRowsLen = 7;
const int Board::vertRowsLen = 5;

namespace
{
    const bool debug = false;

    const SDL_Color scoreColor = {255, 0, 0};
    const int PointsPerLevel = 1000;
    const int SpeedPercentInc = 10;
    const int InitialTimeout = 1000;
#ifdef GEKKO
    const std::string fontFolder("sd:/apps/ziip/fonts/");
#else
    const std::string fontFolder("fonts/");
#endif

    SDL_Rect
    getPlayerRect(SDL_Rect rect, int bSize)
    {
        SDL_Rect ret;
        ret.x = rect.x + bSize * Board::horiRowsLen;
        ret.y = rect.y + bSize * Board::vertRowsLen;
        ret.w = bSize;
        ret.h = bSize;

        return ret;
    }

    void
    fillRowsRects(const SDL_Rect &newOrigSize, int bSize, std::vector<Row> &rows)
    {
        SDL_Rect rect;
        rect.w = bSize;
        rect.h = bSize;

        rect.x = newOrigSize.x;
        for (int i = 0; i < 4; i++)
        {
            rect.y = newOrigSize.y + (5 + i) * rect.h;
            rows[i].setOriginAndSize(rect);
        }
        rect.y = newOrigSize.y + 13 * rect.h;
        for (int i = 0; i < 4; i++)
        {
            rect.x = newOrigSize.x + (7 + i) * rect.w;
            rows[i+4].setOriginAndSize(rect);
        }
        rect.x = newOrigSize.x + 17 * rect.w;
        for (int i = 0; i < 4; i++)
        {
            rect.y = newOrigSize.y + (8 - i) * rect.h;
            rows[i+8].setOriginAndSize(rect);
        }
        rect.y = newOrigSize.y;
        for (int i = 0; i < 4; i++)
        {
            rect.x = newOrigSize.x + (10 - i) * rect.w;
            rows[i+12].setOriginAndSize(rect);
        }
    }

    int
    getRandomRow(int rowLastPiece)
    {
        //avoid having two consecutive pieces from the same row
        int ret = rand() % 16;
        while (ret == rowLastPiece)
            ret = rand() % 16;
        return ret;
    }

    int
    getAimedRow(Player::playerPos pos, Player::playerDirection dir)
    {
        int row = -1;
        switch (dir)
        {
            case Player::Up:
                switch (pos.x)
                {
                    case 0:
                        row = 15;
                        break;
                    case 1:
                        row = 14;
                        break;
                    case 2:
                        row = 13;
                        break;
                    case 3:
                        row = 12;
                        break;
                }
                break;
            case Player::Down:
                switch (pos.x)
                {
                    case 0:
                        row = 4;
                        break;
                    case 1:
                        row = 5;
                        break;
                    case 2:
                        row = 6;
                        break;
                    case 3:
                        row = 7;
                        break;
                }
                break;
            case Player::Left:
                switch (pos.y)
                {
                    case 0:
                        row = 0;
                        break;
                    case 1:
                        row = 1;
                        break;
                    case 2:
                        row = 2;
                        break;
                    case 3:
                        row = 3;
                        break;
                }
                break;
            case Player::Right:
                switch (pos.y)
                {
                    case 0:
                        row = 11;
                        break;
                    case 1:
                        row = 10;
                        break;
                    case 2:
                        row = 9;
                        break;
                    case 3:
                        row = 8;
                        break;
                }
                break;
            default:
                break;
        }

        assert (row != -1);

        return row;
    }
}

Board::Board(int id, Resources *rsc):
    _id(id),
    _rsc(rsc),
    _timer(id, InitialTimeout),
    _player(rsc),
    _rowLastPiece(16),
    _score(id, rsc)
{
    Row tmpRow(horiRowsLen, 1, 0, _rsc);
    _rows.resize(4, tmpRow);
    tmpRow = Row(vertRowsLen, 0, -1, _rsc);
    _rows.resize(8, tmpRow);
    tmpRow = Row(horiRowsLen, -1, 0, _rsc);
    _rows.resize(12, tmpRow);
    tmpRow = Row(vertRowsLen, 0, 1, _rsc);
    _rows.resize(16, tmpRow);

    SDL_Rect gridRect = _rsc->getGridArea(id);
    fillRowsRects(gridRect, _rsc->getBlockSize(), _rows);
    const SDL_Rect playerRect = getPlayerRect(gridRect, _rsc->getBlockSize());
    _player.setOriginAndSize(playerRect);
}

void
Board::clear()
{
    _timer.setTimeout(InitialTimeout);
    _player.reset();
    for (int i = 0; i < numRows; i++)
    {
        _rows[i].clear();
    }
    _rowLastPiece = 16;
    _score.reset();
}

bool
Board::addPiece()
{
    _rowLastPiece = getRandomRow(_rowLastPiece);
    if (debug)
        std::cout << "Piece at row " << _rowLastPiece << std::endl;
    return _rows[_rowLastPiece].addPiece();
}

void
Board::movePlayer(Player::playerDirection mov)
{
    _player.move(mov);
}

void
Board::playerShooted()
{
    int aimedRow = getAimedRow(_player.getPos(), _player.getDirection());
    std::pair<Color, int> result = _rows[aimedRow].shoot(_player.getColor());
    Color newColor = result.first;
    bool newLevel = _score.addPoints(result.second);
    if (newLevel)
        _timer.increaseSpeed(SpeedPercentInc);
    if (newColor != NoColor)
    {
        _player.reverse();
        _player.setColor(newColor);
    }
}

void
Board::draw()
{
    SDL_Rect rect = _rsc->getBoardArea(_id);
    SDL_BlitSurface(_rsc->getSfc(Resources::SfcBoard), &rect,
            _rsc->screen(), &rect);
    _player.draw();
    _score.draw();
    for (int i = 0; i < numRows; i++)
    {
        _rows[i].draw();
    }
}

Board::Score::Score(int id, Resources *rsc):
    _id(id),
    _rsc(rsc),
    _currScore(0)
{
    std::string fontFile = fontFolder;
    fontFile.append("LiberationMono-Bold.ttf");
    _font = TTF_OpenFont(fontFile.c_str(), 24);
    assert (_font != NULL);
    std::string str("SCORE: 0");
    _renderedScore = TTF_RenderText_Solid(_font, str.c_str(), scoreColor);
}

Board::Score::~Score()
{
    SDL_FreeSurface(_renderedScore);
    TTF_CloseFont(_font); 
}

void
Board::Score::reset()
{
    _currScore = 0;
    SDL_FreeSurface(_renderedScore);
    std::ostringstream str;
    str << "SCORE: " << _currScore;
    _renderedScore = TTF_RenderText_Solid(_font, str.str().c_str(), scoreColor);
}

bool
Board::Score::addPoints(int points)
{
    bool nextLevel = false;
    int levelVal = _currScore / PointsPerLevel;
    _currScore += points;
    if ((_currScore/PointsPerLevel) != levelVal)
        nextLevel = true;
    SDL_FreeSurface(_renderedScore);
    std::ostringstream str;
    str << "SCORE: " << _currScore;
    _renderedScore = TTF_RenderText_Solid(_font, str.str().c_str(), scoreColor);
    return nextLevel;
}

void
Board::Score::draw()
{
    SDL_Rect dst = _rsc->getBoardArea(_id);
    SDL_BlitSurface(_renderedScore, NULL, _rsc->screen(), &dst);
}
