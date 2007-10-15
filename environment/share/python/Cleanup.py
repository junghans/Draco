"""A module to support sanitary computing!

Classes in this module support automatically taking various actions
that should be performend when a script ends.

Each class, when instantiated, registers it's necessary members with
atexit.

Future additions to this module could include file deleters which only
operate under certain conditions, e.g. when it would be an error to
delete non-empty files and/or directories.

Off-hand, I can't think of any resources other than temporary files
that need to be cleaned up this way. If I think of them, I'll put
handelers for them here.

"""

import atexit, os, shutil
import Verbosity

class Deleter:
    """
    Register files and diretories with an object of this class to
    have them automatically deleted at the end of program
    execution. E.g. temporary files.

    All exceptions generated by deleting are caught and printed, but
    not re-thrown. This is because it's not clear what happens to
    untrapped exceptions triggered by atexit, or how failures to
    delete could be gracefully handled just before program
    termination.

    """

    def __init__(self, filenames, verbosity=Verbosity.ignore()):

        self.filenames = filenames
        self.verbosity = verbosity

        atexit.register(self.cleanup)

    def cleanup(self):

        for entry in self.filenames:

            try:

                if os.path.isfile(entry):
                    self.verbosity("Deleting file: %s" % entry, 3)
                    os.remove(entry)
                elif os.path.isdir(entry):
                    self.verbosity("Deleting tree: %s" % entry, 3)
                    shutil.rmtree(entry)

            except Exception, e:
                print "Caught an exception during cleanup: %s" % e


def delete_on_exit(filename, verbosity=Verbosity.ignore()):

    """Register an event to delete the given filename"""

    Deleter([filename], verbosity)