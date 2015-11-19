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

#===========================================
#  MakerDialogUtil
#

SET(MAKER_DIALOG_UTIL MakerDialogUtil)
ADD_EXECUTABLE(${MAKER_DIALOG_UTIL}-test
    ${CMAKE_CURRENT_SOURCE_DIR}/${MAKER_DIALOG_UTIL}-test.c
    ${CMAKE_SOURCE_DIR}/src/${MAKER_DIALOG_UTIL}.c
    )

TARGET_LINK_LIBRARIES(${MAKER_DIALOG_UTIL}-test
    ${GLIB2_LIBRARIES}
    )

ADD_TEST("${MAKER_DIALOG_UTIL}"
    gtester ${CMAKE_CURRENT_BINARY_DIR}/${MAKER_DIALOG_UTIL}-test
    )

#===========================================
#  MakerDialogBackend
#

SET(MAKER_DIALOG_BACKEND MakerDialogBackend)
ADD_EXECUTABLE(${MAKER_DIALOG_BACKEND}-test
    ${COMMON_SOURCE_FILES} ${BACKEND_SOURCE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/${MAKER_DIALOG_BACKEND}-test.c
    ${CMAKE_SOURCE_DIR}/src/MakerDialogBackend.c
    ${CMAKE_SOURCE_DIR}/src/MakerDialogProperty.c
    ${CMAKE_SOURCE_DIR}/src/MakerDialogUtil.c
    )

TARGET_LINK_LIBRARIES(${MAKER_DIALOG_BACKEND}-test
    ${COMMON_LIBRARIES}
    ${BACKEND_LIBRARIES}
    )

ADD_TEST("${MAKER_DIALOG_BACKEND}"
    gtester ${CMAKE_CURRENT_BINARY_DIR}/${MAKER_DIALOG_BACKEND}-test
    )

#===========================================
#  IBusChewingPreEdit
#

SET(IBUS_CHEWING_PRE_EDIT IBusChewingPreEdit)


ADD_EXECUTABLE(${IBUS_CHEWING_PRE_EDIT}-test
    ${COMMON_SOURCE_FILES} ${BACKEND_SOURCE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/${IBUS_CHEWING_PRE_EDIT}-test.c
    ${CMAKE_SOURCE_DIR}/src/IBusChewingUtil.c
    ${CMAKE_SOURCE_DIR}/src/${IBUS_CHEWING_PRE_EDIT}.c
    )

TARGET_LINK_LIBRARIES(${IBUS_CHEWING_PRE_EDIT}-test
    ${COMMON_LIBRARIES}
    ${X11_LIBRARIES}
    ${BACKEND_LIBRARIES}
    )

ADD_TEST("${IBUS_CHEWING_PRE_EDIT}"
    gtester ${CMAKE_CURRENT_BINARY_DIR}/${IBUS_CHEWING_PRE_EDIT}-test
    )


#===========================================
#  IBusChewingEngine
#

SET(IBUS_ENGINE_CHEWING ibus-chewing-engine)

ADD_EXECUTABLE(${IBUS_ENGINE_CHEWING}-test
    ${GUI_CORE_SOURCE_FILES}
    ${GOB_GENERATED_ibus-chewing-engine}
    ${CMAKE_SOURCE_DIR}/src/IBusChewingLookupTable.c
    ${CMAKE_SOURCE_DIR}/src/IBusChewingLookupTable.h
    ${CMAKE_SOURCE_DIR}/src/IBusChewingPreEdit.c
    ${CMAKE_SOURCE_DIR}/src/IBusChewingPreEdit.h
    ${CMAKE_SOURCE_DIR}/src/IBusChewingUtil.c
    ${CMAKE_SOURCE_DIR}/src/IBusChewingUtil.h
    ${CMAKE_CURRENT_SOURCE_DIR}/${IBUS_ENGINE_CHEWING}-test.c
    )

TARGET_LINK_LIBRARIES(${IBUS_ENGINE_CHEWING}-test
    ${IBUS_ENGINE_CHEWING_LIBRARIES}
    ${BACKEND_LIBRARIES}
    )

ADD_TEST("${IBUS_ENGINE_CHEWING}"
    gtester ${CMAKE_CURRENT_BINARY_DIR}/${IBUS_ENGINE_CHEWING}-test
    )


