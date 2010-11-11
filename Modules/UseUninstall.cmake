# - Uninstall target.
# Use this module to provide unstall target.
#
# Define following targets
#   uninstall: For uninstalling the package.
#

IF(NOT DEFINED _USE_UNINSTALL_CMAKE_)
    SET(_USE_UNINSTALL_CMAKE_ "DEFINED")
    FIND_FILE(_cmake_uninstall_in cmake_uninstall.cmake.in
	PATHS ${CMAKE_ROOT}/Modules ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Modules)

    IF("${_cmake_uninstall_in}" STREQUAL "_cmake_uninstall_in-NOTFOUND")
	MESSAGE(FATAL_ERROR "Cannot find cmake_uninstall.cmake.in in ${CMAKE_ROOT}/Modules"
	   " ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Modules")
    ENDIF("${_cmake_uninstall_in}" STREQUAL "_cmake_uninstall_in-NOTFOUND")

    CONFIGURE_FILE("${_cmake_uninstall_in}"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	IMMEDIATE @ONLY)

    ADD_CUSTOM_TARGET(uninstall
	"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	)
ENDIF(NOT DEFINED _USE_UNINSTALL_CMAKE_)

