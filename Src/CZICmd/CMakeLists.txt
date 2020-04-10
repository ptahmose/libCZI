IF(UNIX)
find_package(ZLIB)
find_package(PNG)
ENDIF(UNIX)

add_executable(CZIcmd cmdlineoptions.cpp consoleio.cpp execute.cpp getOpt.cpp CZIcmd.cpp SaveBitmap.cpp stdafx.cpp streamimpl.cpp cmdlineoptions.h consoleio.h execute.h getOpt.h inc_libCZI.h SaveBitmap.h stdafx.h streamimpl.h targetver.h utils.cpp utils.h platform_defines.h DisplaySettingsHelper.h)

set_target_properties(CZIcmd PROPERTIES CXX_STANDARD 11)
target_compile_definitions(CZIcmd PRIVATE _STATICLIBBUILD)
target_compile_definitions(CZIcmd PRIVATE UNICODE _UNICODE )
target_link_libraries(CZIcmd PRIVATE libCZIStatic)
IF(UNIX)
target_link_libraries(CZIcmd PRIVATE ${PNG_LIBRARIES} ${ZLIB_LIBRARIES})
include_directories(${ZLIB_INCLUDE_DIR} ${PNG_INCLUDE_DIR})
ENDIF(UNIX)


