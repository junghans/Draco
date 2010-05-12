## -*- Mode: sh -*-----------------------------------------------------------##
##
## file  : bash_functions.sh
## author: Kelly Thompson
## date  : 2 Dec 2004
##
## Summary: Misc bash functions useful during development of code.
##
## 1. Use GNU tools instead of vendor tools when possible
## 2. Create some alias commands to provide hints when invalid
##    commands are issued.
##
## Functions
## ---------
##
## whichall <string> - show path of all commands in path that match
##                     <string>
##
## xe                - start XEmacs using gnuclient if an instance is
##                     already running
##
## cleanemacs        - recursively remove ~ files, .flc files and .rel
##                     files.
##
## ssh1/scp1         - force use of protocol version 1.
##
## fixperms          - Change permissions recursively.  Usage:
##                     'fixperms <world>'  If optional keyword "world"
##                     is ommitted, defaults to no access for world.
##
## loadandlistmodules - load modules specified in the string $defmods.
##
## findsymbol <sym>  - search all libraries (.so and .a files) in the
##                     current directory for symbol <sym>.
##
## pkgdepends        - Print a list of dependencies for the current
##                     directory. 
##
## mpush <file>      - Store <file> to HPSS/xfer and then push it to
##                     Mercury. 
##
## mpull <index>     - Pull index from Mercury (to HPSS/xfer) and then
##                     save it to the CWD.
##
## cuo <arg>         - Update all OPUS subdirectories using 
##                     "cvs update -Adp".  With any optional argument,
##                     only show changes.
##
## npwd              - function used to set the prompt under bash.
##
##---------------------------------------------------------------------------##

## Use gnu tools by default
if test -d /usr/local/gnu/bin; then
   export PATH=/usr/local/gnu/bin:${PATH}
fi

ulimit -s unlimited

##---------------------------------------------------------------------------##
## EOSPAC Setup
##---------------------------------------------------------------------------##

if test -d /ccs/codes/radtran/physical_data/eos; then
   export SESPATHU=/ccs/codes/radtran/physical_data/eos
   export SESPATHC=/ccs/codes/radtran/physical_data/eos
elif test -d /usr/projects/data/eos; then
   export SESPATHU=/usr/projects/data/eos
   export SESPATHU=/usr/projects/data/eos
fi

##---------------------------------------------------------------------------##
## Functions
##---------------------------------------------------------------------------##

function xe
{
  local me=`whoami`
  if test ${HOST:-none} = lambda; then
    xemacs -no-site-file -l /users/kellyt/.emacs $*
  elif test -z "`ps -u${me} | grep xemacs`" ; then
    xemacs $* -g 90x65+100+10
  else
    gnuclient $*
  fi
}

##---------------------------------------------------------------------------##
## Find all matches in PATH (not just the first one)
##---------------------------------------------------------------------------##

function whichall ()
{
    for dir in ${PATH//:/ }; do
        if [ -x $dir/$1 ]; then
            echo $dir/$1;
        fi;
    done
}

##---------------------------------------------------------------------------##
## Recursively delete all ~ files.
##---------------------------------------------------------------------------##

function cleanemacs
{
  echo "Cleaning up XEmacs temporary and backup files.";
  find . -name '*~' -exec echo rm -rf {} \;
  find . -name '*~' -exec rm -rf {} \;
  find . -name '.*~' -exec echo rm -rf {} \;
  find . -name '.*~' -exec rm -rf {} \;
  find . -name '*.flc' -exec echo rm -rf {} \;
  find . -name '*.flc' -exec rm -rf {} \;
  find . -name '*.rel' -exec echo rm -rf {} \;
  find . -name '*.rel' -exec rm -rf {} \;
  if test -d "${HOME}/.emacs-flc"; then
     echo "rm -r ${HOME}/.emacs-flc";
     rm -r ${HOME}/.emacs-flc;
  fi;
  echo "done."
}

##---------------------------------------------------------------------------##

function ssh1 { ssh -x -1 $*; }
function scp1 { scp -oProtocol=1 $*; }

##---------------------------------------------------------------------------##
## Used for formatting PROMPT.
##---------------------------------------------------------------------------##

function npwd() 
{
   local regHome=$(echo ${HOME} | sed -e 's/.*\///')
   #   How many characters of the $PWD should be kept
   local pwdmaxlen=40
   #   Indicator that there has been directory truncation:
   local trunc_symbol="..."
   # substitute ~ for $HOME to shorten the full path
   newPWD=$(echo ${PWD} | sed -e "s/.*${regHome}/~/")
   if [ ${#newPWD} -gt $pwdmaxlen ]
   then
   	local pwdoffset=$(( ${#newPWD} - $pwdmaxlen ))
	newPWD="${trunc_symbol}${newPWD:$pwdoffset:$pwdmaxlen}"
   fi
   echo ${newPWD}
}

# Alternate
function npwd_alt() 
{
   # Home directory
   local regHome=${HOME}
   # Home directory might be prepended with NFS mount location.
   local netHome=/net/machname.lanl.gov
   #   How many characters of the $PWD should be kept
   local pwdmaxlen=40
   #   Indicator that there has been directory truncation:
   local trunc_symbol="..."
   # substitute ~ for $HOME to shorten the full path
   newPWD=`echo ${PWD} | sed -e "s%${netHome}%%" | sed -e "s%${regHome}%~%"`
   if test ${#newPWD} -gt $pwdmaxlen ; then
   	local pwdoffset=$(( ${#newPWD} - $pwdmaxlen ))
	newPWD="${trunc_symbol}${newPWD:$pwdoffset:$pwdmaxlen}"
   fi
   echo ${newPWD}
}
# Another alternate
# PROMPT_COMMAND='DIR=`pwd|sed -e "s!$HOME!~!"`; if [ ${#DIR} -gt 30 ]; then CurDir=${DIR:0:12}...${DIR:${#DIR}-15}; else CurDir=$DIR; fi'
# PS1="[\$CurDir] \$ "

##---------------------------------------------------------------------------##
## Usage:
##    fixperms [world]
##
##    world - optional keyword to provide world read access.
##---------------------------------------------------------------------------##

function fixperms()
{
    local option1=$1
    local exebits='770'
    local filebits='660'
    if test ${option1:='group'} = 'world'; then
      exebits='775'
      filebits='664'
    fi
    # fix root directory first
    groupid=`groups | grep radtran`
    if test "${groupid}" == ""; then
       groupid=draco
    else
       groupid=radtran
    fi
    chgrp ${groupid} .
    chmod $exebits .
    # recursively fix other dirs and files.
    echo chgrp -R ${groupid} *
    chgrp -R ${groupid} *
    echo "Setting executable entries to bitmode $exebits"
    echo "Setting regular entries to bitmode $filebits"
    local permissions="700 750 770 775 777 500 550 555"
    for permbits in $permissions; do
       echo "find . -perm $permbits -exec chmod ${exebits} {} \;"
       find . -perm $permbits -exec chmod ${exebits} {} \;
    done
    permissions="600 640 660 644 664 400 440 444"
    for permbits in $permissions; do
       echo "find . -perm $permbits -exec chmod ${filebits} {} \;"
       find . -perm $permbits -exec chmod ${filebits} {} \;
    done
}

##---------------------------------------------------------------------------##
## Usage:
##    defmods = "(list of module name)..."
##    loadandlistmodules
##
## Note:
##    Must have environment $defmods set!
##---------------------------------------------------------------------------##

function loadandlistmodules()
{
    # Requires the following variables to be set:
    #
    # defmods="module1 module2 ..." // A list of modules

    source ${MODULESHOME}/init/bash

    module load ${defmods}
    module list

    # Setup the required stuff for modules to work under bash
    if ! `module list &> /dev/null`; then
       echo "********************************"
       echo "*** WARNING: could not init  ***"
       echo "***          module command. ***"
       echo "********************************"
    fi
}

##---------------------------------------------------------------------------##
## Usage:
##    findsymbol <symbol>
##
## Searches all .a and .so files in local directory for symbol
## <symbol>.  If found, the script provides the name of the library
## that contains the symbol.
##---------------------------------------------------------------------------##

function findsymbol()
{
    local nm_opt='-a'
    if test `uname` = OSF1; then
       nm_opt=''
    fi
    local a_libs=`\ls -1 *.a`
    if test -z "$a_libs"; then a_libs=""; fi
    local so_libs=`\ls -1 *.so`
    if test -z "$so_libs"; then so_libs=""; fi
    local libs="$a_libs $so_libs"
    echo " "
    echo "Searching..."
    local symbol=" T "
    for lib in $libs; do 
       local gres=`nm $nm_opt $lib | grep $1 | grep "$symbol"`
       if ! test "$gres" = ""; then
           echo " "
           echo "Found \"$symbol\" in $lib:"
           echo "     $gres"
       fi
    done
    echo " "
}


##---------------------------------------------------------------------------##
## Usage:
##    pkgdepends
##
## Purpose:
##    The script will list all of the vendors and Draco packages
##    dependencies for the files in the current directory.
##---------------------------------------------------------------------------##

function pkgdepends()
{
   echo "This package depends on:"
   echo " "
   echo "Packages:"
   grep 'include [<"].*[/]' *.cc *.hh | sed -e 's/.*[#]include [<"]/   /' | sed -e 's/\/.*//' | sort -u
   echo " "
   if test -f configure.ac; then
      echo "Vendors:"
      grep "SETUP[(]pkg" configure.ac | sed -e 's/AC_/   /' | sed -e 's/_.*//'
   fi
}

##---------------------------------------------------------------------------##
## Usage:
##    mpush <file1> ... <fileN>
##
## Purpose:
##     Check for kerberose ticket, get one if needed.  Next, store
##     <file1> on HPSS in the subdirectory "xfer".  Finally, push
##     <file1> via Mercury.
##---------------------------------------------------------------------------##

function mpush()
{
    # Ensure that an argument was provided.  The argument must be the name
    # of a file.
    if test x$1 == x; then
       echo "Error: no filename provided."
       echo "Useage:"
       echo "   mpush <filename>"
       return
    fi

    # Use psi or k5psi
    psicmd='psi'
    linux_release=`uname -r | sed -e 's/-.*//'`
    case ${linux_release} in
    2.6.*)
      psicmd='k5psi'
    ;;
    esac

    ticket_cache=`k5list | grep Ticket | sed -e 's/.*://'`
    # time stamps in seconds
    date_of_ticket=`date -r ${ticket_cache} +%s`
    date_now=`date +%s`
    eight_hours=28800
    dotpeh=`expr $date_of_ticket + $eight_hours`
    # Check for valid kerberose ticket.
    if test ${dotpeh} -lt ${date_now}; then
       # No ticket found
       echo "Last update to Kerberose Ticket: ${date_of_ticket} sec."
       echo "Should expire on               : ${dotpeh} sec."
       echo "Currently                      : ${date_now} sec."
       echo "No kerberose ticket found.  Let's get one..."
       k5init -f
    fi

    # Process for each file (Store file to HPSS)
    ${psicmd} store -d xfer $@

    # Push file via Mercury
    xfiles=""
    for file in $@; do
       xfiles="${xfiles} xfer/${file}"
    done
    push ${xfiles}

    echo "mpush: done."
    echo " "
}



##---------------------------------------------------------------------------##
## Usage:
##    mpull [N] [N] [...]
##
## Purpose:
##     Examine the output from the status command.  If there are files
##     on Mecury that are ready to pull, then pull them to HPSS and
##     then to the CWD.  If no arguments are provided, then pull all
##     available files.  If arguments are provided only pull the files
##     associated with the index number provided.
##---------------------------------------------------------------------------##

function mpull()
{
    # If no arguments, then pull all available files
    items=""
    numitems=0
    if test x$1 == x; then
       echo "Pulling all available Mercury files to HPSS."
       tmp=`status | sed -e 's/[A-Za-z:\s]*//g'`
       for i in ${tmp}; do
          items="${items} ${i}"
          numitems="`expr ${numitems} + 1`" 
       done
    else
       "Pulling item(s) $* from Mercury to HPSS."
       items=$*
       numitems=$#
    fi

    # Pull from Mercury
    echo "pull ${items} xfer"
    pull ${items} xfer

    # Get a list of files located in the user's HPSS xfer directory
    # sorted by time stamp.  Retrieve the most recent N files to the
    # user's CWD.  N is the number of files pulled from Mercury.
    i=1
    xfiles=`psi ls -t xfer/*`
    for file in ${xfiles}; do
       echo "Item ${i}: psi get ${file}"
       if test $i -eq $n; then
          break
       fi
       psi get ${file}
       i="`expr ${i} + 1`"
    done

    echo "mpull: done."
    echo " "
}

##---------------------------------------------------------------------------##
## Usage:
##    cd $OPUS; cuo
##---------------------------------------------------------------------------##
function cuo()
{
    if test x$1 == x; then
       :
    else
       nq="-n1"
    fi
    for dir in `\ls -1`; do
       if test -d $dir/CVS; then
          echo "cd $dir; cvs ${nq} update -AdP"
          cd $dir
          cvs ${nq} update -AdP
          cd ..
       fi    
    done
}

##---------------------------------------------------------------------------##
## Usage:
##    findgrep <regex>
##
## Finds all occurances of <regex> by looking at all files in the
## current directory and all subdirectories recursively.  Prints the
## filename and line number for each occurance.
##---------------------------------------------------------------------------##
function findgrep()
{
    # Exclude .svn directories: (-path '*/.svn' -prune)
    # Or (-o)
    files=`find . -path '*/.svn' -prune -o -type f -exec grep -q $1 {} /dev/null \; -print`
    for file in $files; do
      echo " "
      echo "--> Found \"$1\" in file \"$file\" :"
      echo " "
      grep $1 $file
    done
    echo " "
}

##---------------------------------------------------------------------------##
## Publish all functions to the current shell.
##---------------------------------------------------------------------------##

#export -f whichall cleanemacs ssh1 scp1 npwd fixperms mpush mpull
#export -f loadandlistmodules findsymbol xe pkgdepends cuo
