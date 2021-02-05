#-----------------------------------------------------------------------------#

import argparse
import os

#-----------------------------------------------------------------------------#

parser = argparse.ArgumentParser(description='List all source dependencies')
#parser.add_argument('pathToRepo',type=str,help='path of source repository')
parser.add_argument('mainSource',type=str,help='main source file')
parser.add_argument('depFile',type=str,help='file listing all dependencies')
args = parser.parse_args()

libpaths = ["/home/mario/Desktop/arrow/cpp/src/",
            "/home/mario/Desktop/arrow/cpp/thrift_ep-install/include/",
            "/home/mario/Desktop/arrow/cpp/boost_ep-prefix/src/boost_ep/"]

#-----------------------------------------------------------------------------#

def find_dependencies(srcfile, recdepth, cdeplist) :
    """
    Given a source file and its dependencies in the given repository path
    list all further dependencies recursively

    Args:
        srcfile (string): path/name of source file
        recdepth (integer): current recursion depth
        cdeplist (list): current list of dependencies
    Return:
        deps (list): list of source files in repository, the source file depends on
    """
    # define indentation to visual recursion
    indent = recdepth*("  ")

    print("\n" + indent + "find_dependencies:"
        + "\n" + indent + "1: " + srcfile
        + "\n" + indent + "2: " + str(recdepth)
        + "\n" + indent + "3: " + str(len(cdeplist)) + "\n")

    # show dependencies so far
    #print(cdeplist)

    # generate dependencies by means of g++
    libdeps = (" -I ").join(libpaths)
    cmd = "g++ -c -MMD " + srcfile + " -I " + libdeps
    print(indent + cmd )
    os.system(cmd)

    # open dependency file and extract list of sources
    basename = srcfile.split('/')[-1].split('.')[0]
    depfile = basename + '.d'
    print(indent + "reading dependency file " + depfile)
    with open(depfile,'r') as fin :
        depslist = fin.readlines()

    # delete dependencies and object files
    os.system("rm " + basename + ".d")
    os.system("rm " + basename + ".o")

    # remove first line
    depslist = depslist[1:]

    # delete leading space and trailing backslash
    depslistcl = [dep.lstrip(' ').rstrip(' \\\n') for dep in depslist]

    # collect dependencies
    newdeps = []

    # check all dependencies recursively and collect further dependencies
    count = 0
    for dep in depslistcl :
        # append source itself to list
        if dep not in cdeplist :
            print(indent + "adding dependency " + dep)
            newdeps.append(dep)
            count = count + 1
    print(indent + "=> added " + str(count) + "/" + str(len(depslistcl)) )

    # check recursion depth
    if recdepth < 20 :
        # check all dependencies of every single dependency
        for dep in depslistcl :
            # try to find corresponding *.cc, (*.cpp) file
            depcc = dep.split('.')[0] + '.cc'
            print(indent + "checking for " + depcc)
            if os.path.exists(depcc) :
                if depcc not in cdeplist and depcc not in newdeps :
                    # add file itself as dependency
                    newdeps.append(depcc)
                    # find dependencies of single source
                    newrecdeps = find_dependencies(depcc,recdepth+1,cdeplist+newdeps)
                    # append to list
                    for el in newrecdeps :
                        if el not in newdeps :
                            newdeps.append(el)
                else :
                    print(indent + "already in list")
            else :
                print(indent + "does not exist")

    print("\n")

    # provide list of dependencies
    return newdeps

#-----------------------------------------------------------------------------#

if __name__== "__main__":

    print("\nCLI arguments:\n" + str(args) + "\n")

    # collect list of dependencies
    deps = []

    # start recursion with given source file
    deps = find_dependencies(args.mainSource,0,[])

    print("\nfinal list of dependencies: (" + str(len(deps)) + ")\n")
    print(deps)
    print("\n")

    # remove any duplicates
    depsuni = set(deps)
    print("\nfinal set of dependencies: (" + str(len(depsuni)) + ")\n")
    print(depsuni)
    print("\n")

    # write list of dependencies
    with open(args.depFile,'w') as fout :
        for el in depsuni :
            fout.write(str(el) + '\n')

#-----------------------------------------------------------------------------#
