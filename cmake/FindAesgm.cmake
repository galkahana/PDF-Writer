# Distributed under Apache 2 License.  See accompanying
# file LICENSE or https://github.com/galkahana/PDF-Writer/blob/master/LICENSE
# for details.

#[=======================================================================[.rst:
FindAesgm
-------

Finds the Aesgm library. 
(I guess originally I took the pdfhummus bundled copy from https://github.com/BrianGladman/aes)

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``Aesgm::Aesgm``
  The Aesgm library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``Aesgm_FOUND``
  True if the system has the Aesgm library.
``Aesgm_INCLUDE_DIRS``
  Include directories needed to use Aesgm.
``Aesgm_LIBRARIES``
  Libraries needed to link to Aesgm.
``Aesgm_VERSION``
  The version of the Aesgm library which was found.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``Aesgm_INCLUDE_DIR``
  The directory containing ``aescpp.h``.
``Aesgm_LIBRARY``
  The path to the Aesgm library.

#]=======================================================================]


find_package(PkgConfig)
pkg_check_modules(PC_Aesgm QUIET Aesgm)


find_path(Aesgm_INCLUDE_DIR
  NAMES aescpp.h
  PATHS ${PC_Aesgm_INCLUDE_DIRS} /usr/local/include/aes /usr/include/aes
  PATH_SUFFIXES aes
)
find_library(Aesgm_LIBRARY
  NAMES aesgm
  PATHS ${PC_Aesgm_LIBRARY_DIRS} /usr/local/lib${LIB_SUFFIX} /usr/lib${LIB_SUFFIX}
)

set(Aesgm_VERSION ${PC_Aesgm_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Aesgm
  FOUND_VAR Aesgm_FOUND
  REQUIRED_VARS
    Aesgm_LIBRARY
    Aesgm_INCLUDE_DIR
  VERSION_VAR Aesgm_VERSION
)

if(Aesgm_FOUND)
  set(Aesgm_LIBRARIES ${Aesgm_LIBRARY})
  set(Aesgm_INCLUDE_DIRS ${Aesgm_INCLUDE_DIR})
  set(Aesgm_DEFINITIONS ${PC_Aesgm_CFLAGS_OTHER})
endif()


if(Aesgm_FOUND AND NOT TARGET Aesgm::Aesgm)
  add_library(Aesgm::Aesgm UNKNOWN IMPORTED)
  set_target_properties(Aesgm::Aesgm PROPERTIES
    IMPORTED_LOCATION "${Aesgm_LIBRARY}"
    INTERFACE_COMPILE_OPTIONS "${PC_Aesgm_CFLAGS_OTHER}"
    INTERFACE_INCLUDE_DIRECTORIES "${Aesgm_INCLUDE_DIR}"
  )
endif()


mark_as_advanced(
    Aesgm_INCLUDE_DIR
    Aesgm_LIBRARY
)