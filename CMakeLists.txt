include(ExternalProject)

project(PDFHUMMUS)
cmake_minimum_required (VERSION 2.6)

if(NOT PDFHUMMUS_NO_DCT)
	ADD_SUBDIRECTORY(LibJpeg)
endif(NOT PDFHUMMUS_NO_DCT)
	ADD_SUBDIRECTORY(ZLib)
if(NOT PDFHUMMUS_NO_TIFF)
	ADD_SUBDIRECTORY(LibTiff)
endif(NOT PDFHUMMUS_NO_TIFF)
if(PDFHUMMUS_BIDI)
  ExternalProject_Add(
  libFribidi
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libFribidi
  CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/libFribidi/bootstrap && ${CMAKE_CURRENT_SOURCE_DIR}/libFribidi/configure --prefix=<INSTALL_DIR>/build  --enable-static=yes --enable-shared=no
  PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/libFribidi
  BUILD_COMMAND make
  BUILD_IN_SOURCE 0
  
)

endif(PDFHUMMUS_BIDI)

ADD_SUBDIRECTORY(FreeType)
ADD_SUBDIRECTORY(PDFWriter)
ADD_SUBDIRECTORY(PDFWriterTestPlayground)
