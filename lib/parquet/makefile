#-----------------------------------------------------------------------------#

PARQUETDIR := /home/mario/Desktop/Record_Evolution/parquet-cpp
ARROWDIR := /home/mario/Desktop/Record_Evolution/arrow/cpp/src

CPP := g++ -std=c++14
OPT := -Wall -Woverflow -Wpedantic -Wextra -Waddress -Waligned-new -Walloc-zero

prepare : collect_parquet modify_parquet collect_arrow modify_arrow

collect_parquet :
	cp -r $(PARQUETDIR)/src/parquet ./
	cp $(PARQUETDIR)/examples/low-level-api/reader_writer.h ./
	cp $(PARQUETDIR)/examples/low-level-api/reader-writer.cc ./

modify_parquet :
	cp parquet/parquet_version.h.in parquet/parquet_version.h
	sed -i 's/ReadableFileInterface/ReadWriteFileInterface/g' parquet/util/memory.h
	sed -i 's/ReadableFileInterface/ReadWriteFileInterface/g' parquet/file_reader.h
	sed -i 's/arrow::Codec/arrow::util::Codec/g' parquet/util/memory.h
	sed -i 's/valid_bits_writer/valid_bits_offset/g' parquet/column_reader.h

collect_arrow :
	cp -r $(ARROWDIR)/arrow ./

modify_arrow :
	cp arrow/util/bit_util.h arrow/util/bit-util.h

collect_test :
	cp $(PARQUETDIR)/examples/low-level-api/reader-writer.cc ./

subst :
	sed -i 's/#include \"arrow\//\/\/#include \"arrow/g' parquet/properties.h

test :
	$(CPP) $(OPT) -I$(PWD) reader-writer.cc

clean :
	rm -r parquet/ arrow/
	rm reader-writer.cc reader_writer.h
#-----------------------------------------------------------------------------#

# choose shell
SHELL:=/bin/bash

SRC = reader-writer

# specify path of cloned directory
ARROWGIT := /home/mario/Desktop/Record_Evolution/arrow

filewriter : parquet/file_writer.cc
		$(CPP) -c $(OPT) $<

# build executable (and generate dependency file)
readwrite : reader-writer.cc
		$(CPP) $(OPT) -MMD $< -I ./

# generate dependency file
$(SRC).d : $(SRC).cc
		$(CPP) -c -MMD $< -I ./ -I $(ARROWGIT)/cpp/src/

# extract source dependencies
extract-dep : $(SRC).d
		@# extract relevant dependencies
		cat $< | sed 's/ /\n/g' | awk 'NF' | grep -v '\\' | grep '\/' > deps.log
		cat deps.log | sed ':a;N;$!ba;s/\n/ /g' > headers.log
		cat headers.log | sed 's/.h$$/.cc/g' > sources.log
		@# copy required sources
		mkdir -p temp/
		cp --parents `cat headers.log` temp/
		cp --parents `cat sources.log` temp/ 2>/dev/null
		mv temp$(ARROWGIT)/cpp/src/* ./
		rm -r temp

clean-dep :
		rm -f deps.log headers.log sources.log $(SRC).d

#-----------------------------------------------------------------------------#
# prepare source
#
# before: $ cd arrow/cpp/  and compile relevant sources by 
#         $ cmake . -D ARROW_PARQUET=ON -D PARQUET_BUILD_EXAMPLES=ON -D ARROW_WITH_SNAPPY=ON
#         $ cmake .. -D ARROW_PARQUET=ON ARROW_BUILD_EXAMPLES=ON

cp-src : deps.log
	mkdir src
	cat deps.log | while read f; do echo $f; cp --parents $f src/; done;
	mv src/home/mario/Desktop/arrow/cpp/* src/
	rm -r src/home/

deps.log :
	python3 generate_deps.py reader-writer.cc $<


SRC := $(shell find src/ -name '*.cc')

# OBJ := $(apprefix obj/, $(SRC:%.cc=%.o))
OBJ := $(addprefix bin/,$(notdir $(SRC:%.cc=%.o)))

check :
	echo $(SRC)
	echo $(OBJ)

reader-writer-example : reader-writer.cc $(OBJ)
	g++ $< -I src/src/ -o $@ $(OBJ)

$(OBJ) : $(SRC)
	$(CPP) $(OPT) -c $< -o $@ -I src/src/


#-----------------------------------------------------------------------------#
# only use more recent and up to date repository arrow.git

# build arrow shared/static libraries
build :
	cd arrow/cpp
	# cmake -LA  to show all options
	cmake . -D ARROW_PARQUET=ON #ARROW_ARMV8_ARCH=armv8-a
	make

example :
	cd arrow/cpp/examples/parquet/low-level-api/
	g++ reader-writer.cc -I. -I../../../src/ -L../../../../cpp/build/release/ -larrow -lparquet

# set environment variable LD_LIBRARY_PATH=../../../../cpp/build/release/ before launching executable


#------------------------------------------------------------------------------------#
