add_library(Zlib
#sources
adler32.c
compress.c
crc32.c
deflate.c
gzclose.c
gzlib.c
gzread.c
gzwrite.c
infback.c
inffast.c
inflate.c
inftrees.c
trees.c
uncompr.c
zutil.c

#headers
zconf.h
zlib.h
crc32.h
deflate.h
gzguts.h
inffast.h
inffixed.h
inflate.h
inftrees.h
trees.h
zutil.h
)

add_library(PDFHummus::Zlib ALIAS Zlib)

target_include_directories(Zlib
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/Zlib> 
)

#
# Check for unistd.h
#
include(CheckIncludeFile)
check_include_file(unistd.h Z_HAVE_UNISTD_H)
if(Z_HAVE_UNISTD_H)
    target_compile_definitions(Zlib PUBLIC HAVE_UNISTD_H=1)
endif(Z_HAVE_UNISTD_H)

# drop obsolete symbol, causing trouble when compiling with clang on mac, and in general unneeded
target_compile_options(Zlib PRIVATE -UTARGET_OS_MAC)

if(WIN32 AND BUILD_SHARED_LIBS)
    target_compile_definitions(Zlib PUBLIC ZLIB_DLL)
endif()

install(TARGETS Zlib
    EXPORT PDFHummusTargets
    RUNTIME DESTINATION bin COMPONENT dependencies
    ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT dependencies
    LIBRARY DESTINATION lib${LIB_SUFFIX} COMPONENT dependencies
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include
    COMPONENT dependencies
    FILES_MATCHING
    PATTERN "*.h"
)
