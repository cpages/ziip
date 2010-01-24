#ifndef ROW_HPP
#define ROW_HPP

class Row
{
    public:
        enum pieces
        {
            red,
            green,
            blue,
            purple
        };

        Row(int size);

    private:
        int _size;
};

#endif //ROW_HPP
