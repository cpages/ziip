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
}

Board::Board(SDL_Surface *screen, Player *player):
    _screen(screen),
    _player(player)
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
    for (int i = 0; i < numRows; i++)
    {
        _rows[i].draw();
    }
}
