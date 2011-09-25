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
#ifndef LUA_IFACE_HPP
#define LUA_IFACE_HPP

class lua_State;
class Board;

class LuaIface
{
    public:
        class LuaBot
        {
            public:
                LuaBot(int slot, Board *board, const std::string &script);
                ~LuaBot();

                void move();
                Board * getBoard() const;

            private:
                lua_State *_lState;
                Board *_board;
        };

        LuaIface();
        ~LuaIface();

        void createBot(int slot, Board *board, const std::string &script);
        void move(int slot);

    private:
};

int moveUp(lua_State *s);
int moveDown(lua_State *s);
int moveLeft(lua_State *s);
int moveRight(lua_State *s);
int shoot(lua_State *s);

#endif //LUA_IFACE_HPP
