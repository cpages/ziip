#ifndef ROW_HPP
#define ROW_HPP

#include <vector>
#include "SharedData.hpp"

class Row
{
    public:
        Row(int size, int growX, int growY, SDL_Surface *screen,
                SDL_Surface *pieces);

        bool addPiece();
        colors shoot(colors playerColor);
        void setOriginAndSize(SDL_Rect rect);
        void draw();

    private:
        int _size;
        coord _growDir;
        SDL_Rect _rect;
        std::vector<colors> _pieces;
        SDL_Surface *_screen;
        SDL_Surface *_piecesImg;
};

#endif //ROW_HPP
