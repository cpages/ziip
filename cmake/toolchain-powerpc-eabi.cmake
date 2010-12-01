set (CMAKE_SYSTEM_NAME Generic)
set (CMAKE_SYSTEM_VERSION 1)

set (DEVKITPPC $ENV{DEVKITPPC})
set (DEVKITPRO $ENV{DEVKITPRO})

set (CMAKE_CXX_COMPILER ${DEVKITPPC}/bin/powerpc-eabi-g++)
set (CMAKE_C_COMPILER ${DEVKITPPC}/bin/powerpc-eabi-gcc)

# where is the target environment 
set (CMAKE_FIND_ROOT_PATH  ${DEVKITPPC}
    ${DEVKITPRO}/libogc
    ${DEVKITPRO}/portlibs/ppc)

set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# additional stuff Wii specific
set (WII_MACHDEP "-DGEKKO -mrvl -mcpu=750 -meabi -mhard-float")

set (WII_DEPS /opt/devkitpro/portlibs/ppc/lib/libfreetype.a
    /opt/devkitpro/portlibs/ppc/lib/libjpeg.a
    /opt/devkitpro/portlibs/ppc/lib/libpng.a
    /opt/devkitpro/portlibs/ppc/lib/libz.a
    /opt/devkitpro/libogc/lib/wii/libfat.a
    /opt/devkitpro/libogc/lib/wii/libwiiuse.a
    /opt/devkitpro/libogc/lib/wii/libwiikeyboard.a
    /opt/devkitpro/libogc/lib/wii/libbte.a
    /opt/devkitpro/libogc/lib/wii/libogc.a
    /opt/devkitpro/devkitPPC/powerpc-eabi/lib/libm.a)
