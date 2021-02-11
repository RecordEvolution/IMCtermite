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
	rm -v *.o

# # build CLI executable
# $(EXE) : $(SRC)main.cpp $(LIB)raweat.hpp $(LIB)hexshow.hpp $(LIB)rawmerge.hpp output
# 	$(CCC) $(OPT) $< -o $@

# # development version
# eatdev : $(SRC)main_dev.cpp $(LIB)raweat.hpp
# 	$(CCC) $(OPT) $< -o $@
#
# # build target for conversion set of .raw files
# eatall : $(SRC)eatall.cpp $(LIB)raweat.hpp
# 	$(CCC) $(OPT) $< -o $@

# check existence of name of executable globally
# chexe:=$(shell command -v $(EXE))
#
# # install executable if name does not exist yet
# install : $(EXE)
# ifeq ($(chexe),)
# 	cp $(EXE) /usr/local/bin/
# else
# 	@echo "executable with name already exists! choose different name!"
# 	@exit 1
# endif

# # uninstall
# uninstall :
# 	rm /usr/local/bin/$(EXE)

#-----------------------------------------------------------------------------#
# Python

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
