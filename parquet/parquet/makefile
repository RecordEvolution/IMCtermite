#-----------------------------------------------------------------------------#

CPP := g++ -std=c++14
CPPFLAGS := -Woverflow -Wpedantic -Wextra -Waddress -Waligned-new -Walloc-zero

SRC := src/
BIN := bin/

LIBS := -I src/src/ -I src/thrift_ep-install/include/ -I src/boost_ep-prefix/src/boost_ep/

#-----------------------------------------------------------------------------#
# prepare source
#
# before: $ cd arrow/cpp/  and compile relevant sources by
#         $ cmake . -D ARROW_PARQUET=ON -D PARQUET_BUILD_EXAMPLES=ON -D ARROW_WITH_SNAPPY=ON
#         $ cmake .. -D ARROW_PARQUET=ON ARROW_BUILD_EXAMPLES=ON

lib :
	cmake . -D ARROW_WITH_BROTLI=ON -D ARROW_WITH_BZ2=ON -D ARROW_WITH_LZ4=ON -D ARROW_WITH_SNAPPY=ON -D ARROW_WITH_ZLIB=ON -D ARROW_PARQUET=ON -D ARROW_PYTHON=ON

# cp-src : deps.log
# 	./src_copy.sh

deps.log :
	python3 generate_deps.py reader-writer.cc $@

SRC := $(shell find $(SRC) -name '*.cc')

# OBJ := $(apprefix obj/, $(SRC:%.cc=%.o))
OBJ := $(addprefix $(BIN),$(notdir $(SRC:%.cc=%.o)))

check :
	@echo $(SRC)
	@echo $(OBJ)

# vpath %.cc src/

reader-writer-example : reader-writer.cc $(OBJ) bin/utilmemory.o
	$(CPP) $(CPPFLAGS) $< $(LIBS) -o $@ $(OBJ) bin/utilmemory.o

# $(OBJ) : $(SRC)
# 	$(CPP) $(OPT) -c $< -o $@ -I src/src/
#
# $(BIN)%.o : $(SRC)
# 	$(CPP) $(OPT) -c $< -I src/src/ -o $@

clean-obj :
	rm -f $(OBJ)

# => do build with cmake like here
# https://arrow.apache.org/docs/developers/python.html#build-and-test

#-----------------------------------------------------------------------------#

bin/type.o : src/src/arrow/type.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/result.o : src/src/arrow/result.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/builder.o : src/src/arrow/builder.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/tensor.o : src/src/arrow/tensor.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/table.o : src/src/arrow/table.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/extension_type.o : src/src/arrow/extension_type.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/device.o : src/src/arrow/device.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/memory_pool.o : src/src/arrow/memory_pool.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/datum.o : src/src/arrow/datum.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/record_batch.o : src/src/arrow/record_batch.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/compare.o : src/src/arrow/compare.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/visitor.o : src/src/arrow/visitor.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/chunked_array.o : src/src/arrow/chunked_array.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/status.o : src/src/arrow/status.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/pretty_print.o : src/src/arrow/pretty_print.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/sparse_tensor.o : src/src/arrow/sparse_tensor.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/buffer.o : src/src/arrow/buffer.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/scalar.o : src/src/arrow/scalar.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/string.o : src/src/arrow/util/string.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/utilmemory.o : src/src/arrow/util/memory.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/future.o : src/src/arrow/util/future.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/iterator.o : src/src/arrow/util/iterator.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/compression.o : src/src/arrow/util/compression.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/utf8.o : src/src/arrow/util/utf8.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/time.o : src/src/arrow/util/time.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/cpu_info.o : src/src/arrow/util/cpu_info.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/thread_pool.o : src/src/arrow/util/thread_pool.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/bit_util.o : src/src/arrow/util/bit_util.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/logging.o : src/src/arrow/util/logging.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/basic_decimal.o : src/src/arrow/util/basic_decimal.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/decimal.o : src/src/arrow/util/decimal.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/bit_block_counter.o : src/src/arrow/util/bit_block_counter.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/key_value_metadata.o : src/src/arrow/util/key_value_metadata.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/int_util.o : src/src/arrow/util/int_util.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/io_util.o : src/src/arrow/util/io_util.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/bitmap_ops.o : src/src/arrow/util/bitmap_ops.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/bitmap_builders.o : src/src/arrow/util/bitmap_builders.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/bit_run_reader.o : src/src/arrow/util/bit_run_reader.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/value_parsing.o : src/src/arrow/util/value_parsing.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/string_builder.o : src/src/arrow/util/string_builder.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/formatting.o : src/src/arrow/util/formatting.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/array_primitive.o : src/src/arrow/array/array_primitive.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/array_dict.o : src/src/arrow/array/array_dict.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/builder_binary.o : src/src/arrow/array/builder_binary.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/builder_union.o : src/src/arrow/array/builder_union.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/concatenate.o : src/src/arrow/array/concatenate.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/array_nested.o : src/src/arrow/array/array_nested.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/array_decimal.o : src/src/arrow/array/array_decimal.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/builder_primitive.o : src/src/arrow/array/builder_primitive.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/data.o : src/src/arrow/array/data.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/diff.o : src/src/arrow/array/diff.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/builder_nested.o : src/src/arrow/array/builder_nested.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/builder_decimal.o : src/src/arrow/array/builder_decimal.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/builder_dict.o : src/src/arrow/array/builder_dict.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/array_binary.o : src/src/arrow/array/array_binary.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/builder_adaptive.o : src/src/arrow/array/builder_adaptive.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/array_base.o : src/src/arrow/array/array_base.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/validate.o : src/src/arrow/array/validate.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/builder_base.o : src/src/arrow/array/builder_base.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/util.o : src/src/arrow/array/util.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/caching.o : src/src/arrow/io/caching.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/memory.o : src/src/arrow/io/memory.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/interfaces.o : src/src/arrow/io/interfaces.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/buffered.o : src/src/arrow/io/buffered.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/file.o : src/src/arrow/io/file.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/strtod.o : src/src/arrow/vendored/double-conversion/strtod.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/bignum.o : src/src/arrow/vendored/double-conversion/bignum.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/fixed-dtoa.o : src/src/arrow/vendored/double-conversion/fixed-dtoa.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/fast-dtoa.o : src/src/arrow/vendored/double-conversion/fast-dtoa.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/diy-fp.o : src/src/arrow/vendored/double-conversion/diy-fp.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/double-conversion.o : src/src/arrow/vendored/double-conversion/double-conversion.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/bignum-dtoa.o : src/src/arrow/vendored/double-conversion/bignum-dtoa.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/cached-powers.o : src/src/arrow/vendored/double-conversion/cached-powers.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/api_aggregate.o : src/src/arrow/compute/api_aggregate.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/exec.o : src/src/arrow/compute/exec.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/kernel.o : src/src/arrow/compute/kernel.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/registry.o : src/src/arrow/compute/registry.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/function.o : src/src/arrow/compute/function.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/cast.o : src/src/arrow/compute/cast.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/api_vector.o : src/src/arrow/compute/api_vector.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/api_scalar.o : src/src/arrow/compute/api_scalar.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/codegen_internal.o : src/src/arrow/compute/kernels/codegen_internal.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/column_scanner.o : src/src/parquet/column_scanner.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/statistics.o : src/src/parquet/statistics.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/internal_file_decryptor.o : src/src/parquet/internal_file_decryptor.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/column_writer.o : src/src/parquet/column_writer.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/encryption.o : src/src/parquet/encryption.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/file_reader.o : src/src/parquet/file_reader.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/properties.o : src/src/parquet/properties.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/encryption_internal.o : src/src/parquet/encryption_internal.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/internal_file_encryptor.o : src/src/parquet/internal_file_encryptor.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/types.o : src/src/parquet/types.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/encoding.o : src/src/parquet/encoding.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/metadata.o : src/src/parquet/metadata.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/printer.o : src/src/parquet/printer.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/level_conversion.o : src/src/parquet/level_conversion.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/deprecated_io.o : src/src/parquet/deprecated_io.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/file_writer.o : src/src/parquet/file_writer.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/schema.o : src/src/parquet/schema.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/platform.o : src/src/parquet/platform.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@

bin/column_reader.o : src/src/parquet/column_reader.cc
	$(CPP) $(CPPFLAGS) -c $< $(LIBS) -o $@
