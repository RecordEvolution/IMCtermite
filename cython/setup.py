from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

extensions = Extension(
    name="imc_termite",
    sources=["cython/py_imc_termite.pyx"],
    # libraries=[""],
    # library_dirs=["lib"],
    include_dirs=["lib"],
    language='c++',
    extra_compile_args=['-std=c++17','-Wno-unused-variable'],
    extra_link_args=['-std=c++17'],
)

setup(
    version='0.1',
    description='IMCtermite cython extension',
    author='Record Evolution GmbH',
    author_email='mario.fink@record-evolution.de',
    url='https://github.com/RecordEvolution/IMCtermite.git',
    name="imc_termite",
    ext_modules=cythonize(extensions,force=True)
)
