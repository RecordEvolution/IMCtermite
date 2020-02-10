
RAW = ../raw/

SRC = src/
EXE = eatit

CCC = g++
OPT = -O3 -Wall
 
$(EXE) : $(SRC)main.cpp $(SRC)raweat.hpp $(SRC)half_precision_floating_point.hpp
	$(CCC) $(OPT) $< -o $@

clean :
	rm -f $(EXE)
