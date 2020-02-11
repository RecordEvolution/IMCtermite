
RAW = ../raw/

SRC = src/
EXE = eatit

CCC = g++ -std=c++11
OPT = -O3 -Wall -mavx -mno-tbm -mf16c -mno-f16c
 
$(EXE) : $(SRC)main.cpp $(SRC)raweat.hpp $(SRC)half_precision_floating_point.hpp
	$(CCC) $(OPT) $< -o $@

clean :
	rm -f $(EXE)
