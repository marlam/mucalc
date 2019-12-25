# Copyright (C) 2019
# Computer Graphics Group, University of Siegen
# Written by Martin Lambers <martin.lambers@uni-siegen.de>
#
# Copying and distribution of this file, with or without modification, are
# permitted in any medium without royalty provided the copyright notice and this
# notice are preserved. This file is offered as-is, without any warranty.

FIND_PATH(READLINE_INCLUDE_DIR NAMES readline/readline.h)

FIND_LIBRARY(READLINE_LIBRARY NAMES readline)

MARK_AS_ADVANCED(READLINE_INCLUDE_DIR READLINE_LIBRARY)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(READLINE
    REQUIRED_VARS READLINE_LIBRARY READLINE_INCLUDE_DIR
)

IF(READLINE_FOUND)
    SET(READLINE_LIBRARIES ${READLINE_LIBRARY})
    SET(READLINE_INCLUDE_DIRS ${READLINE_INCLUDE_DIR})
ENDIF()
