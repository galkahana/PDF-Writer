Welcome to PDF-Writer, Also known as [PDFHummus](http://www.pdfhummus.com).   
PDFHummus is a Fast and Free C++ Library for Creating, Parsing an Manipulating PDF Files and Streams. 

Documentation is available [here](https://github.com/galkahana/PDF-Writer/wiki).
Project site is [here](http://www.pdfhummus.com). 

There is also a NodeJS module named [MuhammaraJS](https://github.com/julianhille/MuhammaraJS) wrapping PDFHummus PDF library and making it available for that language. It is the current and still supported version of a now deprecated [HummusJS](https://github.com/galkahana/HummusJS) project of mine, which [julianhille](https://github.com/julianhille) maintains.

# First time around

This is a C++ Project using CMake as project builder.
To build/develop You will need:

1. a compiler. for win you can use vs studio. choose community version - https://visualstudio.microsoft.com/
2. cmake - download from here - https://cmake.org/

Given that this is a Library and _not_ an executable, you can ignore this cmake setup and just use the code as is by copying the folders into your own project. There are, however, better ways to include the code in your own project. The project cmake setup defines PDFHummus as a package and allows you to import the project directly from this repo (remotely) or by pre-installing the package. The instructions below contains information about building the project locally, testing and installing it, as well as explaining how to use CMake `FetchContent` functionality in order to import the project automatically from this repo

For documentation about how to use the library API you should head for the Wiki pages [here](https://github.com/galkahana/PDF-Writer/wiki).

# Short tour of the project

There are 8 folders to this project:
- **FreeType, LibAesgm, LibJpeg, LibPng, LibTiff, Zlib**: 6 libraries that are dependencies to PDFWriter. They are bundled here for convenience. You don't have to use them to compile PDFWriter, but rather use what versions you have installed on your setup.
- **PDFWriter**: main folder, includes the library implementation
- **PDFWriterTesting**: test folder, includes test source code that's used with cmake testing application - ctest.

# Building, Installing and testing the project with CMake

Once you installed pre-reqs, you can now build the project.

## Create the project files

To build you project start by creating a project file in a "build" folder off of the cmake configuration, like this:

```bash
mkdir build
cd build
cmake ..
```

### options for generating Cmake project files

The project defines some optional flags to allow you to control some aspects of building PDFHummus.

- `PDFHUMMUS_NO_DCT` - defines whether to exclude DCT functionality (essentially - not include LibJpeg) from the library. defaults to `FALSE`. when setting `TRUE` the library will not require the existance of LibJpeg however will not be able to decode DCT streams from PDF files. (note that this has no baring on the ability to include JPEG images. That ability does not require LibJpeg given the innate ability of PDF files to include DCT encoded streams).
- `PDFHUMMUS_NO_TIFF` - defines whether to exclude TIFF functionality (essentially - not include LibTiff) from the library. defaults to `FALSE`. when setting `TRUE` the library will not require the existance of LibTiff however will not be able to embed TIFF images.
- `PDFHuMMUS_NO_PNG` -  defines whether to exclude PNG functionality (essentially - not include LibPng) from the library. defaults to `FALSE`. when setting `TRUE` the library will not require the existance of LibPng however will not be able to embed PNG images.
- `USE_BUNDLED` - defines whether to use bundled dependencies when building the project or use system libraries. defaults to `TRUE`. when defined as `FALSE`, the project configuration will look for installed versions of	LibJpeg, Zlib, LibTiff, FreeType, LibAesgm, LibPng and use them instead of the bundled ones (i.e. those contained in the project). Note that for optional dependencies - LibJpeg, LibTiff, LibPng - if not installed the coniguration will succeed but will automatically set the optional building flags (the 3 just described) according to the libraries avialability. As for required dependencies - FreeType, LibAesgm, Zlib - the configuration will fail if those dependencies are not found. see `USE_UNBUNDLED_FALLBACK_BUNDLED` for an alternative method to deal with dependencies not being found.
- `USE_UNBUNDLED_FALLBACK_BUNDLED` - Defines an alternative behavior when using `USE_BUNDLED=OFF` and a certain dependency is not installed on the system. If set to `TRUE` then for a dependency that's not found it will fallback on the bundled version of this dependency. This is essentially attempting to find installed library and if not avialable use a bundled one to ensure that the build will succeed.

You can set any of those options when calling the `cmake` command. For example to use system libraries replaced the earlier sequence with:

```bash
cd build
cmake .. -DUSE_BUNDLED=FALSE
```

## Build

Once you got the project file, you can now build the project. If you created an IDE file, you can use the IDE file to build the project.
Alternatively you can do so from the command line, again using cmake. 

The following builds the project from its root folder:
```bash
cmake --build build [--config release]
```

This will build the project inside the build folder. what's in brackets is optional and will specify a release onfiguration build. You will be able to look up the result library files per how you normally do when building with the relevant build environment. For example, for windows, the `build/PDFWriter/Release` folder will have the result PDFWriter file.

## Testing

This project uses `ctest` for running tests. `ctest` is part of cmake and should be installed as part of cmake installation.
The tests run various checks on PDFHummus...and I admit quite a lot of them are not great as unitests as they may just create PDF files without verifying they are good...one uses ones eyes to inspect the test files to do that...or revert to being OK with no exceptions being thrown, which is also good. They are decent as sample code to learn how to do things though 😬.

To run the project tests (after having created the project files in ./build) go:

```bash
ctest --test-dir build [-C release]
```

This should scan the folders for tests and run them.
Consider appending ` -j22` to the command in order to run tests in parallel to speed things up.


You should be able to see result output files from the tests under `./build/Testing/Output`.


Note that `ctest` does NOT build the project. It may fail if there's no previous build, or will not pick up on your changes if you made them
since the last build. For This purpose there's an extra target created in the project to make sure the project and test code is built (as well as recreating the output folder to clear previous runs output):

```bash
cmake --build build --target pdfWriterCheck [--config release]
```

## Installing

If you want, you can use the `install` verb of cmake to install a built product (the library files and includes). Use the prefix param to specify where you want the result to be installed to

```bash
cmake --install build --prefix ./etc/install [--config release]
```

This will install all the library files in `./etc/install`. You should see an "include" folder and a "lib" folder with include files and library files respectively.

# Using PDFHummus in your own project

If you want to use PDFHummus there are several methods:
- copying the sources to your project
- installing the project and including the result in your project
- using PDFHummus package in your cmake project

Not much to say about the first option. 2nd option just means to follow the installation instructions and then pointing to the resultant lib and include folders to build your project.

3rd option is probably the best, especially if you already have cmake in your project. This project has package definition for `PDFHummus` package, which means you can have cmake look for this package and include it in your project with `find_package`. Then link to the `PDFHummus::PDFWriter` target and you are done. Another option is to do this + allow for fetching the project content from the repo with `FetchContent`. Here's an example from the [PDF TextExtraction project](https://github.com/galkahana/pdf-text-extraction/blob/master/TextExtraction/CMakeLists.txt) of mine:

```cmake
include(FetchContent)

FetchContent_Declare(
  PDFHummus
  GIT_REPOSITORY https://github.com/galkahana/PDF-Writer.git
  GIT_TAG        4.5
  FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(PDFHummus)

target_link_libraries (TextExtraction PDFHummus::PDFWriter)
```

This will either download the project and build it or use an installed version (provided that one exists and has a matching version). 
Change the `GIT_TAG` value to what version you'd like to install. You can use tags, branches, commit hashs. anything goes.
Includes are included haha.

Note that when installing PDFHummus with the bundled libraries built (this is the default behvaior which can be changed by setting `USE_BUNDLED` variable to `FALSE`) there are additional targets that PDFHummus includes:
- PDFHummus::FreeType - bundled freetype library
- PDFHummus::LibAesgm - bundled aesgm library
- PDFHummus::LibJpeg - bundled libjpeg library
- PDFHummus::LibPng - bundled libpng library
- PDFHummus::LibTiff - bundled libtiff library
- PDFHummus:::Zlib - bundled zlib library

You can use those targets in additon or instead of using PDFWriter if this makes sense to your project (like if you are extracting images, having LibJpeg or LibPng around can be useful).

# Packaging PDFHummus for installing someplace else

The project contains definitions for `cpack`, cmake packaging mechanism. It might be useful for when you want to build PDFHummus and then install it someplace else.

The following will create a zip file with all libs and includes:
```
cd build
cpack .
```

# VSCode usage

If you are developing this project using vscode here's some suggestions to help you:  
- install vscode C++ extensions:
    - C/C++
    - C/C++ Extension Pack
    - C/C++ Themes
- install vscode cmake extensions:
    - Cmake
    - Cmake Tools
    - CMake Test Explorder

This should help you enable testing and debugging the tests in vscode.


# More building instructions for when you cant use cmake


## iOS

I wrote a post about how to compile and use the library for the iPhone and iPad environments. you can read it [here](http://pdfhummus.com/post/45501609236/how-to-build-iphone-apps-that-use-pdfhummus).
 

## Build insturctions for other scenraios

It should be quite simple to construct project files in the various building environments (say VS and Xcode) if you want them. Here are some pointers:
- All the PDFWriter sources are in PDFWriter folder (you can get it by downloading the git project or from the Downloads section).
- The library is dependent on the dlls/shared libraries of Zlib, LibTiff, LibJpeg, LibPng and FreeType. When linking - make sure they are available.
- The library should support well both 32 bit and 64 bit environments. It's using standard C++ libraries.
