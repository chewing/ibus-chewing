# Location of library include files
INCLUDE_DIRECTORIES(${GTK2_INCLUDE_DIRS} ${IBUS_INCLUDE_DIRS}
    ${CHEWING_INCLUDE_DIRS} ${GCONF2_INCLUDE_DIRS})

# Library location for the linker
LINK_DIRECTORIES(${GTK2_LIBRARY_DIRS} ${IBUS_LIBRARY_DIRS}
    ${CHEWING_LIBRARY_DIRS} ${GCONF2_LIBRARY_DIRS})

#==================================================================
# Sources
#
SET(GOB_GENERATED ${CMAKE_HOME_DIRECTORY}/src/ibus-chewing-engine.c
    ${CMAKE_HOME_DIRECTORY}/src/ibus-chewing-engine.h
    ${CMAKE_HOME_DIRECTORY}/src/ibus-chewing-engine-private.h)


ADD_CUSTOM_COMMAND(OUTPUT ${GOB_GENERATED}
    COMMAND ${GOB2} ${CMAKE_CURRENT_SOURCE_DIR}/IBusChewingEngine.gob -o ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS IBusChewingEngine.gob IBusChewingEngine-def.c
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_CUSTOM_TARGET(gob
    DEPENDS  ${GOB_GENERATED}
)

SET_SOURCE_FILES_PROPERTIES(${GOB_GENERATED}
    PROPERTIES GENERATED TRUE)


# executable built from some source
ADD_EXECUTABLE(ibus-engine-chewing main.c ibus-chewing-engine.c) 

# Link the executable to the library.
TARGET_LINK_LIBRARIES(ibus-engine-chewing ${GTK2_LIBRARIES} ${IBUS_LIBRARIES}
    ${CHEWING_LIBRARIES} ${GCONF2_LIBRARIES})


IF(NOT DEFINED LIB_INSTALL_DIR)
    IF( $ENV{MACHTYPE} MATCHES "64")
	SET(LIB_INSTALL_DIR lib64)
	ADD_DEFINITIONS(-DLIB_INSTALL_DIR='"${LIB_INSTALL_DIR}"')
    ELSE()
	SET(LIB_INSTALL_DIR lib)
	ADD_DEFINITIONS(-DLIB_INSTALL_DIR='"${LIB_INSTALL_DIR}"')
    ENDIF()
ENDIF()


INSTALL(TARGETS ibus-engine-chewing
     RUNTIME DESTINATION libexec
     LIBRARY DESTINATION ${LIB_INSTALL_DIR}
)

INSTALL(FILES chewing.xml
    DESTINATION "${datadir}/ibus/component")

SET(GCONF_CONFIG_SOURCE "")
#EXECUTE_PROCESS(
#    COMMAND gconftool-2 --get-default-source
#    COMMAND tr \\n \\t
#    COMMAND sed  -e s/\\t//
#    OUTPUT_VARIABLE GCONF_CONFIG_SOURCE
#    )

SET(ENV{GCONF_CONFIG_SOURCE} ${GCONF_CONFIG_SOURCE})

ADD_CUSTOM_TARGET(uninstall_schema
    COMMAND GCONF_CONFIG_SOURCE=${GCONF_CONFIG_SOURCE} 
    gconftool-2 --makefile-uninstall-rule ${SYSCONF_INSTALL_DIR}/gconf/schemas/${PROJECT_NAME}.schema
    DEPENDS ${SYSCONF_INSTALL_DIR}/gconf/schemas/${PROJECT_NAME}.schema
    COMMENT "Install_schema"
    )

ADD_CUSTOM_TARGET(install_schema
    COMMAND GCONF_CONFIG_SOURCE=${GCONF_CONFIG_SOURCE}
    gconftool-2 --makefile-install-rule ${SYSCONF_INSTALL_DIR}/gconf/schemas/${PROJECT_NAME}.schema
    DEPENDS ${PROJECT_NAME}.schema
    COMMENT "Install_schema"
    )
    

INSTALL(FILES ibus-chewing.schema
    DESTINATION "${SYSCONF_INSTALL_DIR}/gconf/schemas")


