add_library(LibPng
#sources
png.c
pngerror.c
pngget.c
pngmem.c
pngpread.c
pngread.c
pngrio.c
pngrtran.c
pngrutil.c
pngset.c
pngtrans.c
pngwio.c
pngwrite.c
pngwtran.c
pngwutil.c

#headers
png.h
pngconf.h
pngdebug.h
pnginfo.h
pnglibconf.h
pngpriv.h
pngstruct.h
)

add_library(PDFHummus::LibPng ALIAS LibPng)

set(LIBPNG_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(LIBPNG_LDFLAGS LibPng PARENT_SCOPE)
# https://github.com/julienr/libpng-android/issues/6
if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "aarch64" OR ${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "arm64")
    set(CMAKE_C_FLAGS -DPNG_ARM_NEON_OPT=0 ${CMAKE_C_FLAGS})
    message (STATUS "libPng Arm64 - disable fpu Neon optimization")
endif()

include_directories (${ZLIB_INCLUDE_DIRS})

target_link_libraries(LibPng ${ZLIB_LDFLAGS})

install(TARGETS LibPng
    EXPORT PDFHummusTargets
    RUNTIME DESTINATION bin COMPONENT dependencies
    ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT dependencies
    LIBRARY DESTINATION lib${LIB_SUFFIX} COMPONENT dependencies
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include
    FILES_MATCHING
    PATTERN "*.h"
)
