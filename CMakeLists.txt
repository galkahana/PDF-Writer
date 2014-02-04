# Set the name of the project.
project(hummus)

# Set the minimum version of cmake required to build this project.
cmake_minimum_required(VERSION 2.6)

# Load the pkg-config package.
find_package(PkgConfig REQUIRED)

# Use pkg-config to detect and configure libtiff.
if(NOT PDFHUMMUS_NO_TIFF)
    pkg_check_modules(TIFF REQUIRED libtiff-4)
    include_directories(${TIFF_INCLUDE_DIRS})
endif(NOT PDFHUMMUS_NO_TIFF)

# Use pkg-config to detect and configure freetype.
pkg_check_modules(FREETYPE REQUIRED freetype2)
include_directories(${FREETYPE_INCLUDE_DIRS})

# Use pkg-config to detect and configure zlib.
pkg_check_modules(Z REQUIRED zlib)
include_directories(${Z_INCLUDE_DIRS})

# Add targets.
add_subdirectory(PDFWriter)
add_subdirectory(PDFWriterTestPlayground)
