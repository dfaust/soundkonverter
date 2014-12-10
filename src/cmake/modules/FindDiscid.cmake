# - Try to find discid
# Once done this will define
#  DISCID_FOUND - System has libdiscid
#  DISCID_INCLUDE_DIRS - The libdiscid include directories
#  DISCID_LIBRARIES - The libraries needed to use libdiscid
#  DISCID_DEFINITIONS - Compiler switches required

find_package(PkgConfig QUIET)
pkg_check_modules(PC_DISCID QUIET libdiscid)
set(DISCID_DEFINITIONS ${PC_DISCID_CFLAGS_OTHER})

find_path(DISCID_INCLUDE_DIR discid/discid.h
          HINTS ${PC_DISCID_INCLUDEDIR} ${PC_DISCID_INCLUDE_DIRS}
          PATH_SUFFIXES libdiscid)

find_library(DISCID_LIBRARY NAMES discid libdiscid
             HINTS ${PC_DISCID_LIBDIR} ${PC_DISCID_LIBRARY_DIRS} )

set(DISCID_LIBRARIES ${DISCID_LIBRARY} )
set(DISCID_INCLUDE_DIRS ${DISCID_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set DISCID_FOUND
find_package_handle_standard_args(Discid  DEFAULT_MSG
        DISCID_LIBRARY DISCID_INCLUDE_DIR)

mark_as_advanced(DISCID_INCLUDE_DIR DISCID_LIBRARY )
