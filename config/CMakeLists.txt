#-----------------------------*-cmake-*----------------------------------------#
# file   config/CMakeLists.txt
# author Kelly Thompson <kgt@lanl.gov>
# date   2010 Nov 2
# brief  Install the config directory in the target directory
# note   Copyright (C) 2010-2016, Los Alamos National Security, LLC.
#        All rights reserved.
#------------------------------------------------------------------------------#
# $Id$
#------------------------------------------------------------------------------#
cmake_minimum_required(VERSION 3.0.0)
project( config )

file( GLOB CMake_src *.cmake )
set( CMake_in
   cmake_uninstall.cmake.in
   doxygen_config.in
   )
set( CMake_AFSD CMakeAddFortranSubdirectory/config_cafs_proj.cmake.in  )

# include(debug_macros)
# echo_all_cmake_variable_values()

string( TOUPPER "${CMAKE_BUILD_TYPE}" upper_build_type )
set( Draco_CXX_COMPILER_FLAGS
   "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${upper_build_type}}" )
set( Draco_C_COMPILER_FLAGS
   "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_${upper_build_type}}" )
set( Draco_Fortran_COMPILER_FLAGS
   "${CMAKE_Fortran_FLAGS} ${CMAKE_Fortran_FLAGS_${upper_build_type}}" )

# sanity checks
# 'src' directory must be processed before the 'config' directory.
if( "${Draco_PACKAGE_LIST}" STREQUAL "" )
   message( FATAL_ERROR
"Something is wrong with the build system.  Draco_PACKAGE_LIST is empty." )
endif()
if( "${Draco_TPL_LIST}" STREQUAL "" )
   message( FATAL_ERROR
"Something is wrong with the build system.  Draco_TPL_LIST is empty." )
endif()
if( "${Draco_LIBRARIES}" STREQUAL "" )
   message( FATAL_ERROR
"Something is wrong with the build system.  Draco_LIBRARIES is empty." )
endif()
if( "${Draco_TPL_LIBRARIES}" STREQUAL "" )
   message( FATAL_ERROR
"Something is wrong with the build system.  Draco_TPL_LIBRARIES is empty." )
endif()

# Publish the active build_types (A subset of CMAKE_CONFIGURATION_TYPES)
if( CMAKE_CONFIGURATION_TYPES )
   unset( DRACO_CONFIGURATION_TYPES CACHE)
   foreach( cfg ${CMAKE_CONFIGURATION_TYPES} )
      if( EXISTS ${Draco_BINARY_DIR}/${cfg} )
         if( "${DRACO_CONFIGURATION_TYPES}x" STREQUAL "x" )
            set( DRACO_CONFIGURATION_TYPES "${cfg}" )
         else()
            list( APPEND DRACO_CONFIGURATION_TYPES ${cfg} )
         endif()
      endif()
   endforeach()
   set( DRACO_CONFIGURATION_TYPES "${DRACO_CONFIGURATION_TYPES}"
      CACHE STRING "Available multiconfig builds." )
endif()

# Process the config file
configure_file( draco-config-install.cmake.in
   ${Draco_BINARY_DIR}/CMakeFiles/draco-config.cmake @ONLY)

# For Knights corner systems, also install the unit test driver
if( HAVE_MIC AND  EXISTS ${Draco_BINARY_DIR}/config/run_test_on_mic.sh )
  install( PROGRAMS ${Draco_BINARY_DIR}/config/run_test_on_mic.sh
    DESTINATION cmake/draco )
endif()

# Install scripts and macros to make them available by other projects.
set( file_list ${CMake_src} ${CMake_in} ${Draco_BINARY_DIR}/CMakeFiles/draco-config.cmake )

install( FILES ${file_list}  DESTINATION cmake/draco )
install( FILES ${CMake_AFSD} DESTINATION cmake/draco/CMakeAddFortranSubdirectory )

##---------------------------------------------------------------------------##
## End of config/CMakeLists.txt
##---------------------------------------------------------------------------##
