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
#include <stdexcept>
#include <cassert>
// for filesystem checks
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef linux
#include <unistd.h>
#endif
#include "SharedData.hpp"

namespace
{
#ifdef GEKKO
    const std::string dataFolder("sd:/apps/ziip/data/");
    const std::string fontFolder("sd:/apps/ziip/fonts/");
#elif defined _WIN32 && !defined USE_LOCAL
    const std::string dataFolder(".\\data\\");
    const std::string fontFolder(".\\fonts\\");
#else
#ifdef USE_LOCAL
    const std::string dataFolder(BUILD_DATA_FOLDER);
    const std::string fontFolder(BUILD_FONT_FOLDER);
#else
    const std::string dataFolder(INSTALL_DATA_FOLDER);
    const std::string fontFolder(INSTALL_FONT_FOLDER);
#endif
#endif

    bool
    checkPath(const std::string &path)
    {
#ifndef _WIN32 //still not implemented under win
        struct stat statBuf;

        if (stat(path.c_str(), &statBuf) < 0) 
            return false;

        return S_ISDIR(statBuf.st_mode);
#else
        assert (0 && "not implemented");
        return false;
#endif
    }
}

std::string
getPath(Folder folder)
{
    switch (folder)
    {
        case FolderData:
            return dataFolder;
            break;
        case FolderFont:
            return fontFolder;
            break;
        case FolderConf:
#ifdef GEKKO
            const std::string confDir("sd:/apps/ziip/user/");
#elif defined _WIN32
            const std::string confDir(".\\user\\");
#else
            const std::string homeDir(getenv("HOME"));
            const std::string confDir = homeDir + std::string("/.ziip/");
#endif
            return confDir;
            break;
    }
}

void
ensureFolder(const std::string &path)
{
#ifndef _WIN32 //still not implemented under win
    if (!checkPath(path))
        if (mkdir(path.c_str(), S_IRWXU) == -1)
            throw std::runtime_error("Error creating conf. folder");
#endif 
}
