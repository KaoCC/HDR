# HDR
High-dynamic-range imaging


## System requirements and Dependencies

- [CMake 3.8+](https://cmake.org/)
- [Conan 1.40+](https://conan.io/)

The following compilers are tested:

- Visual Studio 2017 (Windows x64)
- GCC 5.5.0 (Linux x64)
- Apple LLVM version 13.0.0 (Mac x64)

## Build

### Windows & Mac OS X & Linux

1. Create the build directory `mkdir build && cd build`
2. Run Conan `conan install .. -s compiler.cppstd=17 --build missing` . The dependencies should be resolved by conan
3. Run CMake `cmake ..` for development or `cmake -DCMAKE_BUILD_TYPE=Release` for a release build
4. Compile by running `make`
5. Run test case by `make test`
6. The binary is named hdri

Note that for Windows platform you may need to copy necessary files such as dlls to the working directory in order to execute the binary properly.


## Usage

`./hdri [BaseDirPath] [FileListName]`

For example:

`./hdri ../InputImage/ list.txt`

Note: The default base path is "../InputImage/" and the default name of the file list is "list.txt". You do not have to type it explicitly.

The program will first generate the radiance map. After that, it will generate the output image using tone mapping.
Moreover, the reconstructed G curve (and ln E) will be written to a file.


## Layout

Inputs:

The inputs are all stored in the folder named "\InputImage" with subfolders indexed by numbers.

Outputs:

They are in the "\Result" folder with corresponding indice. Each subfolder contains the output image and the radiance map.


## Reference

- [Recovering High Dynamic Range Radiance Maps from Photographs](http://www.pauldebevec.com/Research/HDR/)
- [Paper Link](https://dl.acm.org/citation.cfm?id=258884)
