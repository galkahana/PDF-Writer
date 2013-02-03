project(PDFWriterTestPlayground)
cmake_minimum_required (VERSION 2.6)


add_executable(PDFWriterTestPlayground 

#sources
AppendingAndReading.cpp
AppendPagesTest.cpp
AppendSpecialPagesTest.cpp
BasicModification.cpp
BoxingBaseTest.cpp
BufferedOutputStreamTest.cpp
CustomLogTest.cpp
DCTDecodeFilterTest.cpp
DFontTest.cpp
EmptyFileTest.cpp
EmptyPagesPDF.cpp
FileURL.cpp
FlateEncryptionTest.cpp
FormXObjectTest.cpp
FreeTypeInitializationTest.cpp
ImagesAndFormsForwardReferenceTest.cpp
InputFlateDecodeTester.cpp
InputImagesAsStreamsTest.cpp
JpegLibTest.cpp
JPGImageTest.cpp
LinksTest.cpp
LogTest.cpp
MergePDFPages.cpp
MergeToPDFForm.cpp
ModifyingExistingFileContent.cpp
OpenTypeTest.cpp
OutputFileStreamTest.cpp
PDFComment.cpp
PDFCommentWriter.cpp
PDFCopyingContextTest.cpp
PDFDateTest.cpp
PDFEmbedTest.cpp
PDFObjectCastTest.cpp
PDFParserTest.cpp
PDFTextStringTest.cpp
PFBStreamTest.cpp
PosixPath.cpp
RefCountTest.cpp
ShutDownRestartTest.cpp
SimpleContentPageTest.cpp
SimpleTextUsage.cpp
TestsRunner.cpp
TIFFImageTest.cpp
TiffSpecialsTest.cpp
TimerTest.cpp
TrueTypeTest.cpp
TTCTest.cpp
Type1Test.cpp
UppercaseSequanceTest.cpp
WindowsPath.cpp
PDFWriterTestPlayground.cpp

#headers
AppendingAndReading.h
AppendPagesTest.h
AppendSpecialPagesTest.h
BasicModification.h
BoxingBaseTest.h
BufferedOutputStreamTest.h
CustomLogTest.h
DCTDecodeFilterTest.h
DFontTest.h
EmptyFileTest.h
EmptyPagesPDF.h
FileURL.h
FlateEncryptionTest.h
FormXObjectTest.h
FreeTypeInitializationTest.h
ImagesAndFormsForwardReferenceTest.h
InputFlateDecodeTester.h
InputImagesAsStreamsTest.h
ITestUnit.h
JpegLibTest.h
JPGImageTest.h
LinksTest.h
LogTest.h
MergePDFPages.h
MergeToPDFForm.h
ModifyingExistingFileContent.h
OpenTypeTest.h
OutputFileStreamTest.h
PDFComment.h
PDFCommentWriter.h
PDFCopyingContextTest.h
PDFDateTest.h
PDFEmbedTest.h
PDFObjectCastTest.h
PDFParserTest.h
PDFTextStringTest.h
PFBStreamTest.h
PosixPath.h
RefCountTest.h
ShutDownRestartTest.h
SimpleContentPageTest.h
SimpleTextUsage.h
TestsRunner.h
TIFFImageTest.h
TiffSpecialsTest.h
TimerTest.h
TrueTypeTest.h
TTCTest.h
Type1Test.h
UppercaseSequanceTest.h
)

if(NO_DCT)
	add_definitions(-DNO_DCT=1)
endif(NO_DCT)
if(NO_TIFF)
	add_definitions(-DNO_TIFF=1)
endif(NO_TIFF)


include_directories (${PDFWriter_SOURCE_DIR}) 
include_directories (${Zlib_SOURCE_DIR})
if(NOT NO_DCT)
	include_directories (${LibJpeg_SOURCE_DIR})
else(NOT NO_DCT)
	add_definitions(-DNO_DCT=1)
endif(NOT NO_DCT)

if(NOT NO_TIFF)
	include_directories (${LibTiff_SOURCE_DIR})  
else(NOT NO_TIFF)
	add_definitions(-DNO_TIFF=1)
endif(NOT NO_TIFF)
include_directories (${FreeType_SOURCE_DIR}/include) 

add_dependencies(PDFWriterTestPlayground PDFWriter) #add_dependencies makes sure that dependencies are built before main target

target_link_libraries (PDFWriterTestPlayground PDFWriter)
target_link_libraries (PDFWriterTestPlayground FreeType)
if(NOT NO_DCT)
	target_link_libraries (PDFWriterTestPlayground LibJpeg)
endif(NOT NO_DCT)
target_link_libraries (PDFWriterTestPlayground Zlib)
if(NOT NO_TIFF)
	target_link_libraries (PDFWriterTestPlayground LibTiff)
endif(NOT NO_TIFF)

if(APPLE)
	set(CMAKE_EXE_LINKER_FLAGS "-framework CoreFoundation")
endif(APPLE)
