#ifndef ROW_HPP
#define ROW_HPP

#include <vector>
#include <utility>
#include "SharedData.hpp"

class Row
{
    public:
        Row(int size, int growX, int growY, Resources *rsc);

        void clear();
        bool addPiece();
        std::pair<colors, int> shoot(colors playerColor);
        void setOriginAndSize(SDL_Rect rect);
        void draw();

    private:
        Resources *_rsc;
        int _size;
        coord _growDir;
        SDL_Rect _rect;
        std::vector<colors> _pieces;
};

#endif //ROW_HPP
