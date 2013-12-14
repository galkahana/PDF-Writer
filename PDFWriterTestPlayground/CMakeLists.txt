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
HighLevelContentContext.cpp
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
PageModifierTest.cpp
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
TestMeasurementsTest.cpp
TestsRunner.cpp
HighLevelImages.cpp
TIFFImageTest.cpp
TiffSpecialsTest.cpp
TimerTest.cpp
TrueTypeTest.cpp
TTCTest.cpp
Type1Test.cpp
UppercaseSequanceTest.cpp
WindowsPath.cpp
PDFWriterTestPlayground.cpp
CopyingAndMergingEmptyPages.cpp

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
HighLevelContentContext.h
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
PageModifierTest.h
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
TestMeasurementsTest.h
TestsRunner.h
HighLevelImages.h
TIFFImageTest.h
TiffSpecialsTest.h
TimerTest.h
TrueTypeTest.h
TTCTest.h
Type1Test.h
UppercaseSequanceTest.h
CopyingAndMergingEmptyPages.h
)

source_group(Main FILES
PDFWriterTestPlayground.cpp
)

source_group("TestingSystem\\Hummus Paths" FILES
FileURL.cpp
FileURL.h
PosixPath.cpp
PosixPath.h
WindowsPath.cpp
WindowsPath.h
)

source_group(TestingSystem FILES
ITestUnit.h
TestsRunner.cpp
TestsRunner.h
)

source_group(Tests\\Basics FILES
TimerTest.cpp
TimerTest.h
)

source_group(Tests\\CFF FILES
OpenTypeTest.cpp
OpenTypeTest.h
)

source_group("Tests\\DCT Decode" FILES
DCTDecodeFilterTest.cpp
DCTDecodeFilterTest.h
JpegLibTest.cpp
JpegLibTest.h
)

source_group("Tests\\Font Packages" FILES
DFontTest.cpp
DFontTest.h
TTCTest.cpp
TTCTest.h
)

source_group("Tests\\Free Type" FILES
FreeTypeInitializationTest.cpp
FreeTypeInitializationTest.h
)

source_group(Tests\\IO FILES
BufferedOutputStreamTest.cpp
BufferedOutputStreamTest.h
FlateEncryptionTest.cpp
FlateEncryptionTest.h
LogTest.cpp
LogTest.h
OutputFileStreamTest.cpp
OutputFileStreamTest.h
)

source_group("Tests\\Modification\\Comments Infrastructure" FILES
PDFComment.cpp
PDFComment.h
PDFCommentWriter.cpp
PDFCommentWriter.h
)

source_group("Tests\\Modification" FILES
AppendingAndReading.cpp
AppendingAndReading.h
BasicModification.cpp
BasicModification.h
ModifyingExistingFileContent.cpp
ModifyingExistingFileContent.h
PageModifierTest.cpp
PageModifierTest.h
)

source_group("Tests\\Object Context Level" FILES
PDFDateTest.cpp
PDFDateTest.h
PDFTextStringTest.cpp
PDFTextStringTest.h
UppercaseSequanceTest.cpp
UppercaseSequanceTest.h
)

source_group(Tests\\Patterns FILES
BoxingBaseTest.cpp
BoxingBaseTest.h
)

source_group(Tests\\PDFEmbedding FILES
AppendPagesTest.cpp
AppendPagesTest.h
AppendSpecialPagesTest.cpp
AppendSpecialPagesTest.h
InputFlateDecodeTester.cpp
InputFlateDecodeTester.h
MergePDFPages.cpp
MergePDFPages.h
MergeToPDFForm.cpp
MergeToPDFForm.h
PDFCopyingContextTest.cpp
PDFCopyingContextTest.h
PDFEmbedTest.cpp
PDFEmbedTest.h
PDFObjectCastTest.cpp
PDFObjectCastTest.h
PDFParserTest.cpp
PDFParserTest.h
RefCountTest.cpp
RefCountTest.h
CopyingAndMergingEmptyPages.cpp
CopyingAndMergingEmptyPages.h
)

source_group(Tests\\PDFs\\CustomStreamsIO FILES
CustomLogTest.cpp
CustomLogTest.h
InputImagesAsStreamsTest.cpp
InputImagesAsStreamsTest.h
)

source_group(Tests\\PDFs\\Generic FILES
EmptyFileTest.cpp
EmptyFileTest.h
EmptyPagesPDF.cpp
EmptyPagesPDF.h
HighLevelContentContext.cpp
HighLevelContentContext.h
FormXObjectTest.cpp
FormXObjectTest.h
LinksTest.cpp
LinksTest.h
ShutDownRestartTest.cpp
ShutDownRestartTest.h
SimpleContentPageTest.cpp
SimpleContentPageTest.h
)

source_group("Tests\\PDFs\\Images in PDF" FILES
ImagesAndFormsForwardReferenceTest.cpp
ImagesAndFormsForwardReferenceTest.h
JPGImageTest.cpp
JPGImageTest.h
HighLevelImages.cpp
HighLevelImages.h
TIFFImageTest.cpp
TIFFImageTest.h
TiffSpecialsTest.cpp
TiffSpecialsTest.h
)

source_group(Tests\\Text FILES
SimpleTextUsage.cpp
SimpleTextUsage.h
TestMeasurementsTest.cpp
TestMeasurementsTest.h
)

source_group(Tests\\TrueType FILES
TrueTypeTest.cpp
TrueTypeTest.h
)

source_group(Tests\\Type1 FILES
PFBStreamTest.cpp
PFBStreamTest.h
Type1Test.cpp
Type1Test.h
)

if(PDFHUMMUS_NO_DCT)
	add_definitions(-DPDFHUMMUS_NO_DCT=1)
endif(PDFHUMMUS_NO_DCT)
if(PDFHUMMUS_NO_TIFF)
	add_definitions(-DPDFHUMMUS_NO_TIFF=1)
endif(PDFHUMMUS_NO_TIFF)


include_directories (${PDFWriter_SOURCE_DIR}) 
include_directories (${Zlib_SOURCE_DIR})
if(NOT PDFHUMMUS_NO_DCT)
	include_directories (${LibJpeg_SOURCE_DIR})
else(NOT PDFHUMMUS_NO_DCT)
	add_definitions(-DPDFHUMMUS_NO_DCT=1)
endif(NOT PDFHUMMUS_NO_DCT)

if(NOT PDFHUMMUS_NO_TIFF)
	include_directories (${LibTiff_SOURCE_DIR})  
else(NOT PDFHUMMUS_NO_TIFF)
	add_definitions(-DPDFHUMMUS_NO_TIFF=1)
endif(NOT PDFHUMMUS_NO_TIFF)
include_directories (${FreeType_SOURCE_DIR}/include) 

add_dependencies(PDFWriterTestPlayground PDFWriter) #add_dependencies makes sure that dependencies are built before main target

target_link_libraries (PDFWriterTestPlayground PDFWriter)
target_link_libraries (PDFWriterTestPlayground FreeType)
if(NOT PDFHUMMUS_NO_DCT)
	target_link_libraries (PDFWriterTestPlayground LibJpeg)
endif(NOT PDFHUMMUS_NO_DCT)
target_link_libraries (PDFWriterTestPlayground Zlib)
if(NOT PDFHUMMUS_NO_TIFF)
	target_link_libraries (PDFWriterTestPlayground LibTiff)
endif(NOT PDFHUMMUS_NO_TIFF)

if(APPLE)
	set(CMAKE_EXE_LINKER_FLAGS "-framework CoreFoundation")
endif(APPLE)
