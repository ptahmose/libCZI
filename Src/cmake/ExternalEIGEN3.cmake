
include(ExternalProject)

ExternalProject_Add(
  eigen_ext
  PREFIX "vendor/eigen3"
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG        39142904cc2301628931481e8b331cc2d567e22f  # this is "known-to-work", just in case
  CMAKE_ARGS
    -DEIGEN_BUILD_PKGCONFIG=OFF
    -DBUILD_TESTING=OFF
  UPDATE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  LOG_DOWNLOAD ON   # redirect output to log-file (so that we have less clutter)
  LOG_CONFIGURE ON  # redirect output to log-file (so that we have less clutter)
)

ExternalProject_Get_Property(eigen_ext source_dir)
set(EIGEN3_INCLUDE_DIR ${source_dir})


#[[
 include(FetchContent)

 set(EIGEN_BUILD_TESTING OFF)
 set(EIGEN_ADD_BLAS_LAPACK_TARGETS 0)
 set(EIGEN_TEST_NOQT 1)

FetchContent_Declare(
      eigen_ext
      GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
      UPDATE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
    )

FetchContent_GetProperties(eigen_ext)

if(NOT eigen_ext_POPULATED)
        FetchContent_Populate(eigen_ext)

	  	  add_subdirectory(${eigen_ext_SOURCE_DIR} ${eigen_ext_SOURCE_DIR})
        set(EIGEN3_INCLUDE_DIR ${eigen_ext_SOURCE_DIR})
		    #set(GTEST_INCLUDE_DIRS ${googletest_SOURCE_DIR}/googletest/include)
		    #set(GTEST_BOTH_LIBRARIES ${CMAKE_BINARY_DIR}/lib/libgtest.a ${CMAKE_BINARY_DIR}/lib/libgtest_main.a)    
endif()
]]