# - Gettext support: Create/Update pot file and translation files.
# This module creates gettext related targets.
# Most part of this module is from FindGettext.cmake of cmake,
# but it is included here because:
#  1. Some system like RHEL5 does not have FindGettext.cmake
#  2. Bug of GETTEXT_CREATE_TRANSLATIONS make it unable to be include in 'All'
#  3. It does not support xgettext
#
# Defines following variables:
#   GETTEXT_MSGMERGE_EXECUTABLE: the full path to the msgmerge tool.
#   GETTEXT_MSGFMT_EXECUTABLE: the full path to the msgfmt tool.
#   XGETTEXT_EXECUTABLE: the full path to the xgettext.
#   XGETTEXT_OPTIONS_C: Usual xgettext options for C programs.
#
# Defines following macros:
#   GETTEXT_CREATE_POT(potFile
#     [OPTIONS xgettext_options]
#     SRC list_of_source_files
#   )
#   - Generate .pot file.
#     Arguments:
#     + potFile: pot file to be generated.
#     + xgettext_options: (optional) xgettext_options.
#       Default: No options.
#     + list_of_source_files: List of source files that contains msgid.
#     Targets:
#     + pot_file: Generate a pot file with the file name specified in potFile.
#
#   GETTEXT_CREATE_TRANSLATIONS ( potFile [ALL] locale1 ... localeN
#     [COMMENT comment] )
#   - This will create a target "translations" which converts given input po
#     files into the binary output mo files. If the ALL option is used, the
#     translations will also be created when building with "make all"
#     Arguments:
#     + potFile: pot file to be referred.
#     + ALL: (Optional) target "translations" is included when building with
#       "make all"
#     + locale1 ... localeN: locale to be built.
#     + comment: (Optional) Comment for target "translations".
#     Targets:
#     + translations: Converts input po files into the binary output mo files.
#

IF(NOT DEFINED _USE_GETTEXT_CMAKE_)
    SET(_USE_GETTEXT_CMAKE_ "DEFINED")
    FIND_PROGRAM(XGETTEXT_EXECUTABLE xgettext)
    IF(XGETTEXT_EXECUTABLE STREQUAL "XGETTEXT_EXECUTABLE-NOTFOUND")
	MESSAGE(FATAL_ERROR "xgettext not found!")
    ENDIF(XGETTEXT_EXECUTABLE STREQUAL "XGETTEXT_EXECUTABLE-NOTFOUND")

    FIND_PROGRAM(GETTEXT_MSGMERGE_EXECUTABLE msgmerge)
    IF(GETTEXT_MSGMERGE_EXECUTABLE STREQUAL "GETTEXT_MSGMERGE_EXECUTABLE-NOTFOUND")
	MESSAGE(FATAL_ERROR "msgmerge not found!")
    ENDIF(GETTEXT_MSGMERGE_EXECUTABLE STREQUAL "GETTEXT_MSGMERGE_EXECUTABLE-NOTFOUND")

    FIND_PROGRAM(GETTEXT_MSGFMT_EXECUTABLE msgfmt)
    IF(GETTEXT_MSGFMT_EXECUTABLE STREQUAL "GETTEXT_MSGFMT_EXECUTABLE-NOTFOUND")
	MESSAGE(FATAL_ERROR "msgfmt not found!")
    ENDIF(GETTEXT_MSGFMT_EXECUTABLE STREQUAL "GETTEXT_MSGFMT_EXECUTABLE-NOTFOUND")

    SET(XGETTEXT_OPTIONS_C
	--language=C --keyword=_ --keyword=N_ --keyword=C_:1c,2 --keyword=NC_:1c,2 -s
	--package-name=${PROJECT_NAME} --package-version=${PRJ_VER})

    MACRO(GETTEXT_CREATE_POT potFile )
	SET(_xgettext_options_list)
	SET(_src_list)
	SET(_src_list_abs)
	SET(_stage "SRC")
	FOREACH(_pot_option ${ARGN})
	    IF(_pot_option STREQUAL "OPTIONS")
		SET(_stage "OPTIONS")
	    ELSEIF(_pot_option STREQUAL "SRC")
		SET(_stage "SRC")
	    ELSE(_pot_option STREQUAL "OPTIONS")
		IF(_stage STREQUAL "OPTIONS")
		    SET(_xgettext_options_list ${_xgettext_options_list} ${_pot_option})
		ELSE(_stage STREQUAL "OPTIONS")
		    FILE(RELATIVE_PATH _relFile ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${_pot_option})
		    GET_FILENAME_COMPONENT(_absFile ${_pot_option} ABSOLUTE)
		    SET(_src_list ${_src_list} ${_relFile})
		    SET(_src_list_abs ${_src_list_abs} ${_absFile})
		ENDIF(_stage STREQUAL "OPTIONS")
	    ENDIF(_pot_option STREQUAL "OPTIONS")
	ENDFOREACH(_pot_option ${_pot_options} ${ARGN})

	IF (_xgettext_options_list)
	    SET(_xgettext_options ${_xgettext_options_list})
	ELSE(_xgettext_options_list)
	    SET(_xgettext_options ${XGETTEXT_OPTIONS})
	ENDIF(_xgettext_options_list)

	#MESSAGE("${XGETTEXT_EXECUTABLE} ${_xgettext_options_list} -o ${potFile} ${_src_list}")
	ADD_CUSTOM_COMMAND(OUTPUT ${potFile}
	    COMMAND ${XGETTEXT_EXECUTABLE} ${_xgettext_options_list} -o ${potFile} ${_src_list}
	    DEPENDS ${_src_list_abs}
	    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	    )

	ADD_CUSTOM_TARGET(pot_file
	    COMMAND ${XGETTEXT_EXECUTABLE} ${_xgettext_options_list} -o ${potFile} ${_src_list}
	    DEPENDS ${potFile}
	    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	    COMMENT "Extract translatable messages to ${potFile}"
	    )
    ENDMACRO(GETTEXT_CREATE_POT potFile _pot_options)


    MACRO(GETTEXT_CREATE_TRANSLATIONS potFile _firstLang)
	SET(_gmoFiles)
	GET_FILENAME_COMPONENT(_potBasename ${potFile} NAME_WE)
	GET_FILENAME_COMPONENT(_absPotFile ${potFile} ABSOLUTE)

	SET(_addToAll)
	SET(_is_comment FALSE)

	FOREACH (_currentLang ${_firstLang} ${ARGN})
	    IF(_currentLang STREQUAL "ALL")
		SET(_addToAll "ALL")
	    ELSEIF(_currentLang STREQUAL "COMMENT")
		SET(_is_comment TRUE)
	    ELSEIF(_is_comment)
		SET(_is_comment FALSE)
		SET(_comment ${_currentLang})
	    ELSE(_currentLang STREQUAL "ALL")
		SET(_lang ${_currentLang})
		GET_FILENAME_COMPONENT(_absFile ${_currentLang}.po ABSOLUTE)
		GET_FILENAME_COMPONENT(_abs_PATH ${_absFile} PATH)
		SET(_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo)

		#MESSAGE("_absFile=${_absFile} _abs_PATH=${_abs_PATH} _lang=${_lang} curr_bin=${CMAKE_CURRENT_BINARY_DIR}")
		ADD_CUSTOM_COMMAND(
		    OUTPUT ${_gmoFile}
		    COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} --quiet --update --backup=none -s ${_absFile} ${_absPotFile}
		    COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_gmoFile} ${_absFile}
		    DEPENDS ${_absPotFile} ${_absFile}
		    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		    COMMENT "Generating ${_currentLang} translation"
		    )

		INSTALL(FILES ${_gmoFile} DESTINATION share/locale/${_lang}/LC_MESSAGES RENAME ${_potBasename}.mo)
		SET(_gmoFiles ${_gmoFiles} ${_gmoFile})
	    ENDIF(_currentLang STREQUAL "ALL")
	ENDFOREACH (_currentLang )

	IF(_comment STREQUAL "")
	    SET(_comment "Generating translations")
	    ADD_CUSTOM_TARGET(translations ${_addToAll} DEPENDS ${_gmoFiles} COMMENT "${_comment}")
	ENDIF(_comment STREQUAL "")
	ADD_CUSTOM_TARGET(translations ${_addToAll} DEPENDS ${_gmoFiles} COMMENT "${_comment}")
    ENDMACRO(GETTEXT_CREATE_TRANSLATIONS potFile _firstLang)

ENDIF(NOT DEFINED _USE_GETTEXT_CMAKE_)

