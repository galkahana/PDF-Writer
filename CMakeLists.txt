cmake_minimum_required(VERSION 3.15)

# Soname
# MAJOR is incremented when symbols are removed or changed in an incompatible way
# MINOR is incremented when new symbols are added
project(PDFHummus VERSION 1.0)


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

enable_testing()
ADD_SUBDIRECTORY(PDFWriterTesting)


include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${PDFHummus_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PDFHummus_VERSION_MINOR}")
set(CPACK_SOURCE_GENERATOR "ZIP")
set(CPACK_GENERATOR "ZIP")
include(CPack)

install(EXPORT PDFHummusTargets
  FILE PDFHummusTargets.cmake
  DESTINATION lib/cmake/PDFHummus
  NAMESPACE PDFHummus::
)

include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/PDFHummusConfig.cmake"
  INSTALL_DESTINATION "lib/cmake/PDFHummus"
  )

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/PDFHummusConfigVersion.cmake"
    VERSION "${PDFHummus_VERSION_MAJOR}.${PDFHummus_VERSION_MINOR}"
    COMPATIBILITY AnyNewerVersion
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/PDFHummusConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/PDFHummusConfigVersion.cmake
  DESTINATION lib/cmake/PDFHummus
  )

export(EXPORT PDFHummusTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/PDFHummusTargets.cmake"
  NAMESPACE PDFHummus::
)