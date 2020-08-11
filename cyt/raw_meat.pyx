
# from <raw_meat> has to match name of .pxd file and cimport name of class defined in .pxd
from raw_meat cimport raw_merger
import numpy as np
import re

cdef class rawmerger:

    # pointer to C++ instance (since there's no nullary constructor)
    cdef raw_merger *rawit

    def __cinit__(self, string rawfile):
        self.rawit = new raw_merger(rawfile)

    def __dealloc__(self):
        del self.rawit

    def validity(self):
        return self.rawit.get_valid()

    def channel_name(self):
        return self.rawit.get_name()

    def unit(self):
        return self.rawit.get_unit()

    def dt(self):
        return self.rawit.get_dt()

    def time_unit(self):
        return self.rawit.get_temp_unit()

    def get_time(self):
        return self.rawit.get_time()

    def get_channel(self):
        return self.rawit.get_data()

    def write_table(self, const char* csvfile, char delimiter):
        return self.rawit.write_table(csvfile,delimiter)

    def add_channel(self, string rawfile):
        return self.rawit.add_channel(rawfile)

    def get_num_channels(self):
        return self.rawit.get_num_channels()

    def get_channel_names(self):
        return self.rawit.get_channel_names()

    def get_channel_by_index(self, int chidx):
        return self.rawit.get_channel(chidx)

    def get_time_series(self):
        return self.rawit.get_time_series()
