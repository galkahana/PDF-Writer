Welcome to PDF-Writer, Also known as [PDFHummus](http://www.pdfhummus.com).   
PDFHummus is a Fast and Free C++ Library for Creating, Parsing an Manipulating PDF Files and Streams. 

Documentation is available [here](https://github.com/galkahana/PDF-Writer/wiki).
Project site is [here](http://www.pdfhummus.com). 

There is also a NodeJS module named [MuhammaraJS](https://github.com/julianhille/MuhammaraJS) wrapping PDFHummus PDF library and making it available for that language. It is the current and still supported version of a now deprecated [HummusJS](https://github.com/galkahana/HummusJS) project of mine, which [julianhille](https://github.com/julianhille) maintains.

**Note and clarification on earlier notes:** Since 9/11/2019 I ended support for this project. You may still use the code as is, with the provided license, however I will not be providing answers, solutions, responses etc. I may still develop the library further, making sure not to break API or change it in any other way to harm your existing implementations, however those developments are based solely on my own personal projects needs.

And now that we got that out of the way, let's move on for some basic instructions.

# First time around

This is a C++ Project using CMake as project builder.
To build/develop You will need:

1. a compiler. for win you can use vs studio. choose community version - https://visualstudio.microsoft.com/
2. cmake - download from here - https://cmake.org/

Given that this is a Library and _not_ an executable, you can ignore this cmake setup and just use the code as is by copying the folders into your own project. You may, however, find the cmake setup, as well as the rest of the instructions in the readme file useful for building the library files, testing the library and developing/debugging it. 

For documentation about how to use the library API you should head for the Wiki pages [here](https://github.com/galkahana/PDF-Writer/wiki).

# Short tour of the project

There are 8 folders to this project:
- **FreeType, LibAesgm, LibJpeg, LibPng, LibTiff, ZLib**: 6 libraries that are dependencies to PDFWriter. They are bundled here for convenience. You don't have to use them to compile PDFWriter, but rather use what versions you have installed on your setup.
- **PDFWriter**: main folder, includes the library implementation
- **PDFWriterTesting**: test folder, includes test source code that's used with cmake testing application - ctest.

# Building, Installing and testing the project

Once you installed pre-reqs, you can now build the project.

## Create the project files

To build you project start by creating a project file in a "build" folder off of the cmake configuration, like this:

```bash
mkdir build
cd build
cmake ..
```

## Build and install

Once you got the project file, you can now build the project. If you created an IDE file, you can use the IDE file to build the project.
Alternatively you can do so from the command line, again using cmake. 

The following builds the project from its root folder:
```bash
cmake --build build --config release
```

This will build the project inside the build folder. You will be able to look up the result library files per how you normally do when building with the relevant build environment. For example, for windows, the `build/PDFWriter/Release` folder will have the result PDFWriter file.

If you want, you can use the `install` verb of cmake to install a built product (the library files and includes). Use the prefix param to specify where you want the result to be installed to

```bash
cmake --install ./build/. --prefix ./etc/install --config release
```

This will install all the library files in `./etc/install`. You should see an "include" folder and a "lib" folder with include files and library files respectively.

if you do not have `cmake --install` as option, you can use a regular build with install target instead, and specify the install target in configuration stage, like this:

```bash
cd build
cmake .. -DCMAKE_INSTALL_PREFIX="..\etc\install"
cd ..

cmake --build build --config release --target install
```

_There are more configuration options with the cmake setup for this project_. Specifically you can build with system libraries instead of with the bundled libraries. You can read about those options [here](https://github.com/galkahana/PDF-Writer/pull/70) and [here](https://github.com/galkahana/PDF-Writer/wiki/Building-and-running-samples#special-options-with-cmake).

## Testing

This project uses `ctest` for running tests. `ctest` is part of cmake and should be installed as part of cmake installation.
The tests run various checks on PDFHummus...and I admit quite a lot of them are not great as unitests as they may just create PDF files without verifying they are good...one uses ones eyes to inspect the test files to do that...or revert to being OK with no exceptions being thrown, which is also good. They are decent as sample code to learn how to do things though 😬.

To run the project tests (after having created the project files in ./build) go:

```bash
ctest --test-dir build -C release
```

This should scan the folders for tests and run them.
Consider appending ` -j22` to the command in order to run tests in parallel to speed things up.


You should be able to see result output files from the tests under `./build/Testing/Output`.


Note that `ctest` does NOT build the project. It may fail if there's no previous build, or will not pick up on your changes if you made them
since the last build. For This purpose there's an extra target created in the project to make sure the project and test code is built (as well as recreating the output folder to clear previous runs output):

```bash
cmake --build build --target pdfWriterCheck --config release
```

## VSCode usage

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
