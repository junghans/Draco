#-----------------------------*-cmake-*----------------------------------------#
# file   windows-cl.cmake
# author Kelly Thompson <kgt@lanl.gov>
# date   2010 June 5
# brief  Establish flags for Windows - MSVC
# note   Copyright (C) 2010-2014 Los Alamos National Security, LLC.
#        All rights reserved.
#------------------------------------------------------------------------------#
# $Id$
#------------------------------------------------------------------------------#

#
# Sanity Checks
#
if( NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio" AND
    NOT ${CMAKE_GENERATOR} MATCHES  "NMake Makefiles" )
  message( FATAL_ERROR "config/windows-cl.cmake must be taught to build for this compiler (CMAKE_GENERATOR = ${CMAKE_GENERATOR}).  Yell atkt for help on this error." )
endif()

#
# Compiler flag checks
#
include(platform_checks)
query_openmp_availability()

# OpenMP is not available in free MSVC products.
# if( USE_OPENMP )
#     # Platform checks for gethostname()
#     include( CheckIncludeFiles )
#     check_include_files( omp.h HAVE_OMP_H )
#     if( NOT HAVE_OMP_H )
#        set( USE_OPENMP OFF CACHE BOOL "Turn on OpenMP features?" FORCE )
#     endif()
# endif()

# This is required to provide compatibility between MSVC and MinGW generated libraries.
if( DRACO_SHARED_LIBS )
  set( CMAKE_GNUtoMS ON CACHE BOOL "Compatibility flag for MinGW/MSVC." FORCE)
endif()

# Extra setup (ds++/config.h) for MSVC
# 1. Allow M_PI to be found via <cmath>
set( _USE_MATH_DEFINES 1 )

set( MD_or_MT_debug "${MD_or_MT}d" )
if( "${DEBUG_RUNTIME_EXT}" STREQUAL "d" )
  set( MD_or_MT_debug "${MD_or_MT}${DEBUG_RUNTIME_EXT} /RTC1" ) # RTC requires /MDd
endif()

set( numproc $ENV{NUMBER_OF_PROCESSORS} )
if( "${numproc}notfound" STREQUAL "notfound" )
  set( numproc 1 )
endif()

if( NOT CXX_FLAGS_INITIALIZED )
  set( CXX_FLAGS_INITIALIZED "yes" CACHE INTERNAL "using draco settings." )

  set( CMAKE_C_FLAGS "/W2 /Gy /DWIN32 /D_WINDOWS /MP${numproc}" )
  set( CMAKE_C_FLAGS_DEBUG "/${MD_or_MT_debug} /Od /Zi /Ob0 /DDEBUG /D_DEBUG" )
  set( CMAKE_C_FLAGS_RELEASE "/${MD_or_MT} /O2 /Ob2 /DNDEBUG" )
  set( CMAKE_C_FLAGS_MINSIZEREL "/${MD_or_MT} /O1 /Ob1 /DNDEBUG" )
  set( CMAKE_C_FLAGS_RELWITHDEBINFO "/${MD_or_MT} /O2 /Ob2 /Zi /DDEBUG" )

  set( CMAKE_CXX_FLAGS "/W2 /Gy /EHa /DWIN32 /D_WINDOWS /MP${numproc}" )
  set( CMAKE_CXX_FLAGS_DEBUG "/${MD_or_MT_debug} /Od /Zi /Ob0 /DDEBUG /D_DEBUG" )
  set( CMAKE_CXX_FLAGS_RELEASE "/${MD_or_MT} /O2 /Ob2 /DNDEBUG" )
  set( CMAKE_CXX_FLAGS_MINSIZEREL "/${MD_or_MT} /O1 /Ob1 /DNDEBUG" )
  set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "/${MD_or_MT} /O2 /Ob2 /Zi /DDEBUG" )

  # If building static libraries, inlcude debugging information in the
  # library.
  if( ${DRACO_LIBRARY_TYPE} MATCHES "STATIC" )
    set( CMAKE_C_FLAGS_DEBUG   "${CMAKE_C_FLAGS_DEBUG} /Z7"   )
    set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Z7" )
  endif()

  # Suppress some MSVC warnings about "unsafe" pointer use.
  if(MSVC_VERSION GREATER 1399)
    set( CMAKE_C_FLAGS
      "${CMAKE_C_FLAGS} /D_CRT_SECURE_NO_DEPRECATE /D_SCL_SECURE_NO_DEPRECATE /D_SECURE_SCL=0" )
    set( CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} /D_CRT_SECURE_NO_DEPRECATE /D_SCL_SECURE_NO_DEPRECATE /D_SECURE_SCL=0" )
    #set( CMAKE_C_FLAGS_DEBUG   "${CMAKE_C_FLAGS_DEBUG} /D_HAS_ITERATOR_DEBUGGING=0" )
    #set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_HAS_ITERATOR_DEBUGGING=0" )
  endif()

endif()

##---------------------------------------------------------------------------##
# Ensure cache values always match current selection
##---------------------------------------------------------------------------##
set( CMAKE_C_FLAGS                "${CMAKE_C_FLAGS}"                CACHE STRING "compiler flags" FORCE )
set( CMAKE_C_FLAGS_DEBUG          "${CMAKE_C_FLAGS_DEBUG}"          CACHE STRING "compiler flags" FORCE )
set( CMAKE_C_FLAGS_RELEASE        "${CMAKE_C_FLAGS_RELEASE}"        CACHE STRING "compiler flags" FORCE )
set( CMAKE_C_FLAGS_MINSIZEREL     "${CMAKE_C_FLAGS_MINSIZEREL}"     CACHE STRING "compiler flags" FORCE )
set( CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}" CACHE STRING "compiler flags" FORCE )

set( CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS}"                CACHE STRING "compiler flags" FORCE )
set( CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG}"          CACHE STRING "compiler flags" FORCE )
set( CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE}"        CACHE STRING "compiler flags" FORCE )
set( CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_MINSIZEREL}"     CACHE STRING "compiler flags" FORCE )
set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CACHE STRING "compiler flags" FORCE )

#
# Toggle compiler flags for optional features
#
if( NOT "${OpenMP_C_FLAGS}x" STREQUAL "x" )
  toggle_compiler_flag( OPENMP_FOUND ${OpenMP_C_FLAGS} "C;CXX;EXE_LINKER" "" )
endif()

#
# Extra runtime libraries...
#

find_library( Lib_win_winsock
  NAMES wsock32;winsock32;ws2_32
  HINTS
  "C:/Program Files (x86)/Microsoft SDKs/Windows/v7.0A/Lib"
  "C:/Windows/SysWOW64"
  )
if( ${Lib_win_winsock} MATCHES "NOTFOUND" )
  message( FATAL_ERROR "Could not find library winsock32 or ws2_32!" )
endif()

#------------------------------------------------------------------------------#
# End windows-cl.cmake
#------------------------------------------------------------------------------#
