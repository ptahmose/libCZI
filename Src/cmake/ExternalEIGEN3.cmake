include(ExternalProject)

ExternalProject_Add(
  eigen_ext
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  UPDATE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  LOG_DOWNLOAD ON
  LOG_CONFIGURE ON)

ExternalProject_Get_Property(eigen_ext source_dir)
set(EIGEN3_INCLUDE_DIRS ${source_dir})