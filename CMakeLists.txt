

cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install)


set(HDRI_DIR ${CMAKE_SOURCE_DIR}/HDRI)

# add ARCH check
if(NOT "${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
    set(PATH_MODIFIER "x86")
    set(POSTFIX_MODIFIER "")
    set(FILENAME_MODIFIER "32")
elseif("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
    set(PATH_MODIFIER "x64")
    set(POSTFIX_MODIFIER "_x64")
    set(FILENAME_MODIFIER "64")
else()
    message(FATAL_ERROR "${CMAKE_GENERATOR} missing! Can't continue")
endif()


project(HDR LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE TRUE)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()


if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()


#debug messages
message("CMAKE_BUILD_TYPE= ${CMAKE_BUILD_TYPE}")
message("CMAKE_SOURCE_DIR= ${CMAKE_SOURCE_DIR}")
message("CMAKE_BINARY_DIR= ${CMAKE_BINARY_DIR}")
message("CMAKE_INSTALL_PREFIX= ${CMAKE_INSTALL_PREFIX}")
message("CMAKE_CXX_STANDARD= ${CMAKE_CXX_STANDARD}")
message("CMAKE_SYSTEM_PREFIX_PATH= ${CMAKE_SYSTEM_PREFIX_PATH}")
message("OpenCV_INCLUDE_DIRS= ${OpenCV_INCLUDE_DIRS}")
message("CMAKE_CXX_FLAGS= ${CMAKE_CXX_FLAGS}")
message("CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
message("CMAKE_GENERATOR= ${CMAKE_GENERATOR}")


set(BASE_SRC

    HDRI/hdr_image.cpp
    HDRI/debevec_weight.cpp
    HDRI/raw_image.cpp
    HDRI/tone_map_algo.cpp
    HDRI/main.cpp
)


add_executable(
    hdri
    ${BASE_SRC}
)

include_directories(${HDRI_DIR}/include)

target_link_libraries( hdri ${CONAN_LIBS} )
target_compile_features(hdri PUBLIC cxx_std_17)

