###############################################################################
## find_tags.py
## Thomas M. Evans
## Mon Dec 17 10:31:44 2001
## $Id$
###############################################################################
##---------------------------------------------------------------------------##
## determine the release tag of a package by examining all files in
## the package directory + Release.cc and presents a report; it
## expects to find a Release.cc and configure.in file in the directory
## Usage:
##       1) enter package directory
##       2) python ../../tools/find_tags.py
##---------------------------------------------------------------------------##

##---------------------------------------------------------------------------##
## imported modules
##---------------------------------------------------------------------------##

import commands
import os
import glob
import fnmatch
import re
import string
import sys

##---------------------------------------------------------------------------##
## GLOBAL VARIABLES (PACKAGE NAME
##---------------------------------------------------------------------------##

# determine package directory
pkg_dir  = os.getcwd()

if len(sys.argv) > 1:
    pkg_name = sys.argv[1]
else:
    pkg_name = os.path.basename(pkg_dir)

##---------------------------------------------------------------------------##
## Regular expressions
##---------------------------------------------------------------------------##

re_tag      = re.compile(r'[a-zA-Z0-9].+\-[0-9]\_[0-9]\_[0-9]', re.IGNORECASE)
re_tag_pkg  = re.compile(r'(.+)\-.+', re.IGNORECASE)
re_files    = re.compile(r'\nT\s+(.*)', re.IGNORECASE)
re_tag_file = re.compile(r'string\s+pkg\_release\s+\=\s*\"\@\(\#\)(.*)\"', re.IGNORECASE)

##---------------------------------------------------------------------------##
## FUNCTION: determine the current package tag
##---------------------------------------------------------------------------##

def get_current_tag():

    # check to see if Release.cc exists
    if not os.path.isfile('configure.in'):
        print ">>> No configure.in ................. exiting"
        sys.exit(1)

    # check the configure.in files for tags
    tags = commands.getoutput('cvs status -v configure.in')

    match = re_tag.findall(tags)

    # initialize tag_prefix
    this_tag   = ''
    ctr        = 0

    while this_tag == '' and ctr < len(match) and len(match) > 0:

        temp_tag = match[ctr]
        
        # find tag prefix
        tag_prefix_match = re_tag_pkg.search(temp_tag)
        if tag_prefix_match:
            tag_prefix = tag_prefix_match.group(1)
            if (tag_prefix == pkg_name): this_tag = temp_tag

        # increment counter
        ctr = ctr + 1
        
    return this_tag

##---------------------------------------------------------------------------##
## FUNCTION: check files that have changed since current tag
##---------------------------------------------------------------------------##

def get_files_changed_since_tag(tag):

    # get a string of files
    command   = 'cvs -n tag -F ' + tag
    files     = commands.getoutput(command)

    # get a list of changed files
    file_list = re_files.findall(files)

    return file_list

##---------------------------------------------------------------------------##
## FUNCTION: determine what the release tag is in Release.cc
##---------------------------------------------------------------------------##

def get_Releasecc_tag():

    # check to see if Release.cc exists
    if not os.path.isfile('Release.cc'):
        release_tag = 'no Release.cc'

    else:
        # else open the file and find the release tag
        file  = open('Release.cc', 'r')
        lines = file.readlines()
        
        release_tag = ''
        
        for line in lines:
            match = re_tag_file.search(line)
            if match:
                release_tag = match.group(1)

    return release_tag
        
##---------------------------------------------------------------------------##
## MAIN PROGRAM
##---------------------------------------------------------------------------##

# announcement
print ">>> Working in package directory    : %s" % (pkg_dir)
print ">>> Package name is                 : %s" % (pkg_name)

# check Release.cc for release tag
releasecc_tag = get_Releasecc_tag()
print ">>> Release tag found in Release.cc : %s" % (releasecc_tag)

# determine the existing tag
current_tag = get_current_tag()

# print out message
if current_tag == '':
    print ">>> Found current cvs tag           : none found"
else:
    print ">>> Found current cvs tag           : %s" % (current_tag)

# check files that have changed since current tag applied
files = get_files_changed_since_tag(current_tag)

# print out message
if len(files) == 0:
    print ">>> Files changed since current tag : none"
else:
    print ">>> Files changed since current tag :"
    for f in files:
        print "    %s" % (f)


###############################################################################
##                            end of find_tags.py
###############################################################################

