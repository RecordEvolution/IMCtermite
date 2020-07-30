
PARQUETDIR := /home/mario/Desktop/Record_Evolution/parquet-cpp
ARROWDIR := /home/mario/Desktop/Record_Evolution/arrow/cpp/src

CPP := g++ -std=c++14
OPT := 
#-Wall -Woverflow -Wpedantic -Wextra -Waddress -Waligned-new -Walloc-zero

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

#------------------------------------------------------------------------------------#
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





