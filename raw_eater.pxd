# cython: language_level = 3
# distutils: language = c++

# use some C++ STL libraries
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

#cdef extern from "raweat.hpp":
#    pass

# these method names have to match the C definitions of the methods!!

cdef extern from "raweat.hpp":
    cdef cppclass raw_eater:
        raw_eater(string) except +
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
        void write_data(const char*)
