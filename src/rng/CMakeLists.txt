# $Id$

cmake_minimum_required(VERSION 2.6)
project( rng CXX )

# ---------------------------------------------------------------------------- #
# Generate config.h (only occurs when cmake is run)
# ---------------------------------------------------------------------------- #

option( ENABLE_RNG_NR "Enable non reproducable random number generator" ON )
if( ENABLE_RNG_NR )
   set( RNG_NR 1 )
endif()
configure_file( config.h.in ${PROJECT_BINARY_DIR}/rng/config.h )

# ---------------------------------------------------------------------------- #
# Source files
# ---------------------------------------------------------------------------- #

# file( GLOB template_implementations *.t.hh *.i.hh )
file( GLOB sources *.cc *.c )
#file( GLOB explicit_instantiations *_pt.cc )
file( GLOB headers *.hh *.h )
#list( REMOVE_ITEM headers ${template_implementations} )

# Make the header files available in the IDE.
if( MSVC_IDE )
   list( APPEND sources ${headers} )
endif()

# ---------------------------------------------------------------------------- #
# Directories to search for include directives
# ---------------------------------------------------------------------------- #

include_directories( 
   ${PROJECT_SOURCE_DIR}      # sources
   ${PROJECT_BINARY_DIR}      # rng/config.h
   ${PROJECT_SOURCE_DIR}/..   # ds++ header files
   ${dsxx_BINARY_DIR}         # ds++/config.h
) 

# ---------------------------------------------------------------------------- #
# Build package library
# ---------------------------------------------------------------------------- #

add_library( Lib_rng ${DRACO_LIBRARY_TYPE} ${sources}  )
if( "${DRACO_LIBRARY_TYPE}" MATCHES "SHARED" )
   set_target_properties( Lib_rng 
      PROPERTIES 
      # Provide compile define macro to enable declspec(dllexport) linkage.
      COMPILE_DEFINITIONS BUILDING_DLL 
      # Use custom library naming
      OUTPUT_NAME rtt_rng
      )
else()
   set_target_properties( Lib_rng 
      PROPERTIES 
      # Use custom library naming
      OUTPUT_NAME rtt_rng
      )
endif()

# ---------------------------------------------------------------------------- #
# Installation instructions
# ---------------------------------------------------------------------------- #

install( TARGETS Lib_rng DESTINATION lib )
install( FILES ${headers} DESTINATION include/rng )

# ---------------------------------------------------------------------------- #
# Unit tests
# ---------------------------------------------------------------------------- #

if( DRACO_BUILD_TESTS )
   add_subdirectory( test )
endif()   