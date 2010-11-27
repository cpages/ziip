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
#include "Player.hpp"
#include "Board.hpp"

const int Board::numRows = HORI_ROWS + VERT_ROWS;
const int Board::horiRowsLen = HORI_ROWS_LEN;
const int Board::vertRowsLen = VERT_ROWS_LEN;

namespace
{
    const bool debug = false;

    const SDL_Color scoreColor = {255, 0, 0};
    const int PointsPerLevel = 1000;
    const int SpeedPercentInc = 10;
    const int InitialTimeout = 1000;
    const int basePoints = 100;

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

    int
    countPoints(int ziiped)
    {
        int points = 0;
        for (int i = 0; i < ziiped; ++i)
            points += basePoints * (i+1);
        return points;
    }
}

Board::Board(int id, Resources *rsc):
    _id(id),
    _rsc(rsc),
    _timer(id, InitialTimeout),
    _player(rsc),
    _rowLastPiece(16),
    _score(id, rsc),
    _sentPieces(0),
    _gameOver(false),
    _pendZiiped(0)
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
    if (_gameOver)
        return true;
    _rowLastPiece = getRandomRow(_rowLastPiece);
    if (debug)
        std::cout << "Piece at row " << _rowLastPiece << std::endl;
    return _rows[_rowLastPiece].addPiece();
}

void
Board::movePlayer(Player::playerDirection mov)
{
    if (_gameOver)
        return;
    _player.move(mov);
}

void
Board::playerShooted()
{
    if (_gameOver)
        return;
    int aimedRow = getAimedRow(_player.getPos(), _player.getDirection());
    std::pair<Color, int> result = _rows[aimedRow].shoot(_player.getColor());
    const Color newColor = result.first;
    const int ziiped = result.second;

    // evt to send pieces to other players in deathmatch
    SDL_Event event;
    event.type = EVT_PIECE;
    event.user.code = _id;
    _pendZiiped += ziiped;
    event.user.data1 = &_pendZiiped;
    SDL_PushEvent(&event);

    // to send pieces in net game
    _sentPieces += ziiped;

    bool newLevel = _score.addPoints(countPoints(ziiped));
    if (newLevel)
        _timer.increaseSpeed(SpeedPercentInc);
    if (newColor != NoColor)
    {
        _player.reverse();
        _player.setColor(newColor);
    }
}

int
Board::getScore() const
{
    return _score.getScore();
}

void
Board::gameOver()
{
    _timer.stop();
    _gameOver = true;
}

Board::State
Board::getState() const
{
    State state;
    assert (int(_rows.size()) == numRows);
    int pos = 0;
    for (int i = 0; i < numRows; ++i)
    {
        const std::vector<Color> rs = _rows[i].getState();
        for (unsigned int j = 0; j < rs.size(); ++j)
        {
            state.colsStat[pos] = rs[j];
            pos++;
        }
    }
    state.playerPos = _player.getPos();
    state.playerDir = _player.getDirection();
    state.playerColor = _player.getColor();
    state.score = getScore();
    state.sentPieces = _sentPieces;
    state.gameOver = _gameOver;

    return state;
}

void
Board::setState(const State &state)
{
    assert (int(_rows.size()) == numRows);
    const Color *pos = state.colsStat;
    // this is a bit dirty, but should work
    for (int i = 0; i < 4; ++i, pos+=horiRowsLen)
        _rows[i].setState(pos);
    for (int i = 4; i < 8; ++i, pos+=vertRowsLen)
        _rows[i].setState(pos);
    for (int i = 8; i < 12; ++i, pos+=horiRowsLen)
        _rows[i].setState(pos);
    for (int i = 12; i < 16; ++i, pos+=vertRowsLen)
        _rows[i].setState(pos);
    _player.setPos(state.playerPos);
    _player.setDirection(state.playerDir);
    _player.setColor(state.playerColor);
    _score.setScore(state.score);
    _sentPieces = state.sentPieces;
    _gameOver = state.gameOver;
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
    if (_gameOver)
    {
        const std::string goStr("Game Over");
        const SDL_Color goCol = {255, 255, 255};
        SDL_Surface *go = _rsc->renderText(goStr, goCol);
        SDL_Rect dst = rect;
        dst.x += dst.w/2 - 60;
        dst.y += 20;
        SDL_BlitSurface(go, NULL, _rsc->screen(), &dst);
        SDL_FreeSurface(go);
    }
}

Board::Score::Score(int id, Resources *rsc):
    _id(id),
    _rsc(rsc),
    _currScore(0)
{
    const std::string str("SCORE: 0");
    _renderedScore = _rsc->renderText(str, scoreColor);
}

Board::Score::~Score()
{
    SDL_FreeSurface(_renderedScore);
}

void
Board::Score::reset()
{
    _currScore = 0;
    SDL_FreeSurface(_renderedScore);
    std::ostringstream sstr;
    sstr << "SCORE: " << _currScore;
    _renderedScore = _rsc->renderText(sstr.str(), scoreColor);
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
    std::ostringstream sstr;
    sstr << "SCORE: " << _currScore;
    _renderedScore = _rsc->renderText(sstr.str(), scoreColor);
    return nextLevel;
}

int
Board::Score::getScore() const
{
    return _currScore;
}

void
Board::Score::setScore(int score)
{
    _currScore = score;
    addPoints(0); //dirty hack to force repaint
}

void
Board::Score::draw()
{
    SDL_Rect dst = _rsc->getBoardArea(_id);
    SDL_BlitSurface(_renderedScore, NULL, _rsc->screen(), &dst);
}
