#-----------------------------------------------------------------------------#

SHELL := /bin/bash

# name of executable and CLI tool
EXE = imctermite

# directory names
SRC = src/
LIB = lib/
CYT = cython/
PYT = python/

# list headers
HPP = $(wildcard $(LIB)/*.hpp)

# choose compiler and its options
CC = g++ -std=c++17
#OPT = -O3 -Wall -mavx -mno-tbm -mf16c -mno-f16c
OPT = -O3 -Wall -Werror -Wunused-variable -Wsign-compare

# determine git version/commit and release tag
GTAG := $(shell git tag | tail -n1)
GHSH := $(shell git rev-parse HEAD | head -c8)
RTAG := v$(shell cat pip/setup.py | grep version | grep -oP "([0-9]\.){2}[0-9]")
CTAG := v$(shell cat cython/setup.py | grep version | grep -oP "([0-9]\.){2}[0-9]")

# define install location
INST := /usr/local/bin

#-----------------------------------------------------------------------------#
# C++ and CLI tool

# build exectuable
$(EXE) : check-vtag $(RTAG) main.o
	$(CC) $(OPT) main.o -o $@

# build main.cpp and include git version/commit tag
main.o : src/main.cpp $(HPP)
	@cp $< $<.cpp
	@sed -i 's/TAGSTRING/$(GTAG)/g' $<.cpp
	@sed -i 's/HASHSTRING/$(GHSH)/g' $<.cpp
	$(CC) -c $(OPT) -I $(LIB) $<.cpp -o $@
	@rm $<.cpp

install : $(EXE)
	cp $< $(INST)/

uninstall : $(INST)/$(EXE)
	rm $<

cpp-clean :
	rm -vf $(EXE)
	rm -vf *.o

#-----------------------------------------------------------------------------#
# check version consistency of git tags and version string in package.json

$(GTAG) :
	@echo "consistent versions check successful: building $(GTAG)"

check-vtag:
	@echo "git tag version:  "$(GTAG)
	@echo "git commit hash:  "$(GHSH)
	@echo "release version:  "$(RTAG)
	@echo "module version:   "$(CTAG)

#-----------------------------------------------------------------------------#
# Docker

docker-build :
	docker build ./ --tag imctermite:0.1

docker-run:
	docker run -it --rm imctermite:0.1 /bin/bash

#-----------------------------------------------------------------------------#
# python

cython-build : check-vtag $(CTAG) $(CYT)setup.py $(CYT)imc_termite.pxd $(CYT)py_imc_termite.pyx $(HPP)
	python3 $(CYT)setup.py build_ext --inplace
	cp -v imc_termite.cpython-*.so $(PYT)

cython-install : check-vtag $(CTAG) $(CYT)setup.py $(CYT)imc_termite.pxd $(CYT)py_imc_termite.pyx $(HPP)
	python3 $(CYT)setup.py install --record files_imctermite.txt

cython-clean :
	rm -vf imc_termite.cpython-*.so
	rm -vf $(PYT)imc_termite.cpython-*.so

#-----------------------------------------------------------------------------#
# pip

pip-release: check-vtag $(RTAG) cython-build
	cd ./pip/ && make publish

#-----------------------------------------------------------------------------#
# clean

clean: cpp-clean cython-clean
	cd ./pip/ && make clean

#-----------------------------------------------------------------------------#
