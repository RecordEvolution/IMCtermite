#-----------------------------------------------------------------------------#

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

# determine git version/commit tag
GTAG := $(shell git tag | tail -n1)
GHSH := $(shell git rev-parse HEAD | head -c8)

# define install location
INST := /usr/local/bin

#-----------------------------------------------------------------------------#
# C++ and CLI tool

# build exectuable
$(EXE) : main.o
	$(CC) $(OPT) $^ -o $@

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

clean :
	rm -vf $(EXE)
	rm -vf *.o

#-----------------------------------------------------------------------------#
# Docker

docker-build :
	docker build ./ --tag imctermite:0.1

docker-run:
	docker run -it --rm imctermite:0.1 /bin/bash

#-----------------------------------------------------------------------------#
# python

cython-build : $(CYT)setup.py $(CYT)imc_termite.pxd $(CYT)py_imc_termite.pyx $(HPP)
	python3 $< build_ext --inplace

#-----------------------------------------------------------------------------#
# Python (to be removed)

# build python module
py : $(CYT)setup_raw_eater.py $(CYT)raw_eater.pyx $(CYT)raw_eater.pxd $(LIB)raweat.hpp \
     $(CYT)setup_raw_meat.py $(CYT)raw_meat.pyx $(CYT)raw_meat.pxd $(LIB)rawmerge.hpp \
		 output
	python3 $(CYT)setup_raw_eater.py build_ext --inplace
	python3 $(CYT)setup_raw_meat.py build_ext --inplace
	cp raw_eater.cpython-*.so $(PYT)
	cp raw_meat.cpython-*.so $(PYT)
	rm *.so

py-install: $(CYT)setup_raw_eater.py $(CYT)raw_eater.pyx $(CYT)raw_eater.pxd $(LIB)raweat.hpp \
            $(CYT)setup_raw_meat.py $(CYT)raw_meat.pyx $(CYT)raw_meat.pxd $(LIB)rawmerge.hpp
	python3 $(CYT)setup_raw_eater.py install --record files_raw_eater.txt
	python3 $(CYT)setup_raw_meat.py install --record files_raw_meat.txt

py-clean :
	rm -f raw_eater.cpython-*.so
	rm -f $(PYT)raw_eater.cpython-*.so
	rm -f $(CYT)raw_eater.cpp
	rm -f raw_meat.cpython-*.so
	rm -f $(PYT)raw_meat.cpython-*.so
	rm -f $(CYT)raw_meat.cpp
	rm -rf build/
	rm -f *.txt
	rm -rf output/

# prepare directory for test output
output :
	mkdir -pv output/

#-----------------------------------------------------------------------------#
