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
#include <vector>
#include <stdexcept>
#include <cassert>
#include <memory>
#include <lua.hpp>
#include "Board.hpp"
#include "Player.hpp"
#include "LuaIface.hpp"

namespace
{
    std::auto_ptr<LuaIface::LuaBot> gBots[4];
}

LuaIface::LuaBot::LuaBot(int slot, Board *board, const std::string &script):
    _lState(lua_open()),
    _board(board)
{
    if (!_lState)
        throw std::runtime_error("Error initializing lua");

    luaL_openlibs(_lState);

	lua_register(_lState, "moveRight", moveRight);

    if (luaL_loadfile (_lState, script.c_str()))
        throw std::runtime_error(lua_tostring(_lState, -1));

    if (lua_pcall(_lState, 0, 0, 0))
        throw std::runtime_error(lua_tostring(_lState, -1));
}

LuaIface::LuaBot::~LuaBot()
{
    lua_close(_lState);
}

void
LuaIface::LuaBot::move()
{
    lua_getglobal(_lState, "OnMove");
    lua_pcall(_lState, 0, 0, 0);
}

Board *
LuaIface::LuaBot::getBoard() const
{
    return _board;
}

LuaIface::LuaIface()
{
#if 0
    if (luaL_dofile(_lState, "test.lua"))
        throw std::runtime_error(lua_tostring(_lState, -1));
#endif
}

LuaIface::~LuaIface()
{
}

void
LuaIface::createBot(int slot, Board *board, const std::string &script)
{
    gBots[slot].reset(new LuaIface::LuaBot(slot, board, script));
}

void
LuaIface::move(int slot)
{
    assert (gBots[slot].get() != 0);

    gBots[slot]->move();
}

int
moveRight(lua_State *s)
{
    const int slot = lua_tonumber(s, -1);
    lua_pop(s, 1);
    Player::playerDirection mov = Player::Right;
    if (gBots[slot].get() == 0)
    {
        //TODO: deal with this
        assert (0 && "Lua accessing wrong slot");
    }
    gBots[slot]->getBoard()->movePlayer(mov);
    lua_pushnumber(s, 0 );
    return 1;
}
