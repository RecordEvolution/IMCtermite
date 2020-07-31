#-----------------------------------------------------------------------------#

import argparse

#-----------------------------------------------------------------------------#

parser = argparse.ArgumentParser(description='List all source dependencies')
parser.add_argument('pathToRepo',type=str,help='path of source repository')
parser.add_argument('mainSource',type=str,help='main source file')
parser.add_argument('depFile',type=str,help='file listing all dependencies')
args = parser.parse_args()

#-----------------------------------------------------------------------------#

def find_dependencies(srcfile, repopath) :
    """
    Given a source file and its dependencies in the given repository path
    list all further dependencies recursively
    """

if __name__== "__main__":

    print("\nCLI arguments:\n" + str(args))


#-----------------------------------------------------------------------------#
