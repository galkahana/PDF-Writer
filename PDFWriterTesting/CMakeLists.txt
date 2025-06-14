## testing
enable_testing()

# Test files (add new tests here)
create_test_sourcelist (Tests
  PDFWriterTesting.cpp
  AppendAndReplaceURLAnnotations.cpp
  AppendingAndReading.cpp
  AppendPagesTest.cpp
  AppendSpecialPagesTest.cpp
  AppendWithAnnotations.cpp
  BasicModification.cpp
  BoxingBaseTest.cpp
  BufferedOutputStreamTest.cpp
  CIDSetWritingCFF.cpp
  CIDSetWritingTrueType.cpp
  CIDSetWritingTrueType2.cpp
  ColorEmojiColr.cpp
  ColorEmojiColrV1.cpp
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
  ParsePDFWithOwner.cpp
  ParsingBadXref.cpp
  ParsingFaulty.cpp
  PDFCopyingContextTest.cpp
  PDFDateTest.cpp
  PDFEmbedTest.cpp
  PDFObjectCastTest.cpp
  PDFObjectParserTest.cpp
  PDFParserFuzzSanity.cpp
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
  TrueTypeAnsiWriteBug.cpp
  TrueTypeTest.cpp
  TTCTest.cpp
  Type1Test.cpp
  UnicodeTextUsage.cpp
  UppercaseSequanceTest.cpp
  WatermarkTest.cpp
  WatermarkWithContextOpacityTest.cpp
  XrefStreamsTest.cpp
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
    AnnotationsWriter.cpp
  
)

target_link_libraries (PDFWriterTesting PDFWriter)

if(APPLE)
	set(CMAKE_EXE_LINKER_FLAGS "-framework CoreFoundation")
endif(APPLE)

# Add all the ADD_TEST for each test (reusing the create_test_sourcelist list minus the generated executable)
set (TestsToRun ${Tests})
list(REMOVE_AT TestsToRun 0) # removing first item which is PDFWriterTesting. started getting a full path for it, so moved to REMOVE_AT instead of REMOVE_ITEM with the file name
list(REMOVE_ITEM TestsToRun PDFParserFuzzSanity.cpp) # will add tests for this specifically

foreach (test ${TestsToRun})
  get_filename_component (TName ${test} NAME_WE)
  add_test (NAME ${TName} COMMAND PDFWriterTesting ${TName} ${CMAKE_CURRENT_SOURCE_DIR}/Materials ${CMAKE_BINARY_DIR}/Testing/Output)
endforeach ()

# fuzz test errors specific
file(GLOB fuzztestfiles ${CMAKE_CURRENT_SOURCE_DIR}/Materials/fuzzing/errors/*)
foreach (fuzztestfile ${fuzztestfiles})
    get_filename_component (TName ${fuzztestfile} NAME)
    add_test (NAME FuzzTest_${TName} COMMAND PDFWriterTesting PDFParserFuzzSanity ${CMAKE_CURRENT_SOURCE_DIR}/Materials ${CMAKE_BINARY_DIR}/Testing/Output ${fuzztestfile} FuzzTest_${TName}.txt)
endforeach ()

# fuzz testing with fuzzing harness
if(BUILD_FUZZING_HARNESS)
    add_executable(PDFParserFuzzingHarness PDFParserFuzzingHarness.cpp)
    target_compile_options(PDFParserFuzzingHarness PRIVATE -fsanitize=fuzzer,address -g -O2)
    target_link_libraries(PDFParserFuzzingHarness PDFWriter -O2 -fsanitize=fuzzer,address -fuse-ld=lld)
endif()


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