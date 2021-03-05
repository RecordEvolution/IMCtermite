
from setuptools import setup, Extension
import os

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

setup(
    name="IMCtermite",
    version="1.2.3",
    author="Record Evolution GmbH",
    author_email="mario.fink@record-evolution.de",
    maintainer="Record Evolution GmbH",
    license="MIT",
    description="Enables extraction of measurement data from binary files with extension 'raw' used by proprietary software imcFAMOS/imcSTUDIO and facilitates its storage in open source file formats",
    keywords="IMC raw imcFAMOS imcSTUDIO imcCRONOS",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/RecordEvolution/IMCtermite.git",
    project_urls={
        "Bug Tracker": "https://github.com/RecordEvolution/IMCtermite/issues",
    },
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    ext_modules=[Extension("imc_termite",
                           ["py_imc_termite.cpp"],
                           # libraries_dirs=["cython/"],
                           # include_dirs=["3rdparty/pugixml/","lib/"],
                           # depends=["../lib/tdm_termite.hpp"]
                           language='c++',
                           extra_compile_args=['-std=c++17','-Wno-unused-variable'],
                           extra_link_args=['-std=c++17'],
                           )],
)
