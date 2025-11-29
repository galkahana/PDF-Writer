cmake_minimum_required(VERSION 3.14)
project(PDFtoPDFExamples)

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

# Build the example
add_executable(pdf_operations pdf_operations.cpp)
target_link_libraries(pdf_operations PDFHummus::PDFWriter)
