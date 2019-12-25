# Copyright (C) 2019
# Computer Graphics Group, University of Siegen
# Written by Martin Lambers <martin.lambers@uni-siegen.de>
#
# Copying and distribution of this file, with or without modification, are
# permitted in any medium without royalty provided the copyright notice and this
# notice are preserved. This file is offered as-is, without any warranty.

FIND_PATH(MUPARSER_INCLUDE_DIR NAMES muParser.h)

FIND_LIBRARY(MUPARSER_LIBRARY NAMES muparser)

MARK_AS_ADVANCED(MUPARSER_INCLUDE_DIR MUPARSER_LIBRARY)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MUPARSER
    REQUIRED_VARS MUPARSER_LIBRARY MUPARSER_INCLUDE_DIR
)

IF(MUPARSER_FOUND)
    SET(MUPARSER_LIBRARIES ${MUPARSER_LIBRARY})
    SET(MUPARSER_INCLUDE_DIRS ${MUPARSER_INCLUDE_DIR})
ENDIF()
