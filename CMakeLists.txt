# simplified from: 
#   1. https://cmake.org/cmake/help/latest/guide/tutorial/A%20Basic%20Starting%20Point.html 
#   2. https://stackoverflow.com/questions/21163188/most-simple-but-complete-cmake-example
#   3. https://cmake.org/cmake/help/latest/guide/tutorial/Adding%20Generator%20Expressions.html 
# CMake is space sensitive. `_AND_ID:CXX,ARMClang,` can't be `_AND_ID: CXX, ARMClang, `

# usage: (ou-of-source build) 
#   0. download CMake: https://cmake.org/download/
#   1. `cd build`
#   2. `cmake ../. -G "MinGW Makefiles"`, or `cmake ../. -G "Visual Studio 17 2022"`, etc 
#   3. `cmake --build .`, or the usual way environment do, e.g., `mingw32-make`, IDE run button, etc
#   4. executable built!
# clean all: (`/s`: Deletes specified dir and all its subdir)
#   1. `rmdir  /s .\`
#   2. alt + tab out your IDE to prevent cannot delete `because it is being used by another process.`
# clean built file only:
#   `cmake --build . --target clean`

# Note: CMake generated Makefiles/soluion need cmake to run & not designed to be portable!
#   source https://stackoverflow.com/questions/48167079/run-generated-makefile-without-cmake-installed
#   only hand-written portable Makefile / Visual Studio "File > New > Project From Existing Code" can be portable


# CMake & C++ Settings 
cmake_minimum_required(VERSION 3.15) # recommanded by 4.0.1 official tutorial
project(ImgSharpen)

set(CMAKE_CXX_STANDARD 17)  # for <filesystem> etc useful
set(CMAKE_CXX_STANDARD_REQUIRED True)


# glob all source files (and remove unwanted) & assets
file(GLOB_RECURSE sources *.cpp *.h)
file(GLOB_RECURSE not_source 
  ${CMAKE_SOURCE_DIR}/TinyTIFF-4.0.1.0/tests/*.cpp 
  ${CMAKE_SOURCE_DIR}/TinyTIFF-4.0.1.0/tests/*.h
  */CMakeFiles/*.cpp
  */CMakeFiles/*.h
)
list(REMOVE_ITEM sources ${not_source})

file(GLOB_RECURSE test_images test_images/*)


# turn on compiler warnings (and -Wno warnings from stb)
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

add_library(warning_compiler_flags INTERFACE)
target_compile_options(warning_compiler_flags INTERFACE
  "$<${gcc_like_cxx}:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused;-Wno-missing-field-initializers;-Wno-unused-parameter;-Wno-unused-function>"
  "$<${msvc_cxx}:-W3>"
)


# build (and find) libraries (added by its CMakeList.txt's `add_library`)
set(TinyTIFF_BUILD_TESTS OFF)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(./TinyTIFF-4.0.1.0)


# build exe, link libraries & copy assets (manage assets related to exe) when building (copy only when different)
add_executable(ImgSharpen ${sources})
target_link_libraries(ImgSharpen PUBLIC warning_compiler_flags TinyTIFF) 

add_custom_command(
    TARGET ImgSharpen POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different 
    ${CMAKE_SOURCE_DIR}/test_images $<TARGET_FILE_DIR:ImgSharpen>/test_images
)
