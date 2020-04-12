cmake_minimum_required(VERSION 3.11)

find_package(GTest QUIET)
find_package(GMock QUIET)

if ("${GTest_FOUND}" AND "${GMock_FOUND}")
#if ("${GTest_FOUND}")
    message(STATUS "Found GTest, using it.")
else()
    message(STATUS "Did not find package GTest, will attempt to fetch it locally.")

    include(FetchContent)

    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG        release-1.10.0
    )
    FetchContent_GetProperties(googletest)
    if(NOT googletest_POPULATED)
        FetchContent_Populate(googletest)

        # we need to patch this file for solving a compilation-error on OpenBSD -> https://github.com/google/googletest/pull/2810
        file(COPY patch_gtest/gtest-port.cc DESTINATION ${googletest_SOURCE_DIR}/googletest/src)
	  	  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
		    set(GTEST_INCLUDE_DIRS ${googletest_SOURCE_DIR}/googletest/include)
		    set(GTEST_BOTH_LIBRARIES ${CMAKE_BINARY_DIR}/lib/libgtest.a ${CMAKE_BINARY_DIR}/lib/libgtest_main.a)    
    endif()
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_executable( libCZI_UnitTests 
                    pch.cpp 
                    pch.h 
                    test_bitmapOperations.cpp
                    testImage.cpp
                    testImage.h 
                    "test_splines.cpp" 
                    "test_reader.cpp" 
                    "test_CziSubBlockDirectory.cpp" 
                    "test_DimCoordinate.cpp" 
                    "test_DisplaySettings.cpp" 
                    "test_IndexSet.cpp" 
                    "test_LUT.cpp" 
                    "MockMetadataSegment.cpp" 
                    "main.cpp" 
                    "test_Utilities.cpp")

if (UNIX)
  find_package(Threads REQUIRED)
  if(THREADS_HAVE_PTHREAD_ARG)
    set_property(TARGET libCZI_UnitTests PROPERTY COMPILE_OPTIONS "-pthread")
    set_property(TARGET libCZI_UnitTests PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
  endif()
  if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(libCZI_UnitTests PRIVATE "${CMAKE_THREAD_LIBS_INIT}")
  endif()
endif()

message(STATUS ${GTEST_INCLUDE_DIRS})

target_include_directories(libCZI_UnitTests PRIVATE ${GMOCK_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS} $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../libCZI/>)

#target_link_libraries(libCZI_UnitTests PRIVATE ${GTEST_BOTH_LIBRARIES} ${GMOCK_LIBRARIES} libCZIStatic)
target_link_libraries(libCZI_UnitTests PRIVATE gtest gmock libCZIStatic)

set_target_properties(libCZI_UnitTests PROPERTIES DEBUG_POSTFIX "d")

set_target_properties(libCZI_UnitTests PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES) 

add_test( libCZI_UnitTests  libCZI_UnitTests  )