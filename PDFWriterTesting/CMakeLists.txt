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

target_link_libraries (PDFWriterTesting PDFWriter)

if(APPLE)
	set(CMAKE_EXE_LINKER_FLAGS "-framework CoreFoundation")
endif(APPLE)

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