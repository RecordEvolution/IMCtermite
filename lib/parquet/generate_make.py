#-----------------------------------------------------------------------------#

import glob
from pathlib import Path

# find source files
srcpaths = Path("src/").rglob('*.cc')
deps =[ str(path) for path in srcpaths ]

print(deps)

with open('makefileobj','w') as fout :
    for el in deps :
        basnam = el.split('/')[-1]
        print(str(el) + " : " + str(basnam) + " : " + str(basnam.split('.')[1]))
        if basnam.split('.')[1] == 'cc' :
            objfile = 'bin/' + basnam.replace('.cc','.o')
            fout.write(objfile + " : " + el + "\n")
            fout.write("\t" + "$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@\n")
            fout.write("\n")


#-----------------------------------------------------------------------------#
