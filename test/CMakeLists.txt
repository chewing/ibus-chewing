#==================================================================
# Variable definition
#

##== Location of library include files
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/bin
    )

IF("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    ADD_DEFINITIONS("-DVERBOSE_LEVEL=DEBUG")
ELSE()
    ADD_DEFINITIONS("-DVERBOSE_LEVEL=MSG")
ENDIF()

ADD_DEFINITIONS("-DUNIT_TEST=1")

SET_SOURCE_FILES_PROPERTIES(${GOB_GENERATED_ibus-chewing-engine}
    ${GOB_GENERATED_maker-dialog} PROPERTIES GENERATED true
    )

#==================================================================
# Dependencies
#

FOREACH(_d ${IBUS_CHEWING_DEPENDENCIES})
    INCLUDE_DIRECTORIES(${${_d}_INCLUDE_DIRS})
    LINK_DIRECTORIES(${${_d}_LIBRARY_DIRS})
ENDFOREACH(_d ${IBUS_CHEWING_DEPENDENCIES})

## ADD_SOURCE_TEST(<testName> [NO_DEFAULT_SOURCE]
##     [SOURCES <src1> <src2...>]
##     [LIBRARIES <lib1> <lib2...>])
MACRO(ADD_SOURCE_TEST testName)
    SET(${testName}_LIBRARIES "")
    SET(${testName}_SOURCES "")
    SET(defaultSource 1)
    SET(mode "")
    FOREACH(arg ${ARGN})
        IF("${arg}" STREQUAL "LIBRARIES")
            SET(mode "${arg}")
        ELSEIF("${arg}" STREQUAL "SOURCES")
            SET(mode "${arg}")
        ELSEIF("${arg}" STREQUAL "NO_DEFAULT_SOURCE")
            SET(defaultSource 0)
            SET(mode "")
        ELSE()
            IF("${mode}" STREQUAL "LIBRARIES")
                LIST(APPEND ${testName}_LIBRARIES "${arg}")
            ELSEIF("${mode}" STREQUAL "SOURCES")
                LIST(APPEND ${testName}_SOURCES "${arg}")
            ENDIF()
        ENDIF()

        IF(defaultSource EQUAL 1)
            LIST(APPEND ${testName}_SOURCES
                "${CMAKE_SOURCE_DIR}/src/${testName}.c"
                )
        ENDIF()
    ENDFOREACH()

    ADD_EXECUTABLE(${testName}-test
        ${CMAKE_CURRENT_SOURCE_DIR}/${testName}-test.c
        ${${testName}_SOURCES}
        )

    TARGET_LINK_LIBRARIES(${testName}-test
        ${${testName}_LIBRARIES}
        ${GLIB2_LIBRARIES}
        )

    ADD_TEST("${testName}"
        gtester ${CMAKE_CURRENT_BINARY_DIR}/${testName}-test
        )

ENDMACRO(ADD_SOURCE_TEST testName)

#===========================================
#  Source File Tests
#

ADD_SOURCE_TEST(MakerDialogUtil
    SOURCES ${CMAKE_SOURCE_DIR}/src/MakerDialogUtil.c
    LIBRARIES ${GLIB2_LIBRARIES}
    )

ADD_SOURCE_TEST(MakerDialogBackend
    SOURCES ${COMMON_SOURCE_FILES} ${BACKEND_SOURCE_FILES}
    ${CMAKE_SOURCE_DIR}/src/MakerDialogProperty.c
    ${CMAKE_SOURCE_DIR}/src/MakerDialogUtil.c
    ${CMAKE_SOURCE_DIR}/src/MakerDialogUtil.h
    LIBRARIES ${COMMON_LIBRARIES} ${BACKEND_LIBRARIES}
    )

ADD_SOURCE_TEST(IBusChewingUtil
    SOURCES ${COMMON_SOURCE_FILES} ${BACKEND_SOURCE_FILES}
    ${CMAKE_SOURCE_DIR}/src/MakerDialogUtil.h
    ${CMAKE_SOURCE_DIR}/src/IBusChewingUtil.h
    LIBRARIES ${COMMON_LIBRARIES} ${BACKEND_LIBRARIES} ${IBUS_LIBRARIES} ${X11_LIBRARIES}
    )

ADD_SOURCE_TEST(IBusChewingPreEdit
    SOURCES ${COMMON_SOURCE_FILES} ${BACKEND_SOURCE_FILES}
    ${CMAKE_SOURCE_DIR}/src/IBusChewingUtil.c
    LIBRARIES ${COMMON_LIBRARIES} ${BACKEND_LIBRARIES} ${X11_LIBRARIES}
    )

ADD_SOURCE_TEST(ibus-chewing-engine NO_DEFAULT_SOURCE
    SOURCES ${GUI_CORE_SOURCE_FILES} ${GOB_GENERATED_ibus-chewing-engine}
    ${CMAKE_SOURCE_DIR}/src/IBusChewingLookupTable.c
    ${CMAKE_SOURCE_DIR}/src/IBusChewingLookupTable.h
    ${CMAKE_SOURCE_DIR}/src/IBusChewingPreEdit.c
    ${CMAKE_SOURCE_DIR}/src/IBusChewingPreEdit.h
    ${CMAKE_SOURCE_DIR}/src/IBusChewingSystray.c
    ${CMAKE_SOURCE_DIR}/src/IBusChewingSystray.h
    ${CMAKE_SOURCE_DIR}/src/IBusChewingUtil.c
    ${CMAKE_SOURCE_DIR}/src/IBusChewingUtil.h
    LIBRARIES ${IBUS_ENGINE_CHEWING_LIBRARIES} ${BACKEND_LIBRARIES}
    )


