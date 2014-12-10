# - Try to find musicbrainz5
# Once done this will define
#  MUSICBRAINZ5_FOUND - System has libmusicbrainz5
#  MUSICBRAINZ5_INCLUDE_DIRS - The libmusicbrainz5 include directories
#  MUSICBRAINZ5_LIBRARIES - The libraries needed to use libmusicbrainz5
#  MUSICBRAINZ5_DEFINITIONS - Compiler switches required

find_package(PkgConfig QUIET)
pkg_check_modules(PC_MUSICBRAINZ5 QUIET libmusicbrainz5)
set(MUSICBRAINZ5_DEFINITIONS ${PC_MUSICBRAINZ5_CFLAGS_OTHER})

find_path(MUSICBRAINZ5_INCLUDE_DIR musicbrainz5/mb5_c.h
          HINTS ${PC_MUSICBRAINZ5_INCLUDEDIR} ${PC_MUSICBRAINZ5_INCLUDE_DIRS}
          PATH_SUFFIXES libmusicbrainz5)

find_library(MUSICBRAINZ5_LIBRARY NAMES musicbrainz5 libmusicbrainz5
             HINTS ${PC_MUSICBRAINZ5_LIBDIR} ${PC_MUSICBRAINZ5_LIBRARY_DIRS} )

set(MUSICBRAINZ5_LIBRARIES ${MUSICBRAINZ5_LIBRARY} )
set(MUSICBRAINZ5_INCLUDE_DIRS ${MUSICBRAINZ5_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set MUSICBRAINZ5_FOUND
find_package_handle_standard_args(Musicbrainz5  DEFAULT_MSG
        MUSICBRAINZ5_LIBRARY MUSICBRAINZ5_INCLUDE_DIR)

mark_as_advanced(MUSICBRAINZ5_INCLUDE_DIR MUSICBRAINZ5_LIBRARY )
