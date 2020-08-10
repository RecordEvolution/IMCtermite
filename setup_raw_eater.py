from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

extensions = Extension(
    name="raw_eater",
    sources=["cyt/raw_eater.pyx"],
    # libraries=[""],
    library_dirs=["src"],
    include_dirs=["src"],
    language='c++',
    extra_compile_args=['-std=c++11','-Wno-unused-variable'],
    extra_link_args=['-std=c++11'],
    #extra_objects=["lib/parquet/libarrow.so.200.0.0"],
)

setup(
    name="raw_eater",
    ext_modules=cythonize(extensions)
)
