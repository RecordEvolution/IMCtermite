
SHELL:=/bin/bash

RAW = ../raw/

SRC = src/
EXE = eatraw

CCC = g++ -std=c++11
OPT = -O3 -Wall -mavx -mno-tbm -mf16c -mno-f16c

# build executable 
$(EXE) : $(SRC)main.cpp $(SRC)raweat.hpp $(SRC)half_precision_floating_point.hpp
	$(CCC) $(OPT) $< -o $@

# remove executable
clean :
	rm -f $(EXE)

# check existence of name of executable globally
chexe:=$(shell command -v $(EXE))

# install executable if name does not exist yet
install : $(EXE)
ifeq ($(chexe),)
	cp $(EXE) /usr/local/bin/
else
	@echo "executable with same name already exists! choose different name!"
	@exit 1
endif

# deinstall
deinstall :
	rm /usr/local/bin/$(EXE)

