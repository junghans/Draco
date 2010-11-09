#-----------------------------*-cmake-*----------------------------------------#
# file   config/unix-intel.cmake
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

if( NOT __LINUX_COMPILER_INTEL )
  message( FATAL_ERROR "If CXX is not Intel C++, then we shouldn't have ended up here.  Something is really wrong with the build system. " )
endif()

if( BUILD_SHARED_LIBS )
  message( FATAL_ERROR "Feature not available - yell at KT." )
endif( BUILD_SHARED_LIBS )

#
# C++ libraries required by Fortran linker
# 

execute_process( 
  COMMAND ${CMAKE_C_COMPILER} -print-libgcc-file-name
  TIMEOUT 5
  RESULT_VARIABLE tmp
  OUTPUT_VARIABLE libgcc_path
  ERROR_VARIABLE err
  )
get_filename_component( libgcc_path ${libgcc_path} PATH )
execute_process( 
  COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libstdc++.so
  TIMEOUT 5
  RESULT_VARIABLE tmp
  OUTPUT_VARIABLE libstdcpp_so_loc
  ERROR_VARIABLE err
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
get_filename_component( libstdcpp_so_loc ${libstdcpp_so_loc} ABSOLUTE )
execute_process( 
  COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libgcc_s.so
  TIMEOUT 5
  RESULT_VARIABLE tmp
  OUTPUT_VARIABLE libgcc_s_so_loc
  ERROR_VARIABLE err
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
get_filename_component( libgcc_s_so_loc ${libgcc_s_so_loc} ABSOLUTE )
set( GCC_LIBRARIES 
  ${libstdcpp_so_loc}
  ${libgcc_s_so_loc}
  )
#message(   "   - GNU C++  : ${libstdcpp_so_loc}" )
#message(   "   -          : ${libgcc_s_so_loc}" )

#
# config.h settings
#

execute_process(
  COMMAND ${CMAKE_C_COMPILER} --version
  OUTPUT_VARIABLE ABS_C_COMPILER_VER
  )
string( REGEX REPLACE "Copyright.*" " " 
  ABS_C_COMPILER_VER ${ABS_C_COMPILER_VER} )
string( STRIP ${ABS_C_COMPILER_VER} ABS_C_COMPILER_VER )

execute_process(
  COMMAND ${CMAKE_CXX_COMPILER} --version
  OUTPUT_VARIABLE ABS_CXX_COMPILER_VER
  )
string( REGEX REPLACE "Copyright.*" " " 
  ABS_CXX_COMPILER_VER ${ABS_CXX_COMPILER_VER} )
string( STRIP ${ABS_CXX_COMPILER_VER} ABS_CXX_COMPILER_VER )


#
# Compiler Flags
# 

# Flags from Draco autoconf build system:
# -ansi -pedantic
# -Wnon-virtual-dtor 
# -Wreturn-type 
# -Wno-long-long
# -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC
# -Wextra
# -Weffc++

IF( CMAKE_GENERATOR STREQUAL "Unix Makefiles" )
  set( CMAKE_C_FLAGS                "-fPIC" )
  set( CMAKE_C_FLAGS_DEBUG          "-g -O0 -inline-level=0 -DDEBUG") 
  set( CMAKE_C_FLAGS_RELEASE        "-O3 -inline-level=1 -DNDEBUG" )
  set( CMAKE_C_FLAGS_MINSIZEREL     "${CMAKE_C_FLAGS_RELEASE}" )
  set( CMAKE_C_FLAGS_RELWITHDEBINFO "-O3 -inline-level=0 -ip -DNDEBUG" )

  set( CMAKE_CXX_FLAGS                "${CMAKE_C_FLAGS}" )
  set( CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_C_FLAGS_DEBUG} -ansi")
  set( CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_C_FLAGS_RELEASE}")
  set( CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_RELEASE}")
  set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}" )
ENDIF()


string( TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_UPPER )
if( ${CMAKE_BUILD_TYPE_UPPER} MATCHES "DEBUG" )
   option( GCC_ENABLE_ALL_WARNINGS 
      "Add \"-Weffc++\" to the compile options (only available for DEBUG builds)." OFF )
   option( GCC_ENABLE_GLIBCXX_DEBUG "Use special version of libc.so that
   includes STL bounds checking (only available for DEBUG builds)." OFF )
   if( GCC_ENABLE_ALL_WARNINGS )
      set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Weffc++" )
   endif()
   if( GCC_ENABLE_GLIBCXX_DEBUG )
      set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC" )
   endif()
endif()

if( ENABLE_SSE )
  set( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -msse2 -mfpmath=sse" )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2 -mfpmath=sse" )
endif( ENABLE_SSE )

if( ENABLE_OPENMP )
  set( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fopenmp" )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp" )

  # When compiling F90 that links in C++-based libraries, we will need
  # librt added to the link line.
  execute_process( 
    COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=librt.so
    TIMEOUT 5
    RESULT_VARIABLE tmp
    OUTPUT_VARIABLE librt_so_loc
    ERROR_VARIABLE err
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  get_filename_component( librt_so_loc ${librt_so_loc} ABSOLUTE )
  set( GCC_LIBRARIES ${GCC_LIBRARIES} ${librt_so_loc} )
endif()

option( ENABLE_C_CODECOVERAGE "Instrument for C/C++ code coverage analysis?" OFF )
if( ENABLE_C_CODECOVERAGE )
  find_program( COVERAGE_COMMAND gcov )
  set( CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG} -O0 -fprofile-arcs -ftest-coverage" )
  set( CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_C_FLAGS_DEBUG}")
  set( CMAKE_LDFLAGS           "-fprofile-arcs -ftest-coverage" )

  # When compiling F90 that links in C++-based libraries, we will need
  # libgcov added to the link line.
  execute_process( 
    COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libgcov.a
    TIMEOUT 5
    RESULT_VARIABLE tmp
    OUTPUT_VARIABLE libgcov_a_loc
    ERROR_VARIABLE err
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  get_filename_component( libgcov_a_loc ${libgcov_a_loc} ABSOLUTE )
  set( GCC_LIBRARIES ${GCC_LIBRARIES} ${libgcov_a_loc} )
endif( ENABLE_C_CODECOVERAGE )

#------------------------------------------------------------------------------#
# End config/unix-g++.cmake
#------------------------------------------------------------------------------#