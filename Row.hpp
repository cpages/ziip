#ifndef ROW_HPP
#define ROW_HPP

#include <vector>
#include "SharedData.hpp"

class Row
{
    public:
        Row(int size, int growX, int growY);

        void setOrigin(int x, int y);
        void draw();

    private:
        int _size;
        std::vector<colors> _pieces;
        coord _growDir;
        coord _origin;
};

#endif //ROW_HPP
