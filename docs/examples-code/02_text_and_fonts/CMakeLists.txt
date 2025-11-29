cmake_minimum_required(VERSION 3.14)
project(TextAndFontsExamples)

set(CMAKE_CXX_STANDARD 11)

# Fetch PDF-Writer from GitHub
include(FetchContent)
FetchContent_Declare(
  PDFHummus
  GIT_REPOSITORY https://github.com/galkahana/PDF-Writer.git
  GIT_TAG        v4.8.0
  FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(PDFHummus)

# Build examples
add_executable(simple_text simple_text.cpp)
target_link_libraries(simple_text PDFHummus::PDFWriter)

add_executable(multiline_text multiline_text.cpp)
target_link_libraries(multiline_text PDFHummus::PDFWriter)
