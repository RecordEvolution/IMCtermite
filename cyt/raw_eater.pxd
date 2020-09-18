# cython: language_level = 3
# distutils: language = c++

# use some C++ STL libraries
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

# to include implemenation/definition file
#cdef extern from "raweat.cpp":
#    pass

# these method names have to match the C definitions of the methods!!
#
# for how to overload the constructor see
# https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html
# and propagating exceptions from C++ to Python
# http://docs.cython.org/en/latest/src/userguide/wrapping_CPlusPlus.html#exceptions

cdef extern from "../lib/raweat.hpp":
    cdef cppclass raw_eater:
        # constructor(s)
        raw_eater() except +
        raw_eater(string) except +
        # set new file for decoding
        void set_file(string)
        # perform conversion (pass any C++ exceptions to Python)
        void setup_and_conversion() except +
        # get validity of data format
        bool get_valid()
        # get channel name and unit
        string get_name()
        string get_unit()
        # get time step and time unit
        double get_dt()
        string get_temp_unit()
        # get data array of time and measured quantity's channel
        vector[double] get_time()
        vector[double] get_data()
        # dump all data to .csv
        void write_table(const char*,char delimiter)
