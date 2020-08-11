# cython: language_level = 3
# distutils: language = c++

# use some C++ STL libraries
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

# these method names have to match the C++ definitions of the methods!!
cdef extern from "../lib/rawmerge.hpp":
    cdef cppclass raw_merger:
        raw_merger(string) except +
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
        void write_table(const char*,char)
        # add channel and try to merge it
        bool add_channel(string)
        # get total number of (added) channels
        int get_num_channels()
        # get list of channel names
        vector[string] get_channel_names()
        # get data of particular channel
        vector[double] get_channel(int)
        # get total merged time series
        vector[double] get_time_series()
