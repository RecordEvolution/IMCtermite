#!/bin/bash

pushd arrow/python
export PYARROW_WITH_PARQUET=1
export PYARROW_WITH_CUDA=0
export PYARROW_WITH_FlIGHT=0
export PYARROW_WITH_DATASET=0
export PYARROW_WITH_ORC=0
export PYARROW_WITH_PLASMA=0
export PYARROW_WITH_S3FS=0
export PYARROW_WITH_HDFS=0
export PYARROW_WITH_GANDIVA=0
# python3 setup.py build_ext --inplace
python3 setup.py install
popd
