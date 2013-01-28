project(PDFHUMMUS)
cmake_minimum_required (VERSION 2.6)

add_executable( test executable.cpp)
ADD_SUBDIRECTORY(LibJpeg)
ADD_SUBDIRECTORY(Zlib)
ADD_SUBDIRECTORY(LibTiff)
ADD_SUBDIRECTORY(PDFWriter)
find_library(FreeType_LIBRARY NAMES FreeType )

message("location of a frretype is ${FreeType_LIBRARY}")
add_library( FreeType UNKNOWN IMPORTED )
set_property(TARGET FreeType PROPERTY IMPORTED_LOCATION ${FreeType_LIBRARY})

include_directories (${PDFWriter_SOURCE_DIR}) 

add_dependencies(test PDFWriter) #add_dependencies makes sure that dependencies are built before main target

target_link_libraries (test PDFWriter)
target_link_libraries (test  FreeType)
target_link_libraries (test LibJpeg )
target_link_libraries (test Zlib )
target_link_libraries (test LibTiff )