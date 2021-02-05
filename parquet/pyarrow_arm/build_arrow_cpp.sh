#!/bin/bash

mkdir arrow/cpp/build
pushd arrow/cpp/build

cmake -DCMAKE_INSTALL_PREFIX=$ARROW_HOME \
      -DCMAKE_INSTALL_LIBDIR=lib \
      -DARROW_WITH_BZ2=ON \
      -DARROW_WITH_ZLIB=ON \
      -DARROW_WITH_ZSTD=ON \
      -DARROW_WITH_LZ4=ON \
      -DARROW_WITH_SNAPPY=ON \
      -DARROW_WITH_BROTLI=ON \
      -DARROW_PARQUET=ON \
      -DARROW_PYTHON=ON \
      -DARROW_BUILD_TESTS=OFF \
      -DARROW_WITH_HDFS=OFF \
      -DARROW_WITH_IPC=OFF \
      ..

make -j4
make install
popd
