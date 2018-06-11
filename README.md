# HDR
High-dynamic-range imaging


## System requirements and Dependencies

- [CMake 3.6+](https://cmake.org/)
- [OpenCV 3.2+](http://opencv.org/)

The following compilers are supported:

- Visual Studio 2015 or 2017


## Build

### Windows & Mac OS X & Linux

We have recently changed from premake to [CMake](https://cmake.org/).
Below are instructions of using a GCC-based compiler as an example.

1. Create your build directory `mkdir HDR_build`
2. Run CMake `cmake ..` or `cmake -DCMAKE_BUILD_TYPE=Debug` for debugging
3. Compile by running `make`
4. The binary is named hdri.

Note that for Windows platform you may need to copy necessary files such as dlls to the working directory in order to execute the binary properly.


## Usage

`./hdri (BaseDirPath) (FileListName)`

For example:

`./hdri ../InputImage/ list.txt`

Note: The default base path is "../InputImage/" and the default name of the file list is "list.txt". You do not have to type it explicitly.

The program will generate the radiance map called "radiance.hdr". Based on the radiance map, the tone algorithm will be triggered to generate the output image. The file name will be "output_image.jpg".
Moreover, the reconstructed G curve (and ln E) will be written to a file called "out.txt".


## Layout

Inputs:

The inputs are all stored in the folder named "\InputImage" with subfolders indexed by numbers.

Outputs:

They are in the "\Result" folder with corresponding indice. Each subfolder contains the output image and the radiance map.


## Reference

- [Recovering High Dynamic Range Radiance Maps from Photographs](http://www.pauldebevec.com/Research/HDR/)
- [Paper Link](https://dl.acm.org/citation.cfm?id=258884)
