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
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "HiScore.hpp"
#include "SharedData.hpp"
#include "Config.hpp"

namespace
{
    const int DefaultWinWidth = 640;
    const int DefaultWinHeight = 480;

    const std::string confFName("config");
}

const int Config::NumControls = 5;

Config::Config():
    _winWidth(DefaultWinWidth),
    _winHeight(DefaultWinHeight),
    _controls(NumControls, -1)
{
    const std::string confDir = getPath(FolderConf);
#ifdef GEKKO
    _controls[0] = 0;
    _controls[1] = 1;
#else
    ensureFolder(confDir);
#endif
    _confFile = confDir + confFName;

#ifndef GEKKO
    loadConfig();
#endif
}

void
Config::createDefaultConf() const
{
    std::ofstream ostr(_confFile.c_str());
    if (ostr.fail())
        throw std::runtime_error("Error creating hiscore file");

    ostr << "[controls]" << std::endl;
    ostr << "4 0" << std::endl;

    ostr.flush();
    ostr.close();
}

void
Config::loadConfig()
{
    std::ifstream fs(_confFile.c_str());
    if (fs.fail()) //if it doesn't exist
    {
        createDefaultConf();
        fs.close();
        fs.open(_confFile.c_str());
    }

    ConfigSection section = NumSections; //invalid value
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
            std::istringstream istr(line);
            int p1c, p2c;

            istr >> p1c >> p2c;

            if (istr.fail())
                throw std::runtime_error("Error parsing config. Unknown field:" + line);

            _controls[p1c] = 0;
            _controls[p2c] = 1;
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

std::vector<int>
Config::getControls() const
{
    return _controls;
}
