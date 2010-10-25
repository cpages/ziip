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
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <string>

class Config
{
    public:
        Config();

        int getWinWidth() const;
        int getWinHeight() const;
        std::vector<int> getControls() const;

    private:
        static const int NumControls;

        enum ConfigSection
        {
            CSScreen,
            CSControls,
            NumSections
        };

        void createDefaultConf() const;
        void loadConfig();

        int _winWidth;
        int _winHeight;
        std::vector<int> _controls;
        std::string _confFile;
};

#endif //CONFIG_HPP
