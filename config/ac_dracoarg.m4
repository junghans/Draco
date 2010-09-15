dnl-------------------------------------------------------------------------dnl
dnl ac_dracoarg.m4
dnl
dnl Declarations of Draco configure options (with some default
dnl settings). 
dnl
dnl Thomas M. Evans
dnl 1999/02/04 01:56:20
dnl-------------------------------------------------------------------------dnl
##---------------------------------------------------------------------------##
## $Id$
##---------------------------------------------------------------------------##

dnl-------------------------------------------------------------------------dnl
dnl AC_DRACO_ARGS
dnl
dnl Declaration of Draco non-vendor configure options. This macro can 
dnl be called to fill out configure help screens
dnl-------------------------------------------------------------------------dnl

AC_DEFUN([AC_DRACO_ARGS], [dnl

   dnl
   dnl Library prefix
   dnl
     
   AC_ARG_WITH(lib-prefix,
      [  --with-lib-prefix[=library prefix]
                          give prefix to libraries (default rtt_)])

   # default for lib_prefix is rtt_
   LIB_PREFIX="${with_lib_prefix:=rtt_}"
   if test "${LIB_PREFIX}" = no ; then
       LIB_PREFIX=''
   fi

   dnl
   dnl c4 toggle (scalar by default)
   dnl

   dnl define --with-c4
   AC_ARG_WITH([c4],
     [AS_HELP_STRING([--with-c4@<:@=@<:@scalar|mpi@:>@@:>@], dnl shmem
       [turn on c4 (default mpi)])],
     [
        dnl If the user provides a --with-c4 command for configure,
        dnl ensure that a valid value has been provided
        if test ${with_c4:=mpi} = yes; then
           with_c4=mpi
        elif ! test ${with_c4} = scalar && 
             ! test ${with_c4} = mpi    &&
             ! test ${with_c4} = no; then
           { echo "configure: error: --with-c4 must be one of [yes|no|mpi|scalar]." 1>&2; \
           exit 1; }
        fi
     ],
     [
        dnl If user does not provide a --with-c4 command for
        dnl configure, default to mpi.
        if test "${with_mpi:-no}" = "no"; then
           with_c4=scalar
        else
           with_c4=mpi
        fi
     ]
   )
   AC_MSG_CHECKING([for c4 configuration])
   AC_MSG_RESULT([${with_c4}])

   dnl
   dnl DBC toggle
   dnl

   dnl defines --with-dbc
   AC_ARG_WITH([dbc],
     [AS_HELP_STRING([--with-dbc@<:@=@<:@0-7@:>@@:>@],[set Design-by-Contract
level. 0 is off; +1 turns on Require; +2 turns on Check; +4 turns on Ensure.])])
	
   if test "${with_dbc}" = yes ; then
       with_dbc='7'
   elif test "${with_dbc}" = no ; then
       with_dbc='0'
   fi
	
   dnl
   dnl SHARED versus ARCHIVE libraries
   dnl

   dnl defines --enable-shared
   AC_ARG_ENABLE(shared,
      [  --enable-shared         turn on shared libraries (.a default)])

   # do shared specific stuff
   if test "${enable_shared}" = yes ; then
      RANLIB=':'
   fi

   dnl
   dnl CHOOSE A C++ COMPILER
   dnl

   dnl defines --with-cxx
   AC_ARG_WITH([cxx],
     [AS_HELP_STRING([--with-cxx@<:@=gcc|icpc|sgi|kcc|compaq|guide@:>@],                                    
       [choose a c++ compiler (defaults are machine dependent)])])

   dnl the default is gcc
   if test "${with_cxx}" = yes ; then
       with_cxx='gcc'
   fi

   dnl
   dnl STATIC VERSUS DYNAMIC LINKING
   dnl

   dnl defines --enable-static-ld
   AC_ARG_ENABLE(static-ld,
      [  --enable-static-ld      use (.a) libraries if possible])

   dnl
   dnl ANSI STRICT COMPLIANCE
   dnl

   dnl defines --enable-strict-ansi
   AC_ARG_ENABLE(strict-ansi,
      [  --disable-strict-ansi   turn off strict ansi compliance])

   dnl
   dnl ONE_PER INSTANTIATION FLAG
   dnl

   dnl defines --enable-one-per
   AC_ARG_ENABLE(one-per,
      [  --disable-one-per       turn off --one_per flag])

   dnl
   dnl COMPILER OPTIMZATION LEVEL
   dnl

   dnl defines --with-opt
   AC_ARG_WITH(opt,
      [  --with-opt[=0,1,2,3]      set optimization level (0 by default)])

   if test "${with_opt}" = yes ; then
       with_opt='0'
   fi

   dnl defines --enable-debug
   AC_ARG_ENABLE(debug,
      [  --enable-debug          turn on debug (-g) option])

   dnl
   dnl POSIX SOURCE
   dnl

   dnl defines --with-posix
   AC_ARG_WITH(posix,
      [  --with-posix[=num]        give posix source (system-dependent defaults)])

   dnl
   dnl ADD TO CPPFLAGS
   dnl
   
   dnl defines --with-cppflags
   AC_ARG_WITH(cppflags,
      [  --with-cppflags@<:@=flags@:>@ add flags to @S|@CPPFLAGS])

   dnl
   dnl ADD TO CXXFLAGS
   dnl
   
   dnl defines --with-cxxflags
   AC_ARG_WITH(cxxflags,
      [  --with-cxxflags@<:@=flags@:>@ add flags to @S|@CXXFLAGS])

   dnl
   dnl ADD TO CFLAGS
   dnl
   
   dnl defines --with-cflags
   AC_ARG_WITH(cflags,
      [  --with-cflags@<:@=flags@:>@   add flags to @S|@CFLAGS])

   dnl
   dnl ADD TO F90FLAGS
   dnl
   
   dnl defines --with-f90flags
   AC_ARG_WITH(f90flags,
      [  --with-f90flags@<:@=flags@:>@ add flags to @S|@F90FLAGS])

   dnl
   dnl ADD TO ARFLAGS
   dnl
   
   dnl defines --with-arflags
   AC_ARG_WITH(arflags,
      [  --with-arflags@<:@=flags@:>@  add flags to @S|@ARFLAGS])

   dnl
   dnl ADD TO LDFLAGS
   dnl
   
   dnl defines --with-ldflags
   AC_ARG_WITH(ldflags,
      [  --with-ldflags@<:@=flags@:>@  add flags to @S|@LDFLAGS])

   dnl 
   dnl ADD TO LIBRARIES
   dnl

   dnl defines --with-libs
   AC_ARG_WITH(libs,
      [  --with-libs=[libs]        add libs to @S|@LIBS])

   dnl
   dnl CHOSE BIT COMPILATION ON SGI'S
   dnl

   dnl defines --enable-32-bit
   AC_ARG_ENABLE(32-bit,
      [  --enable-32-bit         do 32-bit compilation (compiler dependent)])

   dnl defines --enable-64-bit
   AC_ARG_ENABLE(64-bit,
      [  --enable-64-bit         do 64-bit compilation (compiler dependent)])

   dnl
   dnl CHOSE MIPS INSTRUCTION SET ON SGI'S
   dnl

   dnl defines --with-mips
dnl    AC_ARG_WITH(mips,
dnl       [  --with-mips[=1,2,3,4]   set mips, mips4 by default (SGI ONLY)])

dnl    if test "${with_mips}" = yes ; then
dnl        with_mips='4'
dnl    fi

   dnl 
   dnl Arguments for options defined in ac_instrument.m4
   dnl
   
   AC_DRACO_INSTR_ARGS

   dnl
   dnl Doxygen options
   dnl

   AC_ARG_ENABLE(latex-doc,
      [  --enable-latex-doc      build latex docs with doxygen (off by default)],
      [AC_SUBST(latex_yes_no,'YES')],
      [AC_SUBST(latex_yes_no,'NO')])

   AC_ARG_WITH([doc-output],
     [AS_HELP_STRING([--with-doc-output=PATH],[build documentation in
      path (prefix/documentation by default)])], 
      [AC_SUBST(doxygen_output_top,${with_doc_output})],
      [doxygen_output_top='DEFAULT'])

   AC_ARG_ENABLE([all-warnings],
    [AS_HELP_STRING([--enable-all-warnings],[activate all gcc warnings
       (-Wall -Wextra)])])

   AC_ARG_ENABLE([glibcxx-debug],
    [AS_HELP_STRING([--enable-glibcxx-debug],[Use the debug GLIBCXX
    libraries. This provides bounds checking for STL and
    more. (-D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC)])])

   dnl end of AC_DRACO_ARGS
])

dnl-------------------------------------------------------------------------dnl
dnl end of ac_dracoarg.m4
dnl-------------------------------------------------------------------------dnl

