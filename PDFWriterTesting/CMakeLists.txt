## testing
enable_testing()

# Test files (add new tests here)
create_test_sourcelist (Tests
  PDFWriterTesting.cpp
  AppendingAndReading.cpp
  AppendPagesTest.cpp
  AppendSpecialPagesTest.cpp
  BasicModification.cpp
  BoxingBaseTest.cpp
  BufferedOutputStreamTest.cpp
  CopyingAndMergingEmptyPages.cpp
  CustomLogTest.cpp
  DCTDecodeFilterTest.cpp
  DFontTest.cpp
  EmptyFileTest.cpp
  EmptyPagesPDF.cpp
  EncryptedPDF.cpp
  FlateEncryptionTest.cpp
  FlateObjectDecodeTest.cpp
  FormXObjectTest.cpp
  FreeTypeInitializationTest.cpp
  HighLevelContentContext.cpp
  HighLevelImages.cpp
  ImagesAndFormsForwardReferenceTest.cpp
  InputFlateDecodeTester.cpp
  InputImagesAsStreamsTest.cpp
  JpegLibTest.cpp
  JPGImageTest.cpp
  LinksTest.cpp
  LogTest.cpp
  MergePDFPages.cpp
  MergeToPDFForm.cpp
  ModifyingEncryptedFile.cpp
  ModifyingExistingFileContent.cpp
  OpenTypeTest.cpp
  OutputFileStreamTest.cpp
  PageModifierTest.cpp
  PageOrderModification.cpp
  ParsingBadXref.cpp
  ParsingFaulty.cpp
  PDFCopyingContextTest.cpp
  PDFDateTest.cpp
  PDFEmbedTest.cpp
  PDFObjectCastTest.cpp
  PDFObjectParserTest.cpp
  PDFParserTest.cpp
  PDFWithPassword.cpp
  PFBStreamTest.cpp
  PNGImageTest.cpp
  RecryptPDF.cpp
  RefCountTest.cpp
  RotatedPagesPDF.cpp
  ShutDownRestartTest.cpp
  SimpleContentPageTest.cpp
  SimpleTextUsage.cpp
  TestMeasurementsTest.cpp
  TextUsageBugs.cpp
  TIFFImageTest.cpp
  TiffSpecialsTest.cpp
  TimerTest.cpp
  TrueTypeTest.cpp
  TTCTest.cpp
  Type1Test.cpp
  UnicodeTextUsage.cpp
  UppercaseSequanceTest.cpp
)

# add the testing executable
add_executable (PDFWriterTesting     
    ${Tests}

    # and some utilities
    testing/FileURL.cpp
    testing/PosixPath.cpp
    testing/WindowsPath.cpp

    # and test specific additionals
    PDFComment.cpp
    PDFCommentWriter.cpp
)

# (adding to all target only if project is built independently, so that FetchContent imports that build from source 
# wont build this unnecessary step unless asked for explicitly)

set_target_properties(PDFWriterTesting
  PROPERTIES EXCLUDE_FROM_ALL $<NOT:$<BOOL:${PROJECT_IS_TOP_LEVEL}>>
)

### now all the stuff required to compile and link with the rest of pdfwriter
if(PDFHUMMUS_NO_DCT)
	add_definitions(-DPDFHUMMUS_NO_DCT=1)
endif(PDFHUMMUS_NO_DCT)

if(PDFHUMMUS_NO_TIFF)
	add_definitions(-DPDFHUMMUS_NO_TIFF=1)
endif(PDFHUMMUS_NO_TIFF)

if(PDFHUMMUS_NO_PNG)
	add_definitions(-DPDFHUMMUS_NO_PNG=1)
endif(PDFHUMMUS_NO_PNG)

include_directories (${PDFWriter_INCLUDE_DIRS})
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

target_link_libraries (PDFWriterTesting PDFWriter)
target_link_libraries (PDFWriterTesting ${LIBAESGM_LDFLAGS})
target_link_libraries (PDFWriterTesting ${FREETYPE_LDFLAGS})
if(NOT PDFHUMMUS_NO_DCT)
	target_link_libraries (PDFWriterTesting ${LIBJPEG_LDFLAGS})
endif(NOT PDFHUMMUS_NO_DCT)
target_link_libraries (PDFWriterTesting ${ZLIB_LDFLAGS})
if(NOT PDFHUMMUS_NO_TIFF)
	target_link_libraries (PDFWriterTesting ${LIBTIFF_LDFLAGS})
endif(NOT PDFHUMMUS_NO_TIFF)
if(NOT PDFHUMMUS_NO_PNG)
	target_link_libraries (PDFWriterTesting ${LIBPNG_LDFLAGS})
endif(NOT PDFHUMMUS_NO_PNG)

if(APPLE)
	set(CMAKE_EXE_LINKER_FLAGS "-framework CoreFoundation")
endif(APPLE)

### done with the stuff for PDFWriter



# Add all the ADD_TEST for each test (reusing the create_test_sourcelist list minus the generated executable)
set (TestsToRun ${Tests})
remove (TestsToRun PDFWriterTesting.cpp)
foreach (test ${TestsToRun})
  get_filename_component (TName ${test} NAME_WE)
  add_test (NAME ${TName} COMMAND PDFWriterTesting ${TName} ${CMAKE_CURRENT_SOURCE_DIR}/Materials ${CMAKE_BINARY_DIR}/Testing/Output)
endforeach ()

# create temp dir for output files
set (TmpOutputDir ${CMAKE_BINARY_DIR}/Testing/Output)

file(MAKE_DIRECTORY  ${TmpOutputDir})

# pdfWriterCheck: target for running the tests via ctest. using depends to make sure project+deps are build, and also that the tmp output folder is clear of
# materials from previous runs by recreating it. [note that the two CMAKE_COMMAND steps are ran using COMMAND. this is done to make sure they run 
# sequentially]
add_custom_target(pdfWriterCheck
  COMMAND ${CMAKE_COMMAND} -E rm -r -f ${TmpOutputDir}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${TmpOutputDir}
  COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIG>
  DEPENDS PDFWriterTesting
)