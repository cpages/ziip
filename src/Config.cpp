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
#include <string>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "Config.hpp"

namespace
{
    const int DefaultWinWidth = 640;
    const int DefaultWinHeight = 480;
}

Config::Config():
    _winWidth(DefaultWinWidth),
    _winHeight(DefaultWinHeight)
{
#ifdef GEKKO
    const std::string path("sd:/apps/ziip/user/");
#else
    const std::string homeDir(getenv("HOME"));
    const std::string path = homeDir + std::string("/.ziip/");
#endif
    const std::string confName("config");

    const std::string confFile = path + confName;
    loadConfig(confFile);
}

void
Config::loadConfig(const std::string fName)
{
    std::ifstream fs(fName.c_str());

    ConfigSection section;
    while (fs.good())
    {
        std::string line;
        getline(fs,line);

        if (!fs.good())
            break;

        if (line.length() == 0 ||
                line[0] == '#' ||
                line[0] == '\r') // DOS files in unix
        {
            // comment in the file. Do nothing.
        }
        else if (line[0] == '[') //section
        {
            if (line.find("[screen]") == 0)
                section = CSScreen;
            else if (line.find("[controls]") == 0)
                section = CSControls;
            else
                throw std::runtime_error("Error parsing config file: " + line);
        }
        else if(section == CSScreen) //screen params
        {
            std::istringstream istr(line);

            istr >> _winWidth >> _winHeight;

            if (istr.fail())
                throw std::runtime_error("Error parsing config. Unknown field:" + line);
        }
        else if (section == CSControls) //controls def.
        {
        }
        else
        {
            throw std::runtime_error("Error parsing config file: " + line);
        }
    }
}

int
Config::getWinWidth() const
{
    return _winWidth;
}

int
Config::getWinHeight() const
{
    return _winHeight;
}
