include(ExternalProject)

ExternalProject_Add(
  eigen_ext
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  #GIT_TAG        39142904cc2301628931481e8b331cc2d567e22f  # this is "known-to-work", just in case
  UPDATE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  #LOG_DOWNLOAD ON
  #LOG_CONFIGURE ON
  )

ExternalProject_Get_Property(eigen_ext source_dir)
set(EIGEN3_INCLUDE_DIRS ${source_dir})