#-----------------------------*-cmake-*----------------------------------------#
# file   config/unix-pgi.cmake
# author Kelly Thompson 
# date   2010 Nov 1
# brief  Establish flags for Linux64 - Intel C++
# note   Copyright � 2010 Los Alamos National Security, LLC.
#        All rights reserved.
#------------------------------------------------------------------------------#
# $Id$
#------------------------------------------------------------------------------#

#
# Sanity Checks
# 

if( BUILD_SHARED_LIBS )
  message( FATAL_ERROR "Feature not available - yell at KT." )
endif( BUILD_SHARED_LIBS )

#
# C++ libraries required by Fortran linker
# 

# execute_process( 
#   COMMAND ${CMAKE_C_COMPILER} -print-libgcc-file-name
#   TIMEOUT 5
#   RESULT_VARIABLE tmp
#   OUTPUT_VARIABLE libgcc_path
#   ERROR_VARIABLE err
#   )
# get_filename_component( libgcc_path ${libgcc_path} PATH )
# execute_process( 
#   COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libstdc++.so
#   TIMEOUT 5
#   RESULT_VARIABLE tmp
#   OUTPUT_VARIABLE libstdcpp_so_loc
#   ERROR_VARIABLE err
#   OUTPUT_STRIP_TRAILING_WHITESPACE
#   )
# get_filename_component( libstdcpp_so_loc ${libstdcpp_so_loc} ABSOLUTE )
# execute_process( 
#   COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libgcc_s.so
#   TIMEOUT 5
#   RESULT_VARIABLE tmp
#   OUTPUT_VARIABLE libgcc_s_so_loc
#   ERROR_VARIABLE err
#   OUTPUT_STRIP_TRAILING_WHITESPACE
#   )
# get_filename_component( libgcc_s_so_loc ${libgcc_s_so_loc} ABSOLUTE )
# set( GCC_LIBRARIES 
#   ${libstdcpp_so_loc}
#   ${libgcc_s_so_loc}
#   )
#message(   "   - GNU C++  : ${libstdcpp_so_loc}" )
#message(   "   -          : ${libgcc_s_so_loc}" )

#
# config.h settings
#

execute_process(
  COMMAND ${CMAKE_C_COMPILER} -V
  OUTPUT_VARIABLE ABS_C_COMPILER_VER
  )
string( REGEX REPLACE "Copyright.*" " " 
  ABS_C_COMPILER_VER ${ABS_C_COMPILER_VER} )
string( STRIP ${ABS_C_COMPILER_VER} ABS_C_COMPILER_VER )

execute_process(
  COMMAND ${CMAKE_CXX_COMPILER} -V
  OUTPUT_VARIABLE ABS_CXX_COMPILER_VER
  )
string( REGEX REPLACE "Copyright.*" " " 
  ABS_CXX_COMPILER_VER ${ABS_CXX_COMPILER_VER} )
string( STRIP ${ABS_CXX_COMPILER_VER} ABS_CXX_COMPILER_VER )

#
# Compiler Flags
# 

# Flags from Draco autoconf build system:
# -Xa
# -A     ansi
# --no_using_std
# --diag_suppress 940
# --diag_suppress 11
# -DNO_PGI_OFFSET
# -Kieee
# --no_implicit_include
# -Mdaz

if( CMAKE_GENERATOR STREQUAL "Unix Makefiles" )
  set( CMAKE_C_FLAGS                "-Kieee -Mdaz " )
  set( CMAKE_C_FLAGS_DEBUG          "-g -O0") # -DDEBUG") 
  set( CMAKE_C_FLAGS_RELEASE        "-O3 -DNDEBUG" )
  set( CMAKE_C_FLAGS_MINSIZEREL     "${CMAKE_C_FLAGS_RELEASE}" )
  set( CMAKE_C_FLAGS_RELWITHDEBINFO "-O3 -DNDEBUG" )

  set( CMAKE_CXX_FLAGS                "${CMAKE_C_FLAGS} -Xa -A --no_using_std --no_implicit_include --diag_suppress 940 --diag_suppress 11 --diag_suppress 450 -DNO_PGI_OFFSET" )
  set( CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_C_FLAGS_DEBUG}")
  set( CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_C_FLAGS_RELEASE}")
  set( CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_RELEASE}")
  set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}" )
ENDIF()

# string( TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_UPPER )

# if( ENABLE_SSE )
#   set( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -msse2 -mfpmath=sse" )
#   set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2 -mfpmath=sse" )
# endif( ENABLE_SSE )

# if( ENABLE_OPENMP )
#   set( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fopenmp" )
#   set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp" )

#   # When compiling F90 that links in C++-based libraries, we will need
#   # librt added to the link line.
#   execute_process( 
#     COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=librt.so
#     TIMEOUT 5
#     RESULT_VARIABLE tmp
#     OUTPUT_VARIABLE librt_so_loc
#     ERROR_VARIABLE err
#     OUTPUT_STRIP_TRAILING_WHITESPACE
#     )
#   get_filename_component( librt_so_loc ${librt_so_loc} ABSOLUTE )
#   set( GCC_LIBRARIES ${GCC_LIBRARIES} ${librt_so_loc} )
# endif()

# option( ENABLE_C_CODECOVERAGE "Instrument for C/C++ code coverage analysis?" OFF )
# if( ENABLE_C_CODECOVERAGE )
#   find_program( COVERAGE_COMMAND gcov )
#   set( CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG} -O0 -fprofile-arcs -ftest-coverage" )
#   set( CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_C_FLAGS_DEBUG}")
#   set( CMAKE_LDFLAGS           "-fprofile-arcs -ftest-coverage" )

#   # When compiling F90 that links in C++-based libraries, we will need
#   # libgcov added to the link line.
#   execute_process( 
#     COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libgcov.a
#     TIMEOUT 5
#     RESULT_VARIABLE tmp
#     OUTPUT_VARIABLE libgcov_a_loc
#     ERROR_VARIABLE err
#     OUTPUT_STRIP_TRAILING_WHITESPACE
#     )
#   get_filename_component( libgcov_a_loc ${libgcov_a_loc} ABSOLUTE )
#   set( GCC_LIBRARIES ${GCC_LIBRARIES} ${libgcov_a_loc} )
# endif( ENABLE_C_CODECOVERAGE )


#------------------------------------------------------------------------------#
# End config/unix-pgi.cmake
#------------------------------------------------------------------------------#
