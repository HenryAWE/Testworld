# Testworld
A C++/OpenGL project for practicing programing

## Build
### Required additional libraries
- SDL 2
- libconfig

You can install required libraries via [**vcpkg**](https://github.com/Microsoft/vcpkg)  
For example: `vcpkg install sdl2`  
(Note: you may need to specify `x64:windows` when you want to build 64-bit version, please read the document of **vcpkg** for more information)


### Build the project
This project uses [**CMake**](https://cmake.org/) as its build system  
The minimum requested version is 3.12  
- Configuring  
  `mkdir build`  
  `cd build`  
  `cmake ..`  
- Building  
  `cmake --build .`
