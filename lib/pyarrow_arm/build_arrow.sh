#!/bin/bash

sleep infinity

startts=$(date)
echo "starting build process at ${startts}..."

echo -e "\nhome directory is..."
pwd

echo -e "\ncloning apache/arrow..."
git clone https://github.com/apache/arrow.git --single-branch --depth=1

echo -e "\nls -lh /\n"
ls -lh /

echo -e "\nls -lh arrow/\n"
ls -lh arrow/

echo -e "\nls -lh arrow/python/\n"
ls -lh arrow/python


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
      ..

make -j4
make install
popd

#cython --version
cython3 --version

pushd arrow/python
export ARROW_LIB_DIR=/lib/
export PYARROW_WITH_PARQUET=1
export PYARROW_WITH_CUDA=0
export PYARROW_WITH_FlIGHT=0
export PYARROW_WITH_DATASET=0
export PYARROW_WITH_ORC=0
export PYARROW_WITH_PLASMA=0
export PYARROW_WITH_S3FS=0
export PYARROW_WITH_HDFS=0
export PYARROW_WITH_GANDIVA=0
python3 setup.py build_ext --inplace
popd

echo "  started build process at ${startts} ..."
finishts=$(date)
echo "finishing build process at ${finishts}..."

