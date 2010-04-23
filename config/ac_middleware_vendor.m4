dnl-------------------------------------------------------------------------dnl
dnl ac_middleware_vendor.m4
dnl
dnl Macros for setting up a middleware vendor that uses the draco
dnl build system
dnl
dnl Thomas M. Evans and Bob Clark
dnl 2004/01/20 08:03:00
dnl-------------------------------------------------------------------------dnl
##---------------------------------------------------------------------------##
## $Id$
##---------------------------------------------------------------------------##

dnl-------------------------------------------------------------------------dnl
dnl AC_MIDDLEWARE_VENDOR_SETUP(1)
dnl
dnl Setup macro that allows a middleware vendor using the draco build
dnl system to be defined as a vendor. This should be called in a
dnl middleware-client configure.ac as follows:
dnl
dnl AC_MIDDLEWARE_VENDOR_SETUP(vendor_name)
dnl
dnl AC_NEEDS_LIBS(pkg_components)
dnl AC_NEEDS_LIBS_MIDDLEWARE(vendor_name, pkg1 pkg2)
dnl
dnl That is the library dependency ordering is important.  A link line
dnl will be created -lpkg_components -lpkg1 -lpkg2. The vendor_name sets
dnl up the appropriate path data as described below.
dnl                 
dnl Three options are defined that tell configure where the vendor
dnl includes and libraries are located:
dnl
dnl  --with-vendor_name
dnl  --with-vendor_name-lib
dnl  --with-vendor_name-inc
dnl
dnl Where vendor_name is the package name (ie. pika, components, etc.).
dnl
dnl Setting --with-vendor_name=<dir> tells configure to look in
dnl <dir>/lib, <dir>/inc, <dir>/libexec for vendor_name stuff.  The
dnl include and/or lib directories can be overridden by setting
dnl --with-vendor_name-inc or --with-vendor_name-lib.  If
dnl --with-vendor_name is set, without arguments, then --prefix is used
dnl as the vendor_name directory.  The default location (if
dnl --with-vendor_name is not called at all) is /usr/local.  Of course,
dnl this can be overwritten for the lib and include directories by using
dnl --with-vendor_name-lib and/or --with-vendor_name-inc (libexec is
dnl found in the directory set by --with-vendor_name-lib (in libexec
dnl instead of lib)).
dnl
dnl A final option is:
dnl
dnl  --with-vendor_name-src
dnl
dnl This is provided for convenience.  It can be used by clients to
dnl specify where the vendor_name source directory vendor_name/ is
dnl located.  It is NOT used in this function in any way.  It is only
dnl provided so that clients can use it to determine the location of the
dnl vendor_name source directory in their own configure/build systems.
dnl
dnl The AC_MIDDLEWARE_VENDOR_SETUP does not set the libexecdir
dnl variable.  This variable is reserved for tools installed by 
dnl draco.  Middleware vendors should not have their own libexecdir,
dnl but instead, they should only add includes and libraries.
dnl-------------------------------------------------------------------------dnl

AC_DEFUN([AC_MIDDLEWARE_VENDOR_SETUP], [dnl
   
   dnl middleware vendor_name in $1

   dnl define --with-vendor
   AC_ARG_WITH($1,
      [  --with-$1           give head location of $1 includes and libs (default is /usr/local; no argmument sets to prefix)])

   AC_ARG_WITH($1-inc,
      [  --with-$1-inc       give location of $1 includes])

   AC_ARG_WITH($1-lib,
      [  --with-$1-lib       give location of $1 libs])

   AC_ARG_WITH($1-src,
      [  --with-$1-src       give location (prefix) of $1 source directory])

   # middleware include and lib dirs
   MIDDLEWARE_INC_$1=''
   MIDDLEWARE_LIB_$1=''
   middleware_in_prefix_$1=''

   AC_MSG_CHECKING("$1 directories")

   # if we use the with-vendor option to specify the location of vendor,
   # then check it
   if test -n "${with_$1}" ; then 

       # check to see if we should set vendor to the prefix (turned on
       # by --with-vendor without argument)
       if test "${with_$1}" = yes ; then
      
           # set MIDDLEWARE_INC_$1 and MIDDLEWARE_LIB_$1
           MIDDLEWARE_INC_$1="${prefix}/include"
           MIDDLEWARE_LIB_$1="${prefix}/lib"

           middleware_in_prefix_$1='true'

       # otherwise it is the directory where the installed lib and 
       # include are; check them
       else
      
           # set MIDDLEWARE_INC_$1 and MIDDLEWARE_LIB_$1
           MIDDLEWARE_LIB_$1="${with_$1}/lib"   
           MIDDLEWARE_INC_$1="${with_$1}/include"

       fi

   # set draco default location to /usr/local
   else

       MIDDLEWARE_INC_$1="/usr/local/include"
       MIDDLEWARE_LIB_$1="/usr/local/lib"

   fi
     
   # if --with_vendor_inc is defined then set and check MIDDLEWARE_INC_$1
   if test -n "${with_$1_inc}" ; then
       MIDDLEWARE_INC_$1="${with_$1_inc}"
   fi

   # if --with_vendor_lib is defined then set and check MIDDLEWARE_LIB_$1
   if test -n "${with_$1_lib}" ; then
       MIDDLEWARE_LIB_$1="${with_$1_lib}"
   fi

   # make sure they exist   
   if test ! -d "${MIDDLEWARE_INC_$1}" && test -z "${middleware_in_prefix_$1}" ; then
       AC_MSG_ERROR("${MIDDLEWARE_INC_$1} does not exist")
   fi

   if test ! -d "${MIDDLEWARE_LIB_$1}" && test -z "${middleware_in_prefix_$1}" ; then
       AC_MSG_ERROR("${MIDDLEWARE_LIB_$1} does not exist")
   fi


   AC_MSG_RESULT("${MIDDLEWARE_INC_$1} and ${MIDDLEWARE_LIB_$1} set")

   # add vendor include directory to VENDOR_INC
   if test -z "${middlware_in_prefix_$1}" ; then
       VENDOR_INC="${VENDOR_INC} -I${MIDDLEWARE_INC_$1}"
   fi

   # add draco to VENDIR_DIRS
   VENDOR_LIB_DIRS="${MIDDLEWARE_LIB_$1} ${VENDOR_LIB_DIRS}"
   VENDOR_INC_DIRS="${MIDDLEWARE_INC_$1} ${VENDOR_INC_DIRS}"
])

dnl-------------------------------------------------------------------------dnl
dnl AC_NEEDS_LIBS_MIDDLEWARE
dnl
dnl add MIDDLEWARE vendor libraries necessary for a package
dnl this MACRO must be called after AC_NEEDS_LIBS
dnl usage: configure.ac
dnl-------------------------------------------------------------------------dnl

AC_DEFUN([AC_NEEDS_LIBS_MIDDLEWARE], [dnl

   dnl arg 1 is the middleware vendor name
   dnl arg 2 are the libraries (packages)

   if test ${has_$1_libdir:=no} != "yes" ; then
       
       has_$1_libdir="yes"

       if test "${has_libdir}" != "yes" ||
          test "${middleware_in_prefix_$1}" != "true" ; then
          DRACO_LIBS="${DRACO_LIBS} -L${MIDDLEWARE_LIB_$1}"
       fi

   fi

   for lib in $2
   do
       # temporary string to keep line from getting too long
       middleware_depends="${MIDDLEWARE_LIB_$1}/lib\${LIB_PREFIX}${lib}\${libsuffix}"
       DRACO_DEPENDS="${DRACO_DEPENDS} ${middleware__depends}"
       DRACO_LIBS="${DRACO_LIBS} -l\${LIB_PREFIX}${lib}"
   done
])

dnl-------------------------------------------------------------------------dnl
dnl AC_NEEDS_LIBS_TEST_MIDDLEWARE
dnl
dnl add MIDDLEWARE libraries necessary for testing a package
dnl this MACRO must be called after AC_NEEDS_LIBS_TEST and
dnl AC_NEEDS_LIBS_MIDDLEWARE 
dnl usage: configure.ac
dnl-------------------------------------------------------------------------dnl

AC_DEFUN([AC_NEEDS_LIBS_TEST_MIDDLEWARE], [dnl

   dnl $1 is the middleware vendor name
   dnl $2 are the libraries (packages)

   DRACO_TEST_LIBS="${DRACO_TEST_LIBS} -L${MIDDLEWARE_LIB_$1}"

   for lib in $2
   do
       # temporary string to keep line from getting too long
       middleware_test_depends="${MIDDLEWARE_LIB_$1}/lib\${LIB_PREFIX}${lib}\${libsuffix}"
       DRACO_TEST_DEPENDS="${DRACO_TEST_DEPENDS} ${middleware_test_depends}"
       DRACO_TEST_LIBS="${DRACO_TEST_LIBS} -l\${LIB_PREFIX}${lib}"
   done
])

dnl-------------------------------------------------------------------------dnl
dnl end of ac_middleware_vendor.m4
dnl-------------------------------------------------------------------------dnl
