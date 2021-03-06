# Find GRASS
# ~~~~~~~~~~
# Copyright (c) 2007, Martin Dobias <wonder.sk at gmail.com>
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# macro that checks for grass installation in specified directory

MACRO (CHECK_GRASS G_PREFIX)
  #MESSAGE(STATUS "Find GRASS ${GRASS_FIND_VERSION} in ${G_PREFIX}")

  FIND_PATH(GRASS_INCLUDE_DIR${GRASS_CACHE_VERSION} grass/version.h ${G_PREFIX}/include DOC "Path to GRASS ${GRASS_FIND_VERSION} include directory")

  #MESSAGE(STATUS "GRASS_INCLUDE_DIR${GRASS_CACHE_VERSION} = ${GRASS_INCLUDE_DIR${GRASS_CACHE_VERSION}}")

  IF(GRASS_INCLUDE_DIR${GRASS_CACHE_VERSION})
    FILE(READ ${GRASS_INCLUDE_DIR${GRASS_CACHE_VERSION}}/grass/version.h VERSIONFILE)
    # We can avoid the following block using version_less version_equal and
    # version_greater. Are there compatibility problems?
    STRING(REGEX MATCH "[0-9]+\\.[0-9]+\\.[^ ]+" GRASS_VERSION${GRASS_FIND_VERSION} ${VERSIONFILE})
    STRING(REGEX REPLACE "^([0-9]*)\\.[0-9]*\\..*$" "\\1" GRASS_MAJOR_VERSION${GRASS_FIND_VERSION} ${GRASS_VERSION${GRASS_FIND_VERSION}})
    STRING(REGEX REPLACE "^[0-9]*\\.([0-9]*)\\..*$" "\\1" GRASS_MINOR_VERSION${GRASS_FIND_VERSION} ${GRASS_VERSION${GRASS_FIND_VERSION}})
    STRING(REGEX REPLACE "^[0-9]*\\.[0-9]*\\.(.*)$" "\\1" GRASS_MICRO_VERSION${GRASS_FIND_VERSION} ${GRASS_VERSION${GRASS_FIND_VERSION}})
    # Add micro version too?
    # How to numerize RC versions?
    MATH( EXPR GRASS_NUM_VERSION${GRASS_FIND_VERSION} "${GRASS_MAJOR_VERSION${GRASS_FIND_VERSION}}*10000 + ${GRASS_MINOR_VERSION${GRASS_FIND_VERSION}}*100")

    #MESSAGE(STATUS "GRASS_MAJOR_VERSION${GRASS_FIND_VERSION} = ${GRASS_MAJOR_VERSION${GRASS_FIND_VERSION}}")
    IF(GRASS_MAJOR_VERSION${GRASS_FIND_VERSION} EQUAL GRASS_FIND_VERSION)
        SET(GRASS_LIBRARIES_FOUND${GRASS_FIND_VERSION} TRUE)
        SET(GRASS_LIB_NAMES${GRASS_FIND_VERSION} gis dig2 dbmiclient dbmibase shape dgl rtree datetime linkm gproj)
        IF(GRASS_MAJOR_VERSION${GRASS_FIND_VERSION} LESS 7 )
          LIST(APPEND GRASS_LIB_NAMES${GRASS_FIND_VERSION} vect)
          LIST(APPEND GRASS_LIB_NAMES${GRASS_FIND_VERSION} form)
        ELSE(GRASS_MAJOR_VERSION${GRASS_FIND_VERSION} LESS 7 )
          LIST(APPEND GRASS_LIB_NAMES${GRASS_FIND_VERSION} vector)
          LIST(APPEND GRASS_LIB_NAMES${GRASS_FIND_VERSION} raster)
        ENDIF(GRASS_MAJOR_VERSION${GRASS_FIND_VERSION} LESS 7 )

        FOREACH(LIB ${GRASS_LIB_NAMES${GRASS_FIND_VERSION}})
          MARK_AS_ADVANCED ( GRASS_LIBRARY${GRASS_FIND_VERSION}_${LIB} )

          SET(LIB_PATH NOTFOUND)
          # FIND_PATH and FIND_LIBRARY normally search standard locations
          # before the specified paths. To search non-standard paths first,
          # FIND_* is invoked first with specified paths and NO_DEFAULT_PATH
          # and then again with no specified paths to search the default
          # locations. When an earlier FIND_* succeeds, subsequent FIND_*s
          # searching for the same item do nothing. 
          FIND_LIBRARY(LIB_PATH NAMES grass_${LIB} PATHS ${G_PREFIX}/lib NO_DEFAULT_PATH)
          FIND_LIBRARY(LIB_PATH NAMES grass_${LIB} PATHS ${G_PREFIX}/lib)

          IF(LIB_PATH)
            SET(GRASS_LIBRARY${GRASS_FIND_VERSION}_${LIB} ${LIB_PATH})
          ELSE(LIB_PATH)
            SET(GRASS_LIBRARY${GRASS_FIND_VERSION}_${LIB} NOTFOUND)
            SET(GRASS_LIBRARIES_FOUND${GRASS_FIND_VERSION} FALSE)
          ENDIF (LIB_PATH)
        ENDFOREACH(LIB)

        # LIB_PATH is only temporary variable, so hide it (is it possible to delete a variable?)
        UNSET(LIB_PATH CACHE)

        IF(GRASS_LIBRARIES_FOUND${GRASS_FIND_VERSION})
          SET(GRASS_FOUND${GRASS_FIND_VERSION} TRUE)
          SET(GRASS_FOUND TRUE) # GRASS_FOUND is true if at least one version was found
          SET(GRASS_PREFIX${GRASS_CACHE_VERSION} ${G_PREFIX})
          IF(GRASS_FIND_VERSION EQUAL 6)
            # Set also normal variable with number
            SET(GRASS_INCLUDE_DIR${GRASS_FIND_VERSION} ${GRASS_INCLUDE_DIR${GRASS_CACHE_VERSION}})
            SET(GRASS_PREFIX${GRASS_FIND_VERSION} ${G_PREFIX})
          ENDIF(GRASS_FIND_VERSION EQUAL 6)
        ENDIF(GRASS_LIBRARIES_FOUND${GRASS_FIND_VERSION})
    ENDIF(GRASS_MAJOR_VERSION${GRASS_FIND_VERSION} EQUAL GRASS_FIND_VERSION)
  ENDIF(GRASS_INCLUDE_DIR${GRASS_CACHE_VERSION})

  MARK_AS_ADVANCED ( GRASS_INCLUDE_DIR${GRASS_CACHE_VERSION} )
ENDMACRO (CHECK_GRASS)

###################################
# search for grass installations

#MESSAGE(STATUS "GRASS_FIND_VERSION = ${GRASS_FIND_VERSION}")

# list of paths which to search - user's choice as first
SET (GRASS_PATHS ${GRASS_PREFIX${GRASS_CACHE_VERSION}} /usr/lib/grass /opt/grass $ENV{GRASS_PREFIX${GRASS_CACHE_VERSION}})

# os specific paths
IF (WIN32)
  LIST(APPEND GRASS_PATHS c:/msys/local)
ENDIF (WIN32)

IF (UNIX)
  IF (GRASS_FIND_VERSION EQUAL 6)
    LIST(APPEND GRASS_PATHS /usr/lib64/grass64 /usr/lib/grass64)
  ELSEIF (GRASS_FIND_VERSION EQUAL 7)
    LIST(APPEND GRASS_PATHS /usr/lib64/grass70 /usr/lib/grass70 /usr/lib64/grass71 /usr/lib/grass71)
  ENDIF ()
ENDIF (UNIX)

IF (APPLE)
  IF (GRASS_FIND_VERSION EQUAL 6)
    LIST(APPEND GRASS_PATHS
      /Applications/GRASS-6.3.app/Contents/MacOS
      /Applications/GRASS-6.4.app/Contents/MacOS
    )
  ELSEIF (GRASS_FIND_VERSION EQUAL 7)
    LIST(APPEND GRASS_PATHS
      /Applications/GRASS-7.0.app/Contents/MacOS
      /Applications/GRASS-7.1.app/Contents/MacOS
    )
  ENDIF ()
  LIST(APPEND GRASS_PATHS /Applications/GRASS.app/Contents/Resources)
ENDIF (APPLE)

IF (WITH_GRASS${GRASS_CACHE_VERSION})
  FOREACH (G_PREFIX ${GRASS_PATHS})
    IF (NOT GRASS_FOUND${GRASS_FIND_VERSION})
      CHECK_GRASS(${G_PREFIX})
    ENDIF (NOT GRASS_FOUND${GRASS_FIND_VERSION})
  ENDFOREACH (G_PREFIX)
ENDIF (WITH_GRASS${GRASS_CACHE_VERSION})

###################################

IF (GRASS_FOUND${GRASS_FIND_VERSION})
   IF (NOT GRASS_FIND_QUIETLY)
      MESSAGE(STATUS "Found GRASS ${GRASS_FIND_VERSION}: ${GRASS_PREFIX${GRASS_CACHE_VERSION}} (${GRASS_VERSION${GRASS_FIND_VERSION}})")
   ENDIF (NOT GRASS_FIND_QUIETLY)

ELSE (GRASS_FOUND${GRASS_FIND_VERSION})

   IF (WITH_GRASS${GRASS_CACHE_VERSION})

     IF (GRASS_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find GRASS ${GRASS_FIND_VERSION}")
     ELSE (GRASS_FIND_REQUIRED)
        MESSAGE(STATUS "Could not find GRASS ${GRASS_FIND_VERSION}")
     ENDIF (GRASS_FIND_REQUIRED)

   ENDIF (WITH_GRASS${GRASS_CACHE_VERSION})

ENDIF (GRASS_FOUND${GRASS_FIND_VERSION})
