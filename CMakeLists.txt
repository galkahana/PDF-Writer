project(PDFHUMMUS)
cmake_minimum_required (VERSION 2.6)

set(USE_BUNDLED TRUE CACHE BOOL "Whether to use bundled libraries")

if(CMAKE_GENERATOR STREQUAL "Xcode")
    set(IS_XCODE TRUE)
endif(CMAKE_GENERATOR STREQUAL "Xcode")

if(NOT USE_BUNDLED)
    FIND_PACKAGE(PkgConfig)

    find_package (JPEG)
    if (NOT JPEG_FOUND)
        set (PDFHUMMUS_NO_DCT TRUE)
    endif (NOT JPEG_FOUND)

    PKG_CHECK_MODULES(ZLIB zlib REQUIRED)

    PKG_CHECK_MODULES(LIBTIFF libtiff-4)
    if(NOT LIBTIFF_FOUND)
        SET(PDFHUMMUS_NO_TIFF TRUE)
    else(NOT LIBTIFF_FOUND)
        ADD_DEFINITIONS(-D_INCLUDE_TIFF_HEADER)
    endif(NOT LIBTIFF_FOUND)

    PKG_CHECK_MODULES(FREETYPE REQUIRED freetype2)

    if (MINGW OR FLATPAK)
        # FIXME: Fedora's MinGW environment doesn't provide LibAesgm
        # FIXME: Flatpak's Gnome runtime doesn't provide LibAesgm
        ADD_SUBDIRECTORY (LibAesgm)
    else (MINGW OR FLATPAK)
        FIND_PATH(LIBAESGM_INCLUDE_DIRS aescpp.h /usr/local/include/aes /usr/include/aes)
        FIND_LIBRARY(LIBAESGM_LDFLAGS NAMES aesgm PATHS /usr/local/lib${LIB_SUFFIX} /usr/lib${LIB_SUFFIX})
    endif (MINGW OR FLATPAK)

    PKG_CHECK_MODULES(LIBPNG libpng)
    if(NOT LIBPNG_FOUND)
        SET(PDFHUMMUS_NO_PNG TRUE)
    endif(NOT LIBPNG_FOUND)

else(NOT USE_BUNDLED)
    ADD_SUBDIRECTORY(LibAesgm)
    if(NOT PDFHUMMUS_NO_DCT)
    ADD_SUBDIRECTORY(LibJpeg)
    endif(NOT PDFHUMMUS_NO_DCT)
    ADD_SUBDIRECTORY(ZLib)
    if(NOT PDFHUMMUS_NO_TIFF)
    ADD_SUBDIRECTORY(LibTiff)
    endif(NOT PDFHUMMUS_NO_TIFF)
    ADD_SUBDIRECTORY(FreeType)
    if(NOT PDFHUMMUS_NO_PNG)
    ADD_SUBDIRECTORY(LibPng)
    endif(NOT PDFHUMMUS_NO_PNG)
endif(NOT USE_BUNDLED)
ADD_SUBDIRECTORY(PDFWriter)
ADD_SUBDIRECTORY(PDFWriterTestPlayground)
