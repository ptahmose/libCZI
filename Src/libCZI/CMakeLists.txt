
file(GLOB LIBCZISRCFILES "*.h" "*.cpp" "*.hpp")

#add_library(NaCZIrLib SHARED CreateBitmap.cpp CziDimensionInfo.cpp CziMetadata.cpp CziMetadataDocumentInfo.cpp CziMetadataSegment.cpp CziParse.cpp CZIReader.cpp CziSubBlock.cpp CziSubBlockDirectory.cpp CziUtils.cpp decoder.cpp DimCoordinate.cpp dllmain.cpp MultiChannelTileCompositor.cpp NaCZIrLib.cpp NaCZIr_Utilities.cpp SingleChannelTileAccessor.cpp SingleChannelTileCompositor.cpp stdafx.cpp stdAllocator.cpp bitmapData.h CreateBitmap.h CziDataStructs.h CziDimensionInfo.h CziMetadata.h CziMetadataDocumentInfo.h CziMetadataSegment.h CziParse.h CZIReader.h CziStructs.h CziSubBlock.h CziSubBlockDirectory.h CziUtils.h datastore.h decoder.h DimCoordinate.h DimensionIndexExtended.h ImportExport.h MultiChannelTileCompositor.h NaCZIr.h NaCZIr_Compositor.h NaCZIr_DimCoordinate.h NaCZIr_Metadata.h NaCZIr_Pixels.h NaCZIr_Utilities.h readerwriterlocker.h SingleChannelTileAccessor.h SingleChannelTileCompositor.h stdafx.h stdAllocator.h targetver.h utilities.cpp utilities.h pugiconfig.hpp pugixml.cpp pugiconfig.hpp CziDisplaySettings.h CziDisplaySettings.cpp NaCZIr_Site.h NaCZIr_Site.cpp BitmapOperations.cpp BitmapOperations.h MD5Sum.cpp MD5Sum.h splines.h splines.cpp Site.h priv_guiddef.h NaCZIr_exceptions.h SingleChannelPyramidLevelTileAccessor.cpp SingleChannelPyramidLevelTileAccessor.h SingleChannelAccessorBase.cpp SingleChannelAccessorBase.h SingleChannelScalingTileAccessor.cpp SingleChannelScalingTileAccessor.h StreamImpl.cpp StreamImpl.h)


# prepare the configuration-file "libCZI_Config.h"
if (IS_BIG_ENDIAN)
 set(libCZI_ISBIGENDIANHOST 1)
else()
 set(libCZI_ISBIGENDIANHOST 0)
endif()

if (HAVE_ALIGNED_ALLOC)
 set(libCZI_HAVE_ALIGNED_ALLOC 1)
else()
 set(libCZI_HAVE_ALIGNED_ALLOC 0)
endif()
if (HAVE__ALIGNED_MALLOC)
 set(libCZI_HAVE__ALIGNED_MALLOC 1)
else()
 set(libCZI_HAVE__ALIGNED_MALLOC 0)
endif()

set(libCZI_HAVE_ENDIAN_H ${HAVE_ENDIAN_H}) 

string(CONCAT libCZI_CompilerIdentification "\"" ${CMAKE_CXX_COMPILER_ID} " " ${CMAKE_CXX_COMPILER_VERSION} "\"")

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/libCZI_Config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/libCZI_Config.h")


#  Define headers for this library. PUBLIC headers are used for compiling the library, and will be added to consumers' build paths.
set(libCZIPublicHeaders "ImportExport.h" "libCZI.h" "libCZI_Compositor.h" "libCZI_DimCoordinate.h" "libCZI_exceptions.h"
						   "libCZI_Helpers.h" "libCZI_Metadata.h" "libCZI_Metadata2.h" "libCZI_Pixels.h"
						   "libCZI_Site.h" "libCZI_Utilities.h" "priv_guiddef.h")


if (LIBCZI_BUILD_DYNLIB)
	add_library(libCZI SHARED ${LIBCZISRCFILES} ${LIBCZISRCEIGENFILES}  $<TARGET_OBJECTS:JxrDecodeStatic>)
	set_target_properties(libCZI PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES)
	SET_TARGET_PROPERTIES (libCZI PROPERTIES DEFINE_SYMBOL  "LIBCZI_EXPORTS" )
	target_include_directories(libCZI PRIVATE  ${CMAKE_CURRENT_BINARY_DIR}) # this is necessary so that we can find "libCZI_Config.h" which we created above
	target_include_directories(libCZI PRIVATE  ${EIGEN3_INCLUDE_DIR})
	set_target_properties(libCZI PROPERTIES DEBUG_POSTFIX "d")
  set_target_properties(libCZI PROPERTIES PUBLIC_HEADER "${libCZIPublicHeaders}")
	IF(UNIX)
		target_compile_options(libCZI PRIVATE -D_FILE_OFFSET_BITS=64)
  ENDIF(UNIX)
	if (EXTERNALPROJECT_EIGEN3)
		add_dependencies(libCZI eigen_ext)
	endif()
endif(LIBCZI_BUILD_DYNLIB)

if (LIBCZI_BUILD_STATICLIB)
  # Notes: -we use JxrDecode as an "object-library" in order have it "embedded" into libCZI.a
	add_library(libCZIStatic STATIC ${LIBCZISRCFILES} ${LIBCZISRCEIGENFILES} $<TARGET_OBJECTS:JxrDecodeStatic>)
	set_target_properties(libCZIStatic PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES)
	target_compile_definitions(libCZIStatic PRIVATE _LIBCZISTATICLIB=1)
	target_include_directories(libCZIStatic PRIVATE  ${CMAKE_CURRENT_BINARY_DIR})  # this is necessary so that we can find "libCZI_Config.h" which we created above
	target_include_directories(libCZIStatic PRIVATE  ${EIGEN3_INCLUDE_DIR})
	set_target_properties(libCZIStatic PROPERTIES DEBUG_POSTFIX "d")
  set_target_properties(libCZIStatic PROPERTIES PUBLIC_HEADER "${libCZIPublicHeaders}")
	IF(UNIX)
		target_compile_options(libCZIStatic  PRIVATE -D_FILE_OFFSET_BITS=64)
  ENDIF(UNIX)
	if (EXTERNALPROJECT_EIGEN3)
		add_dependencies(libCZIStatic eigen_ext)
	endif()
endif(LIBCZI_BUILD_STATICLIB)

#
# Notes: - the variables CMAKE_INSTALL_<...> have been defined in the module "GNUInstallDirs"
#install(FILES ${libCZIPublicHeaders} DESTINATION  ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
if (LIBCZI_BUILD_DYNLIB)
	install(TARGETS libCZI
	#		 EXPORT "libCZI"
			 ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
			 LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
			 RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
			 PUBLIC_HEADER DESTINATION  ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
endif(LIBCZI_BUILD_DYNLIB)

if (LIBCZI_BUILD_STATICLIB)
	install(TARGETS libCZIStatic
	#		 EXPORT "libCZIStatic"
			 ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
			 LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
			 RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
			 PUBLIC_HEADER DESTINATION  ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
endif(LIBCZI_BUILD_STATICLIB)