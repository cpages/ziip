# clean result
set (SDL_FOUND false)
set (SDL_INCLUDE_DIRS "")
set (SDL_LIBRARIES "")

# use pkg-config to get hints
include (LibFindMacros)
libfind_pkg_check_modules (SDL_PKGCONF sdl)

set (COMMON_SEARCH_PATHS
  / # cross-compile with CMAKE_FIND_ROOT_PATH set
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt)

set (INC_SEARCH_PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local/include/SDL12
  /usr/local/include/SDL11 # FreeBSD ports
  /usr/include/SDL12
  /usr/include/SDL11)

find_path (SDL_INCLUDE_DIR SDL.h
  HINTS $ENV{SDLDIR}
  PATH_SUFFIXES include/SDL include
  PATHS ${SDL_PKGCONF_INCLUDE_DIRS}
  ${INC_SEARCH_PATHS}
  ${COMMON_SEARCH_PATHS})
mark_as_advanced (SDL_INCLUDE_DIR)
#MESSAGE ("SDL_INCLUDE_DIR is ${SDL_INCLUDE_DIR}")

# SDL-1.1 is the name used by FreeBSD ports...
# don't confuse it for the version number.
find_library (SDL_LIB 
  NAMES SDL SDL-1.1
  HINTS $ENV{SDLDIR}
  PATH_SUFFIXES lib64 lib
    lib/wii # Wii
  PATHS ${SDL_PKGCONF_LIBRARY_DIRS}
  ${COMMON_SEARCH_PATHS})
mark_as_advanced (SDL_LIB)
#MESSAGE ("SDL_LIB is ${SDL_LIB}")

if (NOT SDL_BUILDING_LIBRARY)
  if (NOT ${SDL_INCLUDE_DIR} MATCHES ".framework")
    # Non-OS X framework versions expect you to also dynamically link to 
    # SDLmain. This is mainly for Windows and OS X. Other (Unix) platforms 
    # seem to provide SDLmain for compatibility even though they don't
    # necessarily need it.
    find_library (SDLMAIN_LIBRARY 
      NAMES SDLmain SDLmain-1.1
      HINTS $ENV{SDLDIR}
      PATH_SUFFIXES lib64 lib
      PATHS ${SDL_PKGCONF_LIBRARY_DIRS}
      ${COMMON_SEARCH_PATHS})
    mark_as_advanced (SDLMAIN_LIBRARY)
  endif ()
endif ()

# SDL may require threads on your system.
# The Apple build may not need an explicit flag because one of the 
# frameworks may already provide it. 
# But for non-OSX systems, I will use the CMake Threads package.
if (NOT APPLE)
  find_package (Threads)
endif ()

# MinGW needs an additional library, mwindows
# It's total link flags should look like -lmingw32 -lSDLmain -lSDL -lmwindows
# (Actually on second look, I think it only needs one of the m* libraries.)
if (MINGW)
  set (MINGW32_LIBRARY mingw32 CACHE STRING "mwindows for MinGW")
endif ()

macro (FIND_LIBS _LIBS)
  foreach (LIB ${${_LIBS}})
    #MESSAGE ("Searching for SDL_${LIB}")
    libfind_pkg_check_modules (SDL_${LIB}_PKGCONF SDL_${LIB})
    find_library (SDL_${LIB}
      NAMES SDL_${LIB}
      HINTS $ENV{SDLDIR}
      PATH_SUFFIXES lib64 lib
        lib/wii # Wii
      PATHS ${SDL_${LIB}_PKGCONF_LIBRARY_DIRS}
      ${COMMON_SEARCH_PATHS})
    #MESSAGE ("SDL_LIBRARY is ${SDL_${LIB}}")
    mark_as_advanced (SDL_${LIB})
  endforeach ()
endmacro ()

FIND_LIBS (SDL_FIND_COMPONENTS)

set (SDL_PROCESS_INCLUDES SDL_INCLUDE_DIR)
set (SDL_PROCESS_LIBS "")
foreach (LIB ${SDL_FIND_COMPONENTS})
  list (APPEND SDL_PROCESS_LIBS SDL_${LIB})
endforeach ()
list (APPEND SDL_PROCESS_LIBS SDL_LIB)

libfind_process(SDL)

if (SDL_FOUND)
  # For SDLmain
  if (NOT SDL_BUILDING_LIBRARY)
    if (SDLMAIN_LIBRARY)
      set (SDL_LIBRARIES ${SDLMAIN_LIBRARY} ${SDL_LIBRARIES})
    endif ()
  endif ()

  # For OS X, SDL uses Cocoa as a backend so it must link to Cocoa.
  # CMake doesn't display the -framework Cocoa string in the UI even 
  # though it actually is there if I modify a pre-used variable.
  # I think it has something to do with the CACHE STRING.
  # So I use a temporary variable until the end so I can set the 
  # "real" variable in one-shot.
  if (APPLE)
    set (SDL_LIBRARIES ${SDL_LIBRARIES} "-framework Cocoa")
  endif ()
    
  # For threads, as mentioned Apple doesn't need this.
  # In fact, there seems to be a problem if I used the Threads package
  # and try using this line, so I'm just skipping it entirely for OS X.
  if (NOT APPLE)
    set (SDL_LIBRARIES ${SDL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
  endif ()

  # For MinGW library
  if (MINGW)
    set (SDL_LIBRARIES ${MINGW32_LIBRARY} ${SDL_LIBRARIES})
  endif ()
endif ()
