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
RotatedPagesPDF.cpp
FileURL.cpp
FlateEncryptionTest.cpp
FlateObjectDecodeTest.cpp
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
PDFWithPassword.cpp
MergePDFPages.cpp
MergeToPDFForm.cpp
ModifyingEncryptedFile.cpp
ModifyingExistingFileContent.cpp
PageModifierTest.cpp
PageOrderModification.cpp
OpenTypeTest.cpp
OutputFileStreamTest.cpp
ParsingFaulty.cpp
PDFComment.cpp
PDFCommentWriter.cpp
PDFCopyingContextTest.cpp
PDFDateTest.cpp
PDFEmbedTest.cpp
PDFObjectCastTest.cpp
PDFObjectParserTest.cpp
PDFParserTest.cpp
PDFTextStringTest.cpp
PFBStreamTest.cpp
PNGImageTest.cpp
PosixPath.cpp
RecryptPDF.cpp
RefCountTest.cpp
ShutDownRestartTest.cpp
SimpleContentPageTest.cpp
SimpleTextUsage.cpp
TestMeasurementsTest.cpp
TestsRunner.cpp
TextUsageBugs.cpp
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
EncryptedPDF.cpp
UnicodeTextUsage.cpp

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
RotatedPagesPDF.h
FileURL.h
FlateEncryptionTest.h
FlateObjectDecodeTest.h
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
PDFWithPassword.h
MergePDFPages.h
MergeToPDFForm.h
ModifyingEncryptedFile.h
ModifyingExistingFileContent.h
PageModifierTest.h
PageOrderModification.h
OpenTypeTest.h
OutputFileStreamTest.h
ParsingFaulty.h
PDFComment.h
PDFCommentWriter.h
PDFCopyingContextTest.h
PDFDateTest.h
PDFEmbedTest.h
PDFObjectCastTest.h
PDFObjectParserTest.h
PDFParserTest.h
PDFTextStringTest.h
PFBStreamTest.h
PNGImageTest.h
PosixPath.h
RecryptPDF.h
RefCountTest.h
ShutDownRestartTest.h
SimpleContentPageTest.h
SimpleTextUsage.h
TestMeasurementsTest.h
TestsRunner.h
TextUsageBugs.h
HighLevelImages.h
TIFFImageTest.h
TiffSpecialsTest.h
TimerTest.h
TrueTypeTest.h
TTCTest.h
Type1Test.h
UppercaseSequanceTest.h
CopyingAndMergingEmptyPages.h
EncryptedPDF.h
UnicodeTextUsage.h
)

source_group(Main FILES
PDFWriterTestPlayground.cpp
)

source_group("Tests\\Parse" FILES
PDFObjectParserTest.cpp
PDFObjectParserTest.h
FlateObjectDecodeTest.cpp
FlateObjectDecodeTest.h
ParsingFaulty.cpp
ParsingFaulty.h
ParsingBadXref.cpp
ParsingBadXref.h
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
ModifyingEncryptedFile.cpp
ModifyingEncryptedFile.h
ModifyingExistingFileContent.cpp
ModifyingExistingFileContent.h
PageModifierTest.cpp
PageModifierTest.h
PageOrderModification.cpp
PageOrderModification.h
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
EncryptedPDF.cpp
EncryptedPDF.h
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
PDFWithPassword.cpp
PDFWithPassword.h
RecryptPDF.cpp
RecryptPDF.h
RotatedPagesPDF.cpp
RotatedPagesPDF.h
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
PNGImageTest.cpp
PNGImageTest.h
)

source_group(Tests\\Text FILES
SimpleTextUsage.cpp
SimpleTextUsage.h
TestMeasurementsTest.cpp
TestMeasurementsTest.h
TextUsageBugs.cpp
TextUsageBugs.h
UnicodeTextUsage.cpp
UnicodeTextUsage.h
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

if(PDFHUMMUS_NO_PNG)
	add_definitions(-DPDFHUMMUS_NO_PNG=1)
endif(PDFHUMMUS_NO_PNG)

include_directories (${PDFWriter_SOURCE_DIR})
include_directories (${LIBAESGM_INCLUDE_DIRS})
include_directories (${ZLIB_INCLUDE_DIRS})
if(NOT PDFHUMMUS_NO_DCT)
	include_directories (${LIBJPEG_INCLUDE_DIRS})
else(NOT PDFHUMMUS_NO_DCT)
	add_definitions(-DPDFHUMMUS_NO_DCT=1)
endif(NOT PDFHUMMUS_NO_DCT)

if(NOT PDFHUMMUS_NO_TIFF)
	include_directories (${LIBTIFF_INCLUDE_DIRS})
else(NOT PDFHUMMUS_NO_TIFF)
	add_definitions(-DPDFHUMMUS_NO_TIFF=1)
endif(NOT PDFHUMMUS_NO_TIFF)
include_directories (${FREETYPE_INCLUDE_DIRS})

if(NOT PDFHUMMUS_NO_PNG)
	include_directories (${LIBPNG_INCLUDE_DIRS})
else(NOT PDFHUMMUS_NO_PNG)
	add_definitions(-DPDFHUMMUS_NO_PNG=1)
endif(NOT PDFHUMMUS_NO_PNG)

add_dependencies(PDFWriterTestPlayground PDFWriter) #add_dependencies makes sure that dependencies are built before main target

target_link_libraries (PDFWriterTestPlayground PDFWriter)
target_link_libraries (PDFWriterTestPlayground ${LIBAESGM_LDFLAGS})
target_link_libraries (PDFWriterTestPlayground ${FREETYPE_LDFLAGS})
if(NOT PDFHUMMUS_NO_DCT)
	target_link_libraries (PDFWriterTestPlayground ${LIBJPEG_LDFLAGS})
endif(NOT PDFHUMMUS_NO_DCT)
target_link_libraries (PDFWriterTestPlayground ${ZLIB_LDFLAGS})
if(NOT PDFHUMMUS_NO_TIFF)
	target_link_libraries (PDFWriterTestPlayground ${LIBTIFF_LDFLAGS})
endif(NOT PDFHUMMUS_NO_TIFF)
if(NOT PDFHUMMUS_NO_PNG)
	target_link_libraries (PDFWriterTestPlayground ${LIBPNG_LDFLAGS})
endif(NOT PDFHUMMUS_NO_PNG)

if(APPLE)
	set(CMAKE_EXE_LINKER_FLAGS "-framework CoreFoundation")
endif(APPLE)


add_custom_target(check
    ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/TestMaterials ${CMAKE_BINARY_DIR}/Testing/TestMaterials
    COMMAND $<TARGET_FILE:PDFWriterTestPlayground> -b ${CMAKE_BINARY_DIR}/Testing
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS PDFWriterTestPlayground
)
