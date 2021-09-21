#-----------------------------------------------------------------------------#

SHELL := /bin/bash

# name of executable and CLI tool
EXE = IMCtermite

# directory names
SRC = src/
LIB = lib/
PYT = python/

# list headers
HPP = $(wildcard $(LIB)/*.hpp)

# choose compiler and its options
CC = g++ -std=c++17
OPT = -O3 -Wall -Wconversion -Wpedantic -Werror -Wunused-variable -Wsign-compare

# determine git version/commit and release tag
GTAG := $(shell git tag -l --sort=version:refname | tail -n1)
GHSH := $(shell git rev-parse HEAD | head -c8)

# current timestamp
TMS = $(shell date +%Y%m%dT%H%M%S)

# define install location
INST := /usr/local/bin

#-----------------------------------------------------------------------------#
# C++ and CLI tool

# build exectuable
$(EXE) : main.o
	$(CC) $(OPT) main.o -o $@

# build main.cpp and include git version/commit tag
main.o : src/main.cpp $(HPP)
	@cp $< $<.cpp
	@sed -i 's/TAGSTRING/$(GTAG)/g' $<.cpp
	@sed -i 's/HASHSTRING/$(GHSH)/g' $<.cpp
	@sed -i 's/TIMESTAMPSTRING/$(TMS)/g' $<.cpp
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
# C++ linter

check-code:
	cppcheck --enable=all -I lib/ src/main.cpp

#-----------------------------------------------------------------------------#
# Docker

docker-build :
	docker build ./ --tag imctermite:0.1

docker-run:
	docker run -it --rm imctermite:0.1 /bin/bash

#-----------------------------------------------------------------------------#
# python

python-build:
	make -C python/ build-inplace
	cp python/IMCtermite*.so ./ -v

python-clean:
	make -C python/ clean
	rm -vf IMCtermite*.so

#-----------------------------------------------------------------------------#
# clean

clean: cpp-clean python-clean

#-----------------------------------------------------------------------------#
# github actions

github-action-lint: .github/workflows/pypi-deploy.yml
	actionlint $<

# for reference, see:
# https://github.com/rhysd/actionlint

#-----------------------------------------------------------------------------#
