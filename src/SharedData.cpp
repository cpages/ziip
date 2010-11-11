#include <stdexcept>
// for filesystem checks
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "SharedData.hpp"

namespace
{
#ifdef GEKKO
    const std::string dataFolder("sd:/apps/ziip/data/");
    const std::string fontFolder("sd:/apps/ziip/fonts/");
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
        struct stat statBuf;

        if (stat(path.c_str(), &statBuf) < 0) 
            return false;

        return S_ISDIR(statBuf.st_mode);
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
    if (!checkPath(path))
        if (mkdir(path.c_str(), S_IRWXU) == -1)
            throw std::runtime_error("Error creating conf. folder");
}
