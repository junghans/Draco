#!/bin/bash -l

##---------------------------------------------------------------------------##
## Helpful functions
##---------------------------------------------------------------------------##

function die () { echo "ERROR: $1"; exit 1;}

function run () {
  echo $1
  if ! test $dry_run; then eval $1; fi
}

#----------------------------------------------------------------------#
# The script starts here
#----------------------------------------------------------------------#

function establish_permissions
{
  # Permissions - new files should be marked u+rwx,g+rwx,o+rx
  # Group is set to $1 or draco
  umask 0002
  if test `groups | grep othello | wc -l` = 1; then
    install_group="othello"
    install_permissions="g+rwX,o-rwX"
  else
    install_group="draco"
    install_permissions="g+rwX,o=g-w"g
  fi
  build_permissions="g+rwX"
}

# Logic taken from /usr/projects/hpcsoft/templates/header
function machineName
{
  sysName="unknown"
  if test -f /usr/projects/hpcsoft/sys_name; then
    sysName=`/usr/projects/hpcsoft/sys_name`
  fi
  if test "$sysName" = "unknown"; then
    echo "Unable to determine system OS, please edit scripts/common.sh."
    exit 1
  fi
  echo $sysName
}

# Logic taken from /usr/projects/hpcsoft/templates/header
function osName
{
  osName="unknown"
  if test -f /usr/projects/hpcsoft/sys_os; then
    osName=`/usr/projects/hpcsoft/sys_os`
  fi
  if test "$osName" = "unknown"; then
    echo "Unable to determine system OS, please edit scripts/common.sh."
    exit 1
  fi
  echo $osName
}

function flavor
{
  platform=`machineName`
  os=`osName`
  case $os in
    toss*)
      if test -z $LMPI; then
        mpiflavor="unknown"
      else
        mpiflavor=$LMPI-$LMPIVER
      fi
      if test -z $LCOMPILER; then
        compilerflavor="unknown"
      else
        compilerflavor=$LCOMPILER-$LCOMPILERVER
      fi
      ;;
    cle*)
      if test -z $CRAY_MPICH2_VER; then
        mpiflavor="unknown"
      else
        mpiflavor=mpt-$CRAY_MPICH2_VER
      fi
      # Try to determine the loaded compiler
      loadedmodules=`echo $LOADEDMODULES`
      OLDIFS=$IFS
      IFS=:
      # select modules that look like compilers
      unset compilermodules
      for module in $loadedmodules; do
        case $module in
          PrgEnv*)
            # Ingore PrgEnv matches.
            ;;
          intel/* | pgi/* | cray/* | gnu/* )
            tmp=`echo $module | sed -e 's%/%-%'`
            compilermodules="$tmp $compilermodules"
            ;;
        esac
      done
      IFS=$OLDIFS
      # pick the first compiler in the list
      compilerflavor=`echo $compilermodules | sed -e 's/ *//'`
      ;;
  esac
  echo $platform-$mpiflavor-$compilerflavor
}

function selectscratchdir
{
  scratchdirs="scratch scratch1 scratch3 lscratch1"
  for dir in $scratchdirs; do
    if test -x /$dir/$USER; then
      echo "$dir"
      return
    fi
  done
}

function lookupppn()
{
  # https://hpc.lanl.gov/index.php?q=summary_table
  local target="`uname -n | sed -e s/[.].*//`"
  local ppn=1
  case ${target} in
    c[it]-fe[0-9] | c[it]-login[0-9] | c[it]-vizlogin[0-9]) ppn=16 ;;
    ty* ) ppn=32 ;;
    mu* ) ppn=24 ;;
    ml* | pi* | wf* | lu* ) ppn=16 ;;
    mp* ) ppn=8 ;;
    tt-fey* | tt-login*) ppn=32 ;;
  esac
  echo $ppn
}

function npes_build
{
  local np=1
  if ! test "${PBS_NP}x" = "x"; then
    np=${PBS_NP}
  elif ! test "${SLURM_NPROCS}x" = "x"; then
    np=${SLURM_NPROCS}
  elif ! test "${SLURM_CPUS_ON_NODE}x" = "x"; then
    np=${SLURM_CPUS_ON_NODE}
  elif test -f /proc/cpuinfo; then
    np=`cat /proc/cpuinfo | grep processor | wc -l`
  fi
  echo $np
}

function npes_test
{
  local np=1
  if ! test "${PBS_NP}x" = "x"; then
    np=${PBS_NP}
  elif ! test "${SLURM_NPROCS}x" = "x"; then
    np=${SLURM_NPROCS}
  elif ! test "${SLURM_CPUS_ON_NODE}x" = "x"; then
    np=${SLURM_CPUS_ON_NODE}
  elif test -f /proc/cpuinfo; then
    np=`cat /proc/cpuinfo | grep processor | wc -l`
  fi
  # For Cray systems limit the test count to 8.
  local os=`osName`
  case $os in
    cle*) np=8 ;;
  esac
  echo $np
}

##---------------------------------------------------------------------------##
## Configure, Build and Run the Tests
##---------------------------------------------------------------------------##

function install_versions
{
  local config_step=0
  local build_step=0
  local test_step=0

  if test -z "${steps}"; then
    echo "You must provide variable steps."
    echo "E.g.: steps=\"configure build test\""
    return
  else
    for s in $steps; do
      case $s in
        config) config_step=1 ;;
        build)  build_step=1  ;;
        test)   test_step=1   ;;
      esac
    done
  fi
  if test -z ${buildflavor}; then
    buildflavor=`flavor`
  fi
  if test -z "${version}"; then
    echo "You must provide variable version."
    # echo "E.g.: VERSIONS=( \"debug\" \"opt\" )"
    return
  fi
  if test -z "${options}"; then
    echo "You must provide variable option."
    #echo "E.g.: OPTIONS=("
    #echo "\"$OPTIMIZE_OFF $LOGGING_OFF\""
    #echo "\"$OPTIMIZE_ON  $LOGGING_OFF\""
    #echo ")"
    return
  fi
  if test -z "${package}"; then
    echo "You must provide variable package."
    echo "E.g.: package=\"draco\""
    return
  fi
  if test -z "${install_prefix}"; then
    echo "You must provide variable install_prefix."
    echo "E.g.: install_prefix=/usr/projects/draco/$pdir/$buildflavor"
    return
  fi
  if test -z ${build_pe}; then
    build_pe=`npes_build`
  fi
  if test -z ${test_pe}; then
    test_pe=`npes_test`
  fi

  # Echo environment before we start:
  echo
  echo
  echo "# Environment"
  echo "# ------------"
  echo
  run "module list"
  run "printenv"

  echo
  echo
  echo "# Begin release: $buildflavor/$version"
  echo "# ------------"
  echo

  # Create install directory
  install_dir="$install_prefix/$version"
  if ! test -d $install_dir; then
    run "mkdir -p $install_dir" || die "Could not create $install_dir"
  fi

  # try to locate the souce directory
  if test -f $source_prefix/source/ChangeLog; then
    source_dir=$source_prefix/source
  else
    local possible_source_dirs=`/bin/ls -1 $source_prefix/source`
    for dir in "$possible_source_dirs"; do
      if test -f $source_prefix/source/$dir/ChangeLog; then
        source_dir=$source_prefix/source/$dir
        break
      fi
    done
  fi
  if ! test -f $source_dir/CMakeLists.txt; then
    echo "Could not find sources. Tried looking at $source_prefix/source/"
    exit 1
  fi
  # source_dir="$source_prefix/source/$package"
  build_dir="$build_prefix/$version/${package:0:1}"

  # Configure options for Cray
  case `osName` in
    cle*)
      if test -f $source_dir/config/CrayConfig.cmake; then
        CONFIG_EXTRA="-C $source_dir/config/CrayConfig.cmake $CONFIG_EXTRA"
      elif test -f $draco_dir/cmake/draco/CrayConfig.cmake; then
        CONFIG_EXTRA="-C $draco_dir/cmake/draco/CrayConfig.cmake $CONFIG_EXTRA"
      else
        echo "Could not find CrayConfig.cmake!"
        exit 1
      fi
      CONFIG_EXTRA="-DDRACO_LIBRARY_TYPE=STATIC $CONFIG_EXTRA"
      ;;
  esac

  # Purge any existing files before running cmake to configure the build directory.
  if test $config_step == 1; then
    if test -d ${build_dir}; then
      run "rm -rf ${build_dir}"
    fi
    run "mkdir -p $build_dir" || die "Could not create directory $build_dir."
  fi

  run "cd $build_dir"
  if test $config_step == 1; then
    run "cmake -DCMAKE_INSTALL_PREFIX=$install_dir \
             $options $CONFIG_EXTRA $source_dir" \
      || die "Could not configure in $build_dir from source at $source_dir"
  fi
  if test $build_step == 1; then
    run "make -j $build_pe -l $build_pe install"  \
      || die "Could not build code/tests in $build_dir"
  fi
  if test $test_step == 1; then
    run "ctest -j $test_pe" # Allow some tests to fail.
  fi
  if ! test -z $build_permissions; then
    run "chmod -R $build_permissions $build_dir"
  fi

}

##---------------------------------------------------------------------------------------##
## If $jobids is set, wait for those jobs to finish before setting
## groups and permissions.
function publish_release()
{
  echo "waiting batch jobs to finish ..."

  # wait for jobs to finish
  for jobid in $jobids; do
    while test "`showq | grep $jobid`" != ""; do
      showq | grep $jobid
      sleep 5m
    done
  done

  if ! test -z $install_permissions; then
    # Set access to top level install dir.
    if test -d $install_prefix; then
      run "chmod $install_permissions $install_prefix"
      run "chgrp -R ${install_group} $install_prefix"
    fi

    dirs="$source_prefix $source_prefix/source  $source_prefix/logs"
    for dir in $dirs; do
      if test -d $dir; then
        run "chgrp draco $dir"
        run "chmod $install_permissions $dir"
      fi
    done
  fi
}

##---------------------------------------------------------------------------------------##
export die
export run
export establish_permissions
export machineName
export osName
export flavor
export selectscratchdir
export npes_build
export npes_test
export install_versions
##---------------------------------------------------------------------------------------##
