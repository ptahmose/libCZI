cmake_minimum_required(VERSION 3.11)

find_package(GTest QUIET)

if ("${GTest_FOUND}")
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
	  	  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
		    set(GTEST_INCLUDE_DIRS ${googletest_SOURCE_DIR}/googletest/include)
		    set(GTEST_BOTH_LIBRARIES ${CMAKE_BINARY_DIR}/lib/libgtest.a ${CMAKE_BINARY_DIR}/lib/libgtest_main.a)    
    endif()
endif()

add_executable( libCZI_UnitTests 
                    pch.cpp 
                    pch.h 
                    test_bitmapOperations.cpp
                    testImage.cpp
                    testImage.h "test_splines.cpp" "test_reader.cpp")

target_include_directories(libCZI_UnitTests PRIVATE ${GTEST_INCLUDE_DIRS} )
target_include_directories(libCZI_UnitTests PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../libCZI/>
)
target_link_libraries(libCZI_UnitTests PRIVATE ${GTEST_BOTH_LIBRARIES} libCZIStatic)

set_target_properties(libCZI_UnitTests PROPERTIES DEBUG_POSTFIX "d")

add_test( libCZI_UnitTests  libCZI_UnitTests  )