###############################################################################
## tstEnsight_Diff.py
## Thomas M. Evans
## Mon Jan 31 15:19:27 2000 
## $Id$
###############################################################################

import sys, os, string

##---------------------------------------------------------------------------##
## TtstEnsight_Diff.py
#
#  Check standard Ensight output with output produced by tstEnsight_Diff
##---------------------------------------------------------------------------##

def file_not_empty(filename):
    '''
    Returns true if filename is not an empty file.
    '''
    file = open(filename, 'r')
    line = diff_file.readline()
    if line:
        return 1
    else:
        return 0

prefixes  = ["./testproblem_ensight", "./part_testproblem_ensight"]
postfix   = ".0001"
dirs      = ["geo", "Temperatures", "Pressure", "Velocity", "Densities"]

# check the data files
for p in prefixes:
    for d in dirs:
        # create the file strings
        output   = p + '/' + d + '/data' + postfix
        ref_out  = d + postfix
        diff_out = d + '.diff'

        # diff the output and reference
        diff_line = "diff %s %s > %s" % (output, ref_out, diff_out)
        os.system(diff_line)

        # print out an error message if we get lines different
        if file_not_empty(diff_out):
            print "tstEnsight_Diff Test: Failed in file %s" % output

# check the case file
casefile = 'testproblem.case'
caseout  = prefix + '/' + casefile
casediff = 'case.diff'

diff_line = "diff %s %s > %s" % (caseout, casefile, casediff)
os.system(diff_line)
    
# print out an error message if we get lines different
if file_not_empty(casediff):
    print "tstEnsight_Diff Test: Failed in file %s" % caseout


###############################################################################
##                            end of tstEnsight_Diff.py
###############################################################################

