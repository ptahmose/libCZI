cmake_minimum_required(VERSION 3.0.2)

IF(UNIX)
# linking with 'thread' is necessary if we use std::thread and related under Linux it seems
#   otherwise - the program simply crashes (no build-error)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11 -fPIC -D_FILE_OFFSET_BITS=64")
   SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__ANSI__ -fPIC -D_FILE_OFFSET_BITS=64")
ENDIF(UNIX)

IF(CMAKE_COMPILER_IS_GNUCXX)
  LIST(APPEND CMAKE_CXX_FLAGS "-Wno-psabi") # get rid of the spurious "note: parameter passing for argument of type xxx changed in GCC 7.1"
  #SET(CMAKE_CXX_FLAGS "-Wno-psabi")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

include(GNUInstallDirs)

# add the folder "cmake" (in this repo) to CMake's module path
list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include (TestLargeFile)
test_large_files (_LARGEFILES)

include (TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if (IS_BIG_ENDIAN)
 message("identified a big-endian platform")
else()
 message("identified a little-endian platform")
endif()

check_include_file_CXX(endian.h HAVE_ENDIAN_H)
if (NOT DEFINED HAVE_ENDIAN_H OR NOT HAVE_ENDIAN_H)
	set(HAVE_ENDIAN_H 0)
endif()
message("check for endian.h -> ${HAVE_ENDIAN_H}") 

include (CheckCXXSymbolExists)
check_cxx_symbol_exists(aligned_alloc stdlib.h HAVE_ALIGNED_ALLOC)
if (NOT HAVE_ALIGNED_ALLOC)
  check_cxx_symbol_exists(_aligned_malloc stdlib.h HAVE__ALIGNED_MALLOC)
endif()

include(ExternalEIGEN3)
message("EIGEN3 -> ${EIGEN3_INCLUDE_DIRS}")

option(LIBCZI_BUILD_DYNLIB "Build dynamic library 'libCZI'." ON)
option(LIBCZI_BUILD_STATICLIB "Build static library 'libCZI'." ON)

add_subdirectory(JxrDecode)
add_subdirectory(libCZI)

option(LIBCZI_BUILD_CZICMD "Build application 'CZICmd'." ON)
if (LIBCZI_BUILD_CZICMD)
    add_subdirectory(CZICmd)
endif(LIBCZI_BUILD_CZICMD)
