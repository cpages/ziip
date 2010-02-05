#include <cassert>
#include <string>
#include <sstream>
#include <cstdlib>
#include <utility>
#include "SharedData.hpp"
#include "Player.hpp"
#include "Board.hpp"

const int Board::numRows = 16;
const int Board::horiRowsLen = 7;
const int Board::vertRowsLen = 5;
const int Board::rowsInScreen = 14;
const int Board::colsInScreen = 18;

namespace
{
    const SDL_Color scoreColor = {255, 0, 0};
    const int PointsPerLevel = 1000;
    const int SpeedPercentInc = 10;

    SDL_Rect
    getPlayerRect(SDL_Rect rect)
    {
        SDL_Rect ret;
        ret.x = rect.x + rect.w * Board::horiRowsLen;
        ret.y = rect.y + rect.h * Board::vertRowsLen;
        ret.w = rect.w;
        ret.h = rect.h;

        return ret;
    }

    void
    fillRowsRects(const SDL_Rect &newOrigSize, std::vector<Row> &rows)
    {
        SDL_Rect rect;
        rect.w = newOrigSize.w;
        rect.h = newOrigSize.h;

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
        int row;
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

        return row;
    }
}

Board::Board(SDL_Surface *screen, Player *player):
    _timer(1000),
    _rowLastPiece(16),
    _screen(screen),
    _player(player),
    _score(_screen)
{
    _piecesImg = SDL_LoadBMP("pieces.bmp");
    Row tmpRow(horiRowsLen, 1, 0, _screen, _piecesImg);
    _rows.resize(4, tmpRow);
    tmpRow = Row(vertRowsLen, 0, -1, _screen, _piecesImg);
    _rows.resize(8, tmpRow);
    tmpRow = Row(horiRowsLen, -1, 0, _screen, _piecesImg);
    _rows.resize(12, tmpRow);
    tmpRow = Row(vertRowsLen, 0, 1, _screen, _piecesImg);
    _rows.resize(16, tmpRow);

    _board = SDL_LoadBMP("board.bmp");
}

Board::~Board()
{
    SDL_FreeSurface(_piecesImg);
    SDL_FreeSurface(_board);
}

SDL_Rect
Board::calculateOriginAndTileSize(int width, int height)
{
    //TODO: implement this
    //const float proportion = static_cast<float>(rowsInScreen) / colsInScreen;
    SDL_Rect ret;
    ret.x = 22;
    ret.y = 6;
    ret.w = 42;
    ret.h = 42;

    return ret;
}

bool
Board::addPiece()
{
    _rowLastPiece = getRandomRow(_rowLastPiece);
    return _rows[_rowLastPiece].addPiece();
}

void
Board::playerShooted()
{
    int aimedRow = getAimedRow(_player->getPos(), _player->getDirection());
    std::pair<colors, int> result = _rows[aimedRow].shoot(_player->getColor());
    colors newColor = result.first;
    bool newLevel = _score.addPoints(result.second);
    if (newLevel)
        _timer.increaseSpeed(SpeedPercentInc);
    if (newColor != NoColor)
    {
        _player->reverse();
        _player->setColor(newColor);
    }
}

void
Board::resize(int width, int height)
{
    SDL_Rect newOrigSize = calculateOriginAndTileSize(width, height);
    SDL_Rect playerRect = getPlayerRect(newOrigSize);
    fillRowsRects(newOrigSize, _rows);
    _player->setOriginAndSize(playerRect);
}

void
Board::draw()
{
    SDL_BlitSurface(_board, NULL, _screen, NULL);
    _score.draw();
    for (int i = 0; i < numRows; i++)
    {
        _rows[i].draw();
    }
}

Board::Score::Score(SDL_Surface *screen):
    _currScore(0),
    _screen(screen)
{
    _font = TTF_OpenFont("fonts/LiberationMono-Bold.ttf", 24);
    assert (_font != NULL);
    std::string str("SCORE: 0");
    _renderedScore = TTF_RenderText_Solid(_font, str.c_str(), scoreColor);
}

Board::Score::~Score()
{
    SDL_FreeSurface(_renderedScore);
    TTF_CloseFont(_font); 
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
    SDL_BlitSurface(_renderedScore, NULL, _screen, NULL);
}
